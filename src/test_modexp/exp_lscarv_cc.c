#include "soc.h"
#include "common.h"

int NL= 2;

mrz_ctx_t ctx;

void exp_cc( const mrz_ctx_t* ctx, mrz_t r, const mrz_t x, const limb_t* y, int l_y ) {
  mrz_t t;
  mrz_t tt;  

  memcpy( t, ctx->rho_1, SIZEOF( mrz_t ) );

  for( int i = l_y - 1; i >= 0; i-- ) {
    for( int j = ( BITSOF( limb_t ) - 1 ); j >= 0; j-- ) {
      mrz_mul( ctx, t, t, t );

      if( ( y[ i ] >> j ) & 1 ) {
        mrz_mul( ctx, t, t, x );
      } else {
        mrz_mul( ctx, tt, t, x );
	  }
    }
  }

  memcpy( r,          t, SIZEOF( mrz_t ) );
}

int riscv_main() {

	gpio_init();
    uart_init();
	uint32_t t_beg,t_end;

    putstr("\nExp cross-copying \n");	

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

    set_trigger();
    t_beg 	= rdcycle();
    exp_cc( &ctx, r, r, k, l_k );
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

    set_trigger();
    t_beg 	= rdcycle();
    exp_cc( &ctx, r, r, k, l_k );
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
