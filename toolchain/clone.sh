#!/bin/bash

DIR_BASE=${XDI_HOME}/work/toolchain

DIR_GCC=$DIR_BASE/riscv-gcc
DIR_BINUTILS=$DIR_BASE/riscv-binutils
DIR_NEWLIB=$DIR_BASE/riscv-newlib

BRANCH_NAME=XDI-CompactToolChains

# Known good git commit hashes to apply patches too
COMMIT_BINUTILS=d91cadb45f3ef9f96c6ebe8ffb20472824ed05a7
COMMIT_GCC=54945eb8ad5a066da2d4e6a62ffeb513d341eb41
COMMIT_NEWLIB=f289cef6be67da67b2d97a47d6576fa7e6b4c858

BINUTILS_PATCH=${XDI_HOME}/toolchain/binutils.patch

echo "Setting up toolchain..."
echo "---------------------------------------------------"
echo "Download Dir    : $DIR_BASE"
echo "GCC Commit      : $COMMIT_GCC"
echo "Binutils Commit : $COMMIT_BINUTILS"
echo ""
echo "DIR_GCC         = $DIR_GCC"
echo "DIR_BINUTILS    = $DIR_BINUTILS"
echo "DIR_NEWLIB      = $DIR_NEWLIB"
echo ""
echo "Branch Name     = $BRANCH_NAME"
echo "---------------------------------------------------"

set -e
set -x

# ------ Binutils ----------------------------------------------------------

if [ ! -d $DIR_BINUTILS ]; then
    git clone https://github.com/riscv/riscv-binutils-gdb.git $DIR_BINUTILS
    cd $DIR_BINUTILS
    git checkout -B $BRANCH_NAME $COMMIT_BINUTILS 
    git apply ${BINUTILS_PATCH}
    git add --all
    cd -
fi

# ------ GCC ---------------------------------------------------------------

if [ ! -d $DIR_GCC ]; then
    git clone https://github.com/riscv/riscv-gcc.git $DIR_GCC
    cd $DIR_GCC
    git checkout -B $BRANCH_NAME $COMMIT_GCC 
    ./contrib/download_prerequisites
    cd -
fi

# ------ NewLib ------------------------------------------------------------

if [ ! -d $DIR_NEWLIB ]; then
    git clone https://github.com/riscv/riscv-newlib.git $DIR_NEWLIB
    cd $DIR_NEWLIB
    git checkout -b $BRANCH_NAME $COMMIT_NEWLIB
    cd -
fi

