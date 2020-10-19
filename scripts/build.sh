#!/usr/bin/env bash
set -e

OBS_STUDIO_VERSION=25.0.7
OBS_DEPS_VERSION=2020-08-30

BASE_DIR=`pwd`
LIBOBS_DIR=${BASE_DIR}/libobs
OBS_STUDIO_DIR="${LIBOBS_DIR}/obs-studio-${OBS_STUDIO_VERSION}"
MACOS_DEPS_DIR="${LIBOBS_DIR}/macos-deps-${OBS_DEPS_VERSION}"
PREBUILD_DIR=prebuild

# Clone obs studio
if [[ ! -d "${OBS_STUDIO_DIR}" ]]; then
  pushd ${LIBOBS_DIR}
  git clone --recursive -b ${OBS_STUDIO_VERSION} --single-branch https://github.com/obsproject/obs-studio.git "obs-studio-${OBS_STUDIO_VERSION}"
  popd
fi

# Download macos-deps
if [[ ! -d "${MACOS_DEPS_DIR}" ]]; then
  pushd ${LIBOBS_DIR}
  wget "https://github.com/obsproject/obs-deps/releases/download/${OBS_DEPS_VERSION}/macos-deps-${OBS_DEPS_VERSION}.tar.gz"
  tar -xf macos-deps-${OBS_DEPS_VERSION}.tar.gz
  mv obsdeps macos-deps-${OBS_DEPS_VERSION}
  rm -f macos-deps-${OBS_DEPS_VERSION}.tar.gz
  popd
fi

# Compile libobs
echo "Compile libobs"
pushd ${OBS_STUDIO_DIR}
rm -rf build && mkdir build && cd build
if [[ "$OSTYPE" == "darwin"* ]]; then
  # Compile MacOS
  cmake -DCMAKE_INSTALL_PREFIX=${LIBOBS_DIR}/dist -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 -DSWIGDIR="${MACOS_DEPS_DIR}" -DDepsPath="${MACOS_DEPS_DIR}" -DDISABLE_UI=TRUE -DDISABLE_PYTHON=ON ..
  cmake --build . --target install --config Release -- -j 4
else
  >&2 echo "Only macos is supported!"
  exit 1
fi
popd

# Compile libobs-node
echo "Compile libobs-node"
rm -rf build
cmake-js configure --CDOBS_STUDIO_DIR="${LIBOBS_DIR}/dist"
cmake --build build --config Release

# Copy obs files to prebuild
mkdir -p ${PREBUILD_DIR}/libobs
cp -r ${LIBOBS_DIR}/dist/{bin,data,obs-plugins} ${PREBUILD_DIR}/libobs

# Copy dependencies to prebuild
mkdir -p ${PREBUILD_DIR}/libobs/deps
cp -r ${MACOS_DEPS_DIR}/{bin,lib} ${PREBUILD_DIR}/libobs/deps

# Copy libobs_node
cp build/Release/libobs_node.node ${PREBUILD_DIR}

# Fix loader path
sh scripts/fix-loader-path-macos.sh

# Compress and Upload assets
ts-node src/scripts/upload.ts "${PREBUILD_DIR}"