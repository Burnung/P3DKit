#!/bin/bash -x
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

mkdir -p P3D/
pushd P3D/

cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../../../cmake/ios.toolchain.cmake  \
	-DCMAKE_BUILD_TYPE=Release -DTARGET_PLATFORM=IOS ../../..

popd
