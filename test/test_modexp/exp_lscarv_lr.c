#include "soc.h"
#include "common.h"
#define  W 3
#define PW 4  //(2^(W-1))
int NL= 2;

mrz_ctx_t ctx;

// Left to Right Window format of the key.
void LtRWinFrm(uint8_t *d, const limb_t* k, int l_k){
	int i,j;
	limb_t t;
	uint8_t wi;

	for( int i = 0; i < l_k; i++ ) {
		t=k[i];
		for( int j = 0; j< BITSOF( limb_t ); j++ ) {
			d[ i*BITSOF( limb_t ) +j] = t & 1;
			t >>=1;
		}
	}
	
	i= l_k*BITSOF( limb_t )-1;
	while (i>=0){
		if (d[i]){
			//printf("i=%d, ", i);
			j=(i>=W)?W:i+1;
			while (d[i-j+1]==0){j--;}
			i = i-j+1;
			wi=0;
			while (j>0){ 
				wi<<=1;
				if (d[i+j-1]) {wi+=1;}  
				d[i+j-1] = 0;
				j--;				
			}		
			d[i]= wi;			 
		}
		i --;		
	}
}

void exp_lr( const mrz_ctx_t* ctx, mrz_t r, const mrz_t x, const uint8_t* d, int l_k ) {
	mrz_t t, x2;
	mrz_t b[PW];
	int i;

	memcpy( b[0], ctx->rho_1, SIZEOF( mrz_t ) );
//Precomputation
	mrz_mul( ctx, b[0], b[0], x );	// b[0]=x
	mrz_mul( ctx, x2, b[0], x );	// x2 = x*x 

	for (i = 1; i < PW; i++) {
		//printf(" size (i=%d, 2*i+1=%d, 2^(w-1)= %d)\n", i, 2*i+1, 1<<(w-1));
		mrz_mul( ctx, b[i], b[i-1], x2 );   //b[i]=b[i-1]*x^2		
	}

//calculation loop
	memcpy( t, ctx->rho_1, SIZEOF( mrz_t ) );
	i = l_k*BITSOF( limb_t ) - 1;
	while (i>=0){
		mrz_mul( ctx, t, t, t );
		if (d[i] != 0){
			mrz_mul( ctx, t, t, b[(d[i]>>1)]);			
		}	
		i --;	
	}
	memcpy( r,          t, SIZEOF( mrz_t ) );
}

int riscv_main() {

	gpio_init();
    uart_init();
	uint32_t t_beg,t_end;

    putstr("\nExp left-to-right sliding window (w=3) \n");	

	uint8_t d[NL * BITSOF( limb_t )];	

	mrz_t N; int l_N;
    mrz_t x; int l_x;
    mrz_t k; int l_k;
    mrz_t r; int l_r;
	
    uint8_t  Narr[8]= {0x3f,0x40,0x10, 0, 0, 0, 0, 0};
	uint8_t  xarr[8]= {0x63,   0,   0, 0, 0, 0, 0, 0};
	uint8_t k1arr[8]= {0x4f,0xb4,0x0f, 0, 0, 0, 0, 0};
	uint8_t k2arr[8]= {0x19,0x83,0x05, 0, 0, 0, 0, 0};
	l_N = l_r = l_x= l_k = NL;

	memcpy( N, Narr, NL * SIZEOF( limb_t ) );
	memcpy( x, xarr, NL * SIZEOF( limb_t ) );

	mrz_precomp( &ctx, N, l_N );

    mrz_mul( &ctx, r, x, ctx.rho_2 );    
	memcpy( k, k1arr, NL * SIZEOF( limb_t ) );

	LtRWinFrm(d, k, l_k);
	
    set_trigger();
    t_beg 	= rdcycle();
    exp_lr( &ctx, r, r, d, l_k );
    t_end 	= rdcycle();
    clear_trigger();

    mrz_mul( &ctx, r, r, ctx.rho_0 );

	test_dump( "N", N, l_N );  
    test_dump( "x", x, l_x );  
    test_dump( "k", k, l_k );  
    test_dump( "r", r, l_r ); 	
	putstr("(0x"); puthex8(1); putstr(", 0x"); puthex(t_end-t_beg); putstr(")  \n");

	mrz_mul( &ctx, r, x, ctx.rho_2 );
	memcpy( k, k2arr, NL * SIZEOF( limb_t ) );
	
	LtRWinFrm(d, k, l_k);

    set_trigger();
    t_beg 	= rdcycle();
    exp_lr( &ctx, r, r, d, l_k );
    t_end 	= rdcycle();
    clear_trigger();

    mrz_mul( &ctx, r, r, ctx.rho_0 );

	test_dump( "N", N, l_N );  
    test_dump( "x", x, l_x );  
    test_dump( "k", k, l_k );  
    test_dump( "r", r, l_r );
	putstr("(0x"); puthex8(2); putstr(", 0x"); puthex(t_end-t_beg); putstr(")  \n");

	return 0;
}

void riscv_irq_delegated_handler() {

}
