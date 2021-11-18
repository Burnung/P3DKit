#!/bin/bash -x
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOP_DIR=${SCRIPT_DIR}/../..
mkdir -p build
pushd build

BUILD_TYPE=Release
cmake -DCMAKE_TOOLCHAIN_FILE=${TOP_DIR}/cmake/ios.toolchain.cmake -DINCLUDE_YCNN=false -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DTARGET_PLATFORM=IOS ${TOP_DIR} && make -j32

libtool -static -o libmmu_lib.a $(find . -name "lib*.a" ! -name "*liblibpng.a*")

cp libmmu_lib.a $TOP_DIR/distribute/mmulib_ios/
cp $TOP_DIR/pubs/base/mmu_lib_inf.h $TOP_DIR/distribute/mmulib_ios/
cp $TOP_DIR/pubs/base/mmu_pubs_common.h $TOP_DIR/distribute/mmulib_ios/
cp pubs/base/mmu_pubs_config.h $TOP_DIR/distribute/mmulib_ios/

popd

echo "!!! All success! Check mmuFuture/distribute/mmulib_ios/ dir, which is updated !!!"
