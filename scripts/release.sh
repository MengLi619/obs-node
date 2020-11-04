#!/usr/bin/env bash
set -e

RELEASE_TYPE=${1:-patch}

npm version "${RELEASE_TYPE}"
git push && git push --tags