#include "common.h"
#include "sasstarget.h"
#include <scarv/mp/mpn.h>
#include <scarv/mp/mpz.h>
#include <scarv/mp/mrz.h>

#define NL 4   // size length in limbs (a limb is 32 bits)
#define  W 3   // window size for LtR algorithm 
#define PW (1<<(W-1))   // (2^(W-1))
// For evaluating Ttest on modulo exponentiation   ===========


mrz_ctx_t ctx;

mrz_t N; int l_N;
mrz_t k; int l_k;
mrz_t c; int l_c;

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
//Precomputation for LtR slided window algorithm
void LtRPreCom(const mrz_ctx_t* ctx, mrz_t * b, const mrz_t x) {
    mrz_t x2;
    
    memcpy( b[0], ctx->rho_1, SIZEOF( mrz_t ) );
    //Precomputation
	mrz_mul( ctx, b[0], b[0], x );	// b[0]=x
	mrz_mul( ctx, x2, b[0], x );	// x2 = x*x 

	for (int i = 1; i < PW; i++) {
		mrz_mul( ctx, b[i], b[i-1], x2 );   //b[i]=b[i-1]*x^2		
	}
}

//straightforward algorithm
void exp_sf( const mrz_ctx_t* ctx, mrz_t r, const mrz_t x, const limb_t* y, int l_y ) {

  mrz_t t;
  memcpy( t, ctx->rho_1, SIZEOF( mrz_t ) ); 

  set_trigger();     //SET_TRIG
  for( int i = l_y - 1; i >= 0; i-- ) {
    for( int j = ( BITSOF( limb_t ) - 1 ); j >= 0; j-- ) { 

      mrz_mul( ctx, t, t, t );

      if( ( y[ i ] >> j ) & 1 ) {
        mrz_mul( ctx, t, t, x );
      }
      
    }
  } 
  clear_trigger();    //CLR_TRIG

  memcpy( r, t, SIZEOF( mrz_t ) );
}

//Montgomery ladder algorithm
void exp_Ml( const mrz_ctx_t* ctx, mrz_t r, const mrz_t x, const limb_t* y, int l_y ) {
	mrz_t R[2];
	bool b, di;
	memcpy( R[0], ctx->rho_1, SIZEOF( mrz_t ) );
	memcpy( R[1], x, NL*SIZEOF( limb_t ) );	
   
	set_trigger();    //SET_TRIG
	for( int i = l_y - 1; i >= 0; i-- ) {
		for( int j = ( BITSOF( limb_t ) - 1 ); j >= 0; j-- ) {
			di = (( y[ i ] >> j ) & 1); 
			b  = ! di;
			mrz_mul( ctx, R[b],  R[0],  R[1] );
			mrz_mul( ctx, R[di], R[di], R[di]);
  		}
	}
	clear_trigger();  //CLR_TRIG  
    
    memcpy( r,          R[0], SIZEOF( mrz_t ) );
}

//always square-multiply algorithm
void exp_sma( const mrz_ctx_t* ctx, mrz_t r, const mrz_t x, const limb_t* y, int l_y ) {
	mrz_t R[2];
	bool b;
	memcpy( R[0], ctx->rho_1, SIZEOF( mrz_t ) );
    
	set_trigger();   //SET_TRIG
	for( int i = l_y - 1; i >= 0; i-- ) {
		for( int j = ( BITSOF( limb_t ) - 1 ); j >= 0; j-- ) {			
			b = !(( y[ i ] >> j ) & 1); 
			mrz_mul( ctx, R[0], R[0], R[0] );
			mrz_mul( ctx, R[b], R[0], x);            
  		}
	}
	clear_trigger();  //CLR_TRIG    

    memcpy( r,          R[0], SIZEOF( mrz_t ) );
}

// left to right sliding window algorithm
void exp_sw( const mrz_ctx_t* ctx, mrz_t r, const mrz_t x, const mrz_t* b, const uint8_t* d, int l_k ) {
	mrz_t t;
	int i;

	memcpy( t, ctx->rho_1, SIZEOF( mrz_t ) );

    //calculation loop    
	set_trigger();   //SET_TRIG
	i = l_k*BITSOF( limb_t ) - 1;
	while (i>=0){
		mrz_mul( ctx, t, t, t );
		if (d[i] != 0){
			mrz_mul( ctx, t, t, b[(d[i]>>1)]);			
		}	
		i --;	
	}
	clear_trigger();  //CLR_TRIG    

	memcpy( r,          t, SIZEOF( mrz_t ) );
}
//================================================================
 
//! Context object for host/target communication
sass_ctx sass;

 
//! Function used to send bytes to the host.
void sass_send_byte_to_host(unsigned char to_send) { 
    // Wait until the TX FIFO is not full
    int unsigned  full = *uart_stat & (0x1<<3);
    while(full) { full = *uart_stat & (0x1<<3);   }
    // Write the byte to the TX FIFO of the UART.
    int unsigned to_write = 0 | to_send;
    *uart_tx = to_write;
}

//! Function used to recieve bytes from the host.
unsigned char sass_recv_byte_from_host() {
    // Wait until the RX FIFO has something in it.
    int unsigned        valid = *uart_stat & (0x1);
    while(valid == 0) { valid = *uart_stat & (0x1); }

    unsigned char tr = *uart_rx & 0xFF;
    return tr;
}

