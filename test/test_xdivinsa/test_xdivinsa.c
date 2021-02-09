#include "soc.h"
#include "common.h"
#include "xdivinsa.h"

#define SIZEOF(x) ( sizeof(x)     )
#define BITSOF(x) ( sizeof(x) * 8 )


unsigned int uint2str(unsigned long long int n, char *st){
	unsigned int len;
	unsigned int temp;
	short i;
	len = 0;
	
	for(temp=n;temp!=0;temp/=10,len++);    
    
	for(i=len-1,temp=n;i>=0;i--) {
		st[i]=(temp%10)+0x30;
      	temp/=10;
    }

    n=n>>32;
	if (n > 0){
    	st[len]='+'; len++;
		int j=len;
		for(temp=n;temp!=0;temp/=10,len++);    
    
		for(i=len-1,temp=n;i>=j;i--) {
			st[i]=(temp%10)+0x30;
      		temp/=10;
    	}
    
		st[len]='*'; len++;
		st[len]='2'; len++; st[len]='^'; len++; st[len]='3'; len++;st[len]='2'; len++;
	}
	st[len]=0;
	return len;
}


int riscv_main() {

char st[10]={0};
unsigned int len;

uart_init();

int i;
volatile unsigned int c0, c1, a, b, g, h;
volatile unsigned long int c;
unsigned int t_beg,t_end;

XD_RDN(a); //init

putstr("\n\n\nHello from RISC-V CPU to test the xdivinsa instructions!\n\n");

putstr("\nTest read cycle instruction\n");
//putstr("sizeof(long int) ="); uint2str(sizeof(c), st); putstr(st); putstr(";\n");


for (i=1;i<7;i++){
	t_beg=rdcycle();
	t_end=rdcycle();
	uint2str(i, st); putstr(st); putstr(" process time of two continued rdcycle call = "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
	}

putstr("\nTest reading TRNG using custom instruction\n");
for (i=1;i<7;i++){
	t_beg=rdcycle();
	XD_RDN(c0);
	t_end=rdcycle();
	uint2str(i, st); putstr(st); putstr(":cirand = "); uint2str(c0, st); putstr(st); putstr(" process time= "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
	}

putstr("\nTest XDivinsa instructions\n");

XD_RDN(a); 
putstr("first  number a="); uint2str(a, st); putstr(st); putstr(";\n");

XD_RDN(b); 
putstr("second number b="); uint2str(b, st); putstr(st); putstr(";\n");

putstr("Running normal instruction\n");
for (i=1;i<7;i++){
	
	t_beg=rdcycle();
	c = (long int) a+b;
	t_end=rdcycle();
	//printf("%d: add = %d process time= %d\n", i, c, t_end-t_beg);
	uint2str(i, st); putstr(st); putstr("  :addi = "); uint2str(c, st); putstr(st); putstr(" process time= "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
}
  
for (i=1;i<7;i++){
	t_beg=rdcycle(); 
	c =(long int) a^b;
	t_end=rdcycle();
	uint2str(i, st); putstr(st); putstr("  :xori = "); uint2str(c, st); putstr(st); putstr(" process time= "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
	}

putstr("\nRunning custom instruction \n"); 
for (i=1;i<7;i++){ 
	t_beg=rdcycle(); 
	XD_ADD(c0, a, b);   
	t_end=rdcycle();
	uint2str(i, st); putstr(st); putstr(":ciaddi = "); uint2str(c0, st); putstr(st); putstr(", "); 
	putstr(" process time= "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
	} 

for (i=1;i<7;i++){
	t_beg=rdcycle();
	XD_XOR(c0, a, b);
	t_end=rdcycle();
	uint2str(i, st); putstr(st); putstr(":cixori = "); uint2str(c0, st); putstr(st); putstr(", "); 
	putstr(" process time= "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
	}

}


void riscv_irq_delegated_handler() {

}
