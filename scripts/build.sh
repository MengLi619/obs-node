#!/usr/bin/env bash
set -e

OBS_STUDIO_VERSION=25.0.7
OBS_DEPS_VERSION=2020-08-30

BASE_DIR=`pwd`
OBS_STUDIO_DIR="${BASE_DIR}/libobs/obs-studio-${OBS_STUDIO_VERSION}"
MACOS_DEPS_DIR="${BASE_DIR}/libobs/macos-deps-${OBS_DEPS_VERSION}"
DIST_DIR=build/dist

# Clone obs studio
if [[ ! -d "${OBS_STUDIO_DIR}" ]]; then
  pushd libobs
  git clone --recursive -b ${OBS_STUDIO_VERSION} --single-branch https://github.com/obsproject/obs-studio.git "obs-studio-${OBS_STUDIO_VERSION}"
  popd
fi

# Download macos-deps
if [[ ! -d "${MACOS_DEPS_DIR}" ]]; then
  pushd libobs
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
  cmake -DCMAKE_INSTALL_PREFIX=${OBS_STUDIO_DIR}/build/dist -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 -DSWIGDIR="${MACOS_DEPS_DIR}" -DDepsPath="${MACOS_DEPS_DIR}" -DDISABLE_UI=TRUE -DDISABLE_PYTHON=ON ..
  cmake --build . --target install --config Release -- -j 4
else
  >&2 echo "Only macos is supported!"
  exit 1
fi
popd

# Compile libobs-node
echo "Compile libobs-node"
rm -rf build
cmake-js configure --CDOBS_STUDIO_DIR="${OBS_STUDIO_DIR}/build/dist"
cmake --build build --config Release

# Copy obs files to dist
mkdir -p ${DIST_DIR}/libobs
cp -r ${OBS_STUDIO_DIR}/build/dist/{bin,data,obs-plugins} ${DIST_DIR}/libobs

# Copy Dependencies to dist
mkdir -p ${DIST_DIR}/libobs/deps
cp -r ${MACOS_DEPS_DIR}/{bin,lib} ${DIST_DIR}/libobs/deps

# Copy libobs_node
cp build/Release/libobs_node.node ${DIST_DIR}

# Fix loader path
sh scripts/fix-loader-path-macos.sh