
#!/bin/sh

echo "-------------------------[Setting Up Project]--------------------------"

# Top level environment variables
export XDI_HOME=`pwd`

if [ -z ${VIVADO_TOOL_DIR} ]; then
    echo "ERROR: VIVADO_TOOL_DIR not set"
else 
	source $VIVADO_TOOL_DIR/settings64.sh
fi

if [ -z ${RISCV} ]; then
    echo "[ERROR] No 'RISCV' environment variable defined"
fi	

export soc_dir=${XDI_HOME}/soc
export LIBSCARV=${XDI_HOME}/../Forked/libscarv
export SASS_RIG=${XDI_HOME}/../Forked/fw-acquisition

echo "XDI_HOME          = $XDI_HOME"
echo "LIBSCARV          = $LIBSCARV"
echo "SASS_RIG          = $SASS_RIG"
echo "RISCV             = $RISCV"
echo "VIVADO_TOOL_DIR   = $VIVADO_TOOL_DIR"

echo "------------------------------[Finished]-------------------------------"
