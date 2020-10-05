#!/usr/bin/env bash
set -e

RELEASE_TYPE=${1:-patch}

# Prebuild libobs
sh scripts/build.sh

# Update version
npm version ${RELEASE_TYPE}

# Compress and Upload assets
ts-node scripts/upload.ts

# Git push
