#include "soc.h"
#include "common.h"
#include "xdivinsa.h"

int riscv_main() {

int  i;
char st[10]={0};

register uint32_t r, a, b;
unsigned int cyc1, cyc2, cyc3, cyc4;

uart_init();
XD_RDN(a); //init


putstr("\n\n\nTest the xdivinsa instructions!\n\n");

putstr("\nTest read cycle instruction\n");
for (i=0;i<7;i++){
    MEASURE(cyc1, )
	uint2str(i, st); putstr(st); putstr(" process time of two continued rdcycle instructions = "); uint2str(cyc1, st); putstr(st); putstr(";\n"); 
	}

putstr("\nTest reading TRNG using custom instruction\n");
for (i=0;i<7;i++){

	MEASURE(cyc2,XD_RDN(r));

	uint2str(i, st); putstr(st); putstr(":cirand = "); uint2str(r, st); putstr(st); putstr(" process time= "); uint2str(cyc2, st); putstr(st); putstr(";\n"); 
	}

putstr("\nTest XDivinsa instructions\n");
XD_RDN(a); 
putstr("first  number a="); uint2str(a, st); putstr(st); putstr(";\n");
XD_RDN(b); 
putstr("second number b="); uint2str(b, st); putstr(st); putstr(";\n");

putstr("Running normal instruction\n");
for (i=0;i<7;i++){	
	MEASURE(cyc3, ISA_ADD(r, a, b);)
	uint2str(i, st); putstr(st); putstr("  :add = "); uint2str(r, st); putstr(st); putstr(" process time= "); uint2str(cyc3, st); putstr(st); putstr(";\n"); 
}  
for (i=0;i<7;i++){
	MEASURE(cyc4, ISA_XOR(r, a, b);)
	uint2str(i, st); putstr(st); putstr("  :xor = "); uint2str(r, st);  putstr(st); putstr(" process time= "); uint2str(cyc4, st); putstr(st); putstr(";\n"); 
	}

putstr("\nRunning custom instruction \n"); 
for (i=0;i<7;i++){ 
	MEASURE(cyc3, XD_ADD(r, a, b);)   
	uint2str(i, st); putstr(st); putstr(":xdiadd = "); uint2str(r, st); putstr(st); putstr(" process time= "); uint2str(cyc3, st); putstr(st); putstr(";\n"); 
	} 
for (i=0;i<7;i++){
	MEASURE(cyc4, XD_XOR(r, a, b);)
	uint2str(i, st); putstr(st); putstr(":xdixor = "); uint2str(r, st); putstr(st); putstr(" process time= "); uint2str(cyc4, st); putstr(st); putstr(";\n"); 
	}

}

void riscv_irq_delegated_handler() {
}
