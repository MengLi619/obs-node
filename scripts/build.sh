#!/usr/bin/env bash
set -e

OBS_STUDIO_VERSION=26.0.2
MAXOS_DEPS_VERSION=2020-08-30

BASE_DIR="$(pwd)"
BUILD_DIR="${BASE_DIR}/build"
OBS_STUDIO_BUILD_DIR="${BASE_DIR}/obs-studio-build"
OBS_STUDIO_DIR="${OBS_STUDIO_BUILD_DIR}/obs-studio-${OBS_STUDIO_VERSION}"
MACOS_DEPS_DIR="${OBS_STUDIO_BUILD_DIR}/macos-deps-${MAXOS_DEPS_VERSION}"
OBS_INSTALL_PREFIX="${OBS_STUDIO_BUILD_DIR}/obs-installed"
PREBUILD_DIR="${BASE_DIR}/prebuild"

BUILD_TYPE=$1
if [[ $BUILD_TYPE != 'all' && $BUILD_TYPE != 'obs-studio' && $BUILD_TYPE != 'obs-node' ]]; then
  >&2 echo "The first argument should be 'all', 'obs-studio' or 'obs-node'"
  exit 1
fi

RELEASE_TYPE="${2:-Release}"
if [[ $RELEASE_TYPE != 'Release' && $RELEASE_TYPE != 'Debug' ]]; then
  >&2 echo "The second argument should be 'Release' or 'Debug'"
  exit 1
fi

mkdir -p "${BUILD_DIR}" "${PREBUILD_DIR}"
if [[ $BUILD_TYPE == 'all' || $BUILD_TYPE == 'obs-studio' ]]; then
  echo "Building obs-studio"
  # Clone obs studio
  if [[ ! -d "${OBS_STUDIO_DIR}" ]]; then
    pushd "${BUILD_DIR}"
    git clone --recursive -b ${OBS_STUDIO_VERSION} --single-branch https://github.com/obsproject/obs-studio.git "obs-studio-${OBS_STUDIO_VERSION}"
    popd
  fi

  # Download macos-deps
  if [[ "$OSTYPE" == "darwin"* ]]; then
    if [[ ! -d "${MACOS_DEPS_DIR}" ]]; then
      pushd "${BUILD_DIR}"
      wget "https://github.com/obsproject/obs-deps/releases/download/${MAXOS_DEPS_VERSION}/macos-deps-${MAXOS_DEPS_VERSION}.tar.gz"
      tar -xf macos-deps-${MAXOS_DEPS_VERSION}.tar.gz
      mv obsdeps macos-deps-${MAXOS_DEPS_VERSION}
      rm -f macos-deps-${MAXOS_DEPS_VERSION}.tar.gz
      popd
    fi
  fi

  # Compile obs-studio
  echo "Compile obs-studio"
  pushd "${OBS_STUDIO_DIR}"
  rm -rf build && mkdir -p build && cd build
  if [[ "$OSTYPE" == "darwin"* ]]; then
    # Compile MacOS
    rm -rf "${OBS_INSTALL_PREFIX}"
    cmake -DCMAKE_INSTALL_PREFIX="${OBS_INSTALL_PREFIX}" \
          -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 \
          -DSWIGDIR="${MACOS_DEPS_DIR}" \
          -DDepsPath="${MACOS_DEPS_DIR}" \
          -DDISABLE_UI=TRUE \
          -DDISABLE_PYTHON=ON \
          -DCMAKE_BUILD_TYPE="${RELEASE_TYPE}" \
          ..
    cmake --build . --target install -- -j 4
  elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Compile Linux
    rm -rf "${OBS_INSTALL_PREFIX}"
    cmake -DCMAKE_INSTALL_PREFIX="${OBS_INSTALL_PREFIX}" \
          -DUNIX_STRUCTURE=0 \
          -DDISABLE_UI=TRUE \
          -DDISABLE_PYTHON=ON \
          -DCMAKE_BUILD_TYPE="${RELEASE_TYPE}" \
          ..
    cmake --build . --target install -- -j 4
  else
    >&2 echo "Only MaxOS and Linux is supported"
    exit 1
  fi
  popd

  # Copy obs files to prebuild
  if [[ "$OSTYPE" == "darwin"* ]]; then
    cp -r "${OBS_INSTALL_PREFIX}"/{bin,data,obs-plugins} "${PREBUILD_DIR}/obs-studio"
  elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    cp -r "${OBS_INSTALL_PREFIX}"/{bin,lib,data,obs-plugins} "${PREBUILD_DIR}/obs-studio"
  fi

  # Copy macos dependencies to prebuild
  if [[ "$OSTYPE" == "darwin"* ]]; then
    mkdir -p "${PREBUILD_DIR}/macos-deps"
    cp -r "${MACOS_DEPS_DIR}"/{bin,lib} "${PREBUILD_DIR}/macos-deps"
  fi

  # Fix loader path for macos
  if [[ "$OSTYPE" == "darwin"* ]]; then
    sh scripts/fix-loader-path-macos.sh "/tmp/obsdeps" "@loader_path/../../macos-deps" "${PREBUILD_DIR}/obs-studio/bin/*.{dylib,so}"
    sh scripts/fix-loader-path-macos.sh "/tmp/obsdeps" "@loader_path/../../macos-deps" "${PREBUILD_DIR}/obs-studio/obs-plugins/*.so"
    sh scripts/fix-loader-path-macos.sh "/tmp/obsdeps/bin" "@loader_path" "${PREBUILD_DIR}/macos-deps/bin/*.dylib"
    sh scripts/fix-loader-path-macos.sh "/tmp/obsdeps/lib" "@loader_path/../lib" "${PREBUILD_DIR}/macos-deps/bin/*.dylib"
    sh scripts/fix-loader-path-macos.sh "/tmp/obsdeps/lib" "@loader_path" "${PREBUILD_DIR}/macos-deps/lib/*.dylib"
  fi
fi

if [[ $BUILD_TYPE == 'all' || $BUILD_TYPE == 'obs-node' ]]; then
  echo "Building obs-node"
  rm -rf build/{CMakeFiles,Debug,Release,cmake_install.cmake,CMakeCache.txt,Makefile}
  if [[ ! -d "$BASE_DIR/node_modules/" ]]; then
    npm ci
  fi
  node node_modules/.bin/cmake-js configure \
    "$([[ $RELEASE_TYPE == 'Debug' ]] && echo '-D')" \
    --CDOBS_STUDIO_DIR="${OBS_INSTALL_PREFIX}"
  cmake --build build

  # Copy obs-node to prebuild
  echo "Copy ${BUILD_DIR}/${RELEASE_TYPE}/obs-node.node to ${PREBUILD_DIR}"
  cp "${BUILD_DIR}/${RELEASE_TYPE}/obs-node.node" "${PREBUILD_DIR}"

  # Fix obs-node loader path
  if [[ "$OSTYPE" == "darwin"* ]]; then
    install_name_tool -id "obs-node.node" "${PREBUILD_DIR}/obs-node.node"
    install_name_tool -change "@rpath/libobs.0.dylib" "@loader_path/obs-studio/bin/libobs.0.dylib" "${PREBUILD_DIR}/obs-node.node"
  elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    patchelf --replace-needed libobs.so.0 '$ORIGIN/obs-studio/bin/64bit/libobs.so.0' "${PREBUILD_DIR}/obs-node.node"
  fi
fi