
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

echo "XDI_HOME          = $XDI_HOME"
echo "RISCV             = $RISCV"
echo "VIVADO_TOOL_DIR   = $VIVADO_TOOL_DIR"

echo "------------------------------[Finished]-------------------------------"
