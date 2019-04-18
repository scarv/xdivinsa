
#!/bin/sh

echo "-------------------------[Setting Up Project]--------------------------"

# Top level environment variables
export XDI_HOME=`pwd`
export soc_dir=${XDI_HOME}/soc

if [ -z ${VIVADO_TOOL_DIR} ]; then
    echo "ERROR: VIVADO_TOOL_DIR not set"
else 
	source $VIVADO_TOOL_DIR/settings64.sh
fi

if [ -z ${RISCV} ]; then
    echo "[ERROR] No 'RISCV' environment variable defined"
fi	

if [ -z ${SASS_RIG} ]; then
    echo "[ERROR] No 'SASS_RIG' environment variable defined"
fi

if [ -z ${LIBSCARV} ]; then
    echo "[ERROR] No 'LIBSCARV' environment variable defined"
fi


export CORE=rocket-xdivinsa-25M
export TARGET=sakura-x

echo "XDI_HOME          = $XDI_HOME"
echo "LIBSCARV          = $LIBSCARV"
echo "SASS_RIG          = $SASS_RIG"
echo "RISCV             = $RISCV"
echo "VIVADO_TOOL_DIR   = $VIVADO_TOOL_DIR"
echo
echo "CORE              = $CORE"
echo "TARGET            = $TARGET"

echo "------------------------------[Finished]-------------------------------"
