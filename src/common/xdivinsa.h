#ifndef XDiVINSA_H
#define XDiVINSA_H
#include <stdint.h>

#define XD_RDN(r) asm volatile     (" xdirdn   %0, %1    \n\t" :"=r" (r) : "r" (r)); 

#ifdef XDIVINSA	
		#define XSTR(x) #x
		#define opt(x) XSTR(x)
    	#define addins "xdi" opt(XDIVINSA) "add"
    	#define xorins "xdi" opt(XDIVINSA) "xor"
#else
		#define addins "add"
		#define xorins "xor"
#endif

#define XD_ADD( r,a,b) asm volatile ( addins "   %0, %1, %2\n\t"   \
                                     "add        %0, %0, x0\n\t"   \
                                      :"=r" (r) : "r" (a), "r" (b)); 
#define XD_XOR( r,a,b) asm volatile ( xorins "   %0, %1, %2\n\t"   \
                                     "xor        %0, %0, x0\n\t"   \
                                      :"=r" (r) : "r" (a), "r" (b));

#define ISA_ADD(r,a,b) asm volatile ( "add       %0, %1, %2\n\t"   \
                                      "add       %0, %0, x0\n\t"   \
                                      :"=r" (r) : "r" (a), "r" (b)); 
#define ISA_XOR(r,a,b) asm volatile ( "xor       %0, %1, %2\n\t"   \
                                      "xor       %0, %0, x0\n\t"   \
                                      :"=r" (r) : "r" (a), "r" (b)); 

#endif  
