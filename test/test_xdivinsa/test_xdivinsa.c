#include "soc.h"
#include "common.h"
#include "xdivinsa.h"

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
volatile unsigned int a, b, c0, c1;
volatile unsigned long long int c;
unsigned int t_beg,t_end;

CiRand(a); //init

putstr("\n\n\nHello from RISC-V CPU to test the xdivinsa instructions!\n\n");

putstr("\nTest reading TRNG using custom instruction\n");
for (i=1;i<7;i++){
	t_beg=rdcycle();
	CiRand(c0);
	t_end=rdcycle();
	uint2str(i, st); putstr(st); putstr(":cirand = "); uint2str(c0, st); putstr(st); putstr(" process time= "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
	}

putstr("\nTest XDivinsa instructions\n");
CiRand(a); 
//a &= 0xFFFF;
putstr("first  number a="); uint2str(a, st); putstr(st); putstr(";\n");

CiRand(b); 
//b &= 0xFFFF;
putstr("second number b="); uint2str(b, st); putstr(st); putstr(";\n");

putstr("sizeof(c) ="); uint2str(sizeof(c), st); putstr(st); putstr(";\n");

putstr("Running normal instruction\n");
for (i=1;i<7;i++){
	
	t_beg=rdcycle();
	c = (long long int) a+b;
	t_end=rdcycle();
	//printf("%d: add = %d process time= %d\n", i, c, t_end-t_beg);
	uint2str(i, st); putstr(st); putstr("  :addi = "); uint2str(c, st); putstr(st); putstr(" process time= "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
}
  
for (i=1;i<7;i++){
	t_beg=rdcycle(); 
	c =(long long int) a*b;
	t_end=rdcycle();
	uint2str(i, st); putstr(st); putstr("  :mult = "); uint2str(c, st); putstr(st); putstr(" process time= "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
	}

putstr("\nRunning custom instruction f1\n"); 
for (i=1;i<7;i++){ 
	t_beg=rdcycle(); 
	CiAddi_f1(c1, c0, a, b);   
	t_end=rdcycle();
	uint2str(i, st); putstr(st); putstr(":ciaddi = "); uint2str(c0, st); putstr(st); putstr(", "); uint2str(c1, st); putstr(st); 
	putstr(" process time= "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
	} 
for (i=1;i<7;i++){
	t_beg=rdcycle();
	CiMult_f1(c1, c0, a, b);
	t_end=rdcycle();
	uint2str(i, st); putstr(st); putstr(":cimult = "); uint2str(c0, st); putstr(st); putstr(", "); uint2str(c1, st); putstr(st);
	putstr(" process time= "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
	}

putstr("\nRunning custom instruction f2\n"); 
for (i=1;i<7;i++){ 
	t_beg=rdcycle();
	CiAddi_f2(c, a, b);   
	t_end=rdcycle();
	uint2str(i, st); putstr(st); putstr(":ciaddi = "); uint2str(c, st); putstr(st); putstr(" process time= "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
	} 
for (i=1;i<7;i++){
	t_beg=rdcycle();
	CiMult_f2(c, a, b);
	t_end=rdcycle();
	uint2str(i, st); putstr(st); putstr(":cimult = "); uint2str(c, st); putstr(st); putstr(" process time= "); uint2str(t_end-t_beg, st); putstr(st); putstr(";\n"); 
	}
}
void riscv_irq_delegated_handler() {

}