unsigned char sass_custom(){

	set_trigger();     clear_trigger();
    return SASS_STATUS_OK;
}

//! Pointer to function used for encryption
void sass_encrypt(char * message, char * key, char * cipher, int unsigned keylen, int unsigned mlen)
{
    set_trigger();	clear_trigger();
}

//! Pointer to function used for decryption
void sass_decrypt(char * message, char * key, char * cipher, int unsigned keylen, int unsigned mlen) 
{
    set_trigger();  clear_trigger();
}

unsigned char sass_t_func(char * datout, char * datin ){

    mrz_t r; 
    
    #ifdef TTEST_T1
    memcpy( c, datin, NL * SIZEOF( limb_t ) );
    #elif  TTEST_T2   
    memcpy( k, datin, NL * SIZEOF( limb_t ) );
    #endif
 
    mrz_mul( &ctx, r, c, ctx.rho_2 );

	#ifdef ALG_SMA
    exp_sma ( &ctx, r, r, k, l_k );
    #elif  ALG_SW
    uint8_t d[NL * BITSOF( limb_t )]; mrz_t b[PW];
    LtRWinFrm(d, k, l_k);	LtRPreCom(&ctx, b, r);
    exp_sw( &ctx, r, r, b, d, l_k );
    #else
    exp_sf ( &ctx, r, r, k, l_k );
	#endif
    //exp_Ml ( &ctx, r, r, k, l_k );
    mrz_mul( &ctx, r, r, ctx.rho_0 ); 

    memcpy(datout, (uint8_t*) r, NL * SIZEOF( limb_t ) );

    return SASS_STATUS_OK;
}

void riscv_main()
{
    gpio_init();
    uart_init();
    int r;
    CiRand(r);
    
    /* python code to generate 1024 bit key of RSA
    from Crypto.PublicKey import RSA 
    new_key = RSA.generate(1024, e=65537)
    bn= new_key.n.to_bytes(128,'little')
    Narr=",".join("0x{:02x}".format(b) for b in bn)
    bd= new_key.d.to_bytes(128,'little')
    karr=",".join("0x{:02x}".format(b) for b in bd)
	*/
    //N=0xb9-b4-8b-3e-f6-10-fd-30-db-40-4e-d7-59-17-8d-03-ae-fa-8b-5d-9f-4a-ec-b0-7f-92-bf-07-39-f8-c2-5b
    //    b5-62-ef-49-de-2d-21-4d-37-95-9a-1e-d4-e4-88-46-39-a2-b0-c4-c6-98-69-de-dd-63-be-74-71-a5-28-72
    //    02-ec-76-52-fb-7f-30-f3-ed-66-be-88-41-da-6d-87-55-a8-24-91-26-78-e5-c7-ab-c3-f5-48-d1-92-6b-b8
    //    7c-05-3e-b5-e1-05-fc-a3-53-63-a1-02-be-ce-fb-d6-2b-b2-22-ac-f0-94-84-72-8e-08-2f-df-df-d6-e3-c5
    //d=0xb1-f5-4c-d7-1a-10-ef-c0-e8-ea-b8-62-ef-b2-7c-e4-c1-c2-c0-d9-07-f4-51-db-55-75-77-f0-61-e2-7b-c6
    //    4c-8d-31-88-15-0c-24-74-8d-c6-a4-39-d5-83-e6-ff-7c-ed-e5-fd-bb-de-84-de-18-44-24-58-0c-2b-dc-13
    //    bb-ca-e4-99-33-f2-53-a8-9e-d5-61-ee-9d-f2-51-61-65-86-08-df-85-d4-af-96-bf-7d-aa-ba-dc-85-c9-7c
    //    1c-3e-b2-ce-63-e3-d6-1f-62-f9-92-8b-f8-d2-d6-86-9f-3e-89-34-a5-68-88-fe-80-b4-15-26-78-4e-1f-e1 

    //                LSB                                                                         MSB  
    uint8_t  Narr[16]={0x03,0x8d,0x17,0x59,0xd7,0x4e,0x40,0xdb,0x30,0xfd,0x10,0xf6,0x3e,0x8b,0xb4,0xf9};
    uint8_t  karr[16]={0xbf,0x7c,0xb2,0xef,0x62,0xb8,0xea,0xe8,0xc0,0xef,0x10,0x1a,0xd7,0x4c,0xf5,0xef};
    uint8_t  carr[16]={0xc8,0x87,0x38,0x67,0x85,0x9a,0x11,0x4e,0x0b,0x02,0x53,0x5c,0x93,0xeb,0x2d,0x43};

    l_N = l_c= l_k = NL;

	memcpy( N, Narr, NL * SIZEOF( limb_t ) );
	memcpy( k, karr, NL * SIZEOF( limb_t ) );
    memcpy( c, carr, NL * SIZEOF( limb_t ) );

    mrz_precomp( &ctx, N, l_N );

    // Setup the SASS context
 	sass_ctx_init(&sass);
    sass.send_byte_to_host   = sass_send_byte_to_host;
    sass.recv_byte_from_host = sass_recv_byte_from_host;
    sass.encrypt             = sass_encrypt;
    sass.decrypt             = sass_decrypt;
    sass.custom              = sass_custom;
	sass.t_func              = sass_t_func;

    sass_target_run(&sass);

}

void riscv_irq_delegated_handler() {
}

