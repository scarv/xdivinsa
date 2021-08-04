#!/bin/bash

DIR_BASE=${XDI_HOME}/work/toolchain

DIR_GCC=$DIR_BASE/riscv-gcc
DIR_BINUTILS=$DIR_BASE/riscv-binutils
DIR_NEWLIB=$DIR_BASE/riscv-newlib

DIR_GCC_BUILD=$DIR_GCC-build
DIR_BINUTILS_BUILD=$DIR_BINUTILS-build
DIR_NEWLIB_BUILD=$DIR_NEWLIB-build

TARGET_ARCH=riscv64-unknown-elf
ARCH_STRING=rv32imac
ABI_STRING=ilp32
INSTALL_DIR=${RISCV}

echo "Setting up toolchain..."
echo "---------------------------------------------------"
echo "Download Dir    : $DIR_BASE"
echo "Installation Dir: $INSTALL_DIR"
echo "Target Arch     : $TARGET_ARCH / $ARCH_STRING / $ABI_STRING"
echo ""
echo "DIR_GCC         = $DIR_GCC"
echo "DIR_BINUTILS    = $DIR_BINUTILS"
echo "DIR_NEWLIB      = $DIR_NEWLIB"
echo "---------------------------------------------------"

#
# Check if the directory exists. If so, delete it and create fresh.
#
function refresh_dir {
if [ -d $1 ]; then
    rm -rf $1
fi
mkdir -p $1
}

mkdir -p $INSTALL_DIR

# ------ Binutils ----------------------------------------------------------
refresh_dir  $DIR_BINUTILS_BUILD
cd   $DIR_BINUTILS_BUILD
$DIR_BINUTILS/configure \
    --prefix=$INSTALL_DIR \
    --target=$TARGET_ARCH \
    --with-arch=$ARCH_STRING --with-abi=$ABI_STRING
make -j$(nproc)
make install

# ------ GCC ---------------------------------------------------------------
refresh_dir  $DIR_GCC_BUILD
cd           $DIR_GCC_BUILD
$DIR_GCC/configure \
    --prefix=$INSTALL_DIR \
    --enable-languages=c \
    --disable-libssp \
    --disable-float --disable-atomic \
    --target=$TARGET_ARCH \
    --with-arch=$ARCH_STRING --with-abi=$ABI_STRING
make -j$(nproc)
make install

# ------ Newlib ------------------------------------------------------------
refresh_dir  $DIR_NEWLIB_BUILD
cd           $DIR_NEWLIB_BUILD
export PATH="$RISCV/bin:$PATH"
$DIR_NEWLIB/configure \
    --prefix=$INSTALL_DIR \
    --target=$TARGET_ARCH \
    --with-arch=$ARCH_STRING --with-abi=$ABI_STRING
make -j$(nproc)
make install


