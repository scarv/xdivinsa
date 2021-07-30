//Diversifying instruction design 0 Implementing baseline of time varied instruction
module cop_ise #( 
parameter [6:0] CUSOPCODE = 7'b0001011,   // custom 0 is used by default
parameter 		TRNGNoise = 1,  		  // 1: Use TRNG as noise source while delaying
parameter 		TB =  0 				  // 1: Use for testbench to verify the control interface
) 
(
    cop_clk,
    cop_rst,
    cop_valid,
    cop_ready,
    cop_wait,
    cop_wr,
    cop_insn,   
    cop_rs1,   
    cop_rs2,           
    cop_rd    );
input           cop_clk, cop_rst;
input           cop_valid;
output          cop_ready;
output          cop_wait;
output          cop_wr;
input  [31:0]   cop_insn;
input  [31:0]   cop_rs1;
input  [31:0]   cop_rs2;
output [31:0]   cop_rd;

wire isopcode= (cop_insn[6:0] == CUSOPCODE); 
 
wire       op_valid = cop_valid && isopcode;
wire [6:0] funct    = cop_insn[31:25];

wire doAddi = (funct[2:0] == 3'b100);
wire doXori = (funct[2:0] == 3'b110);
wire rdrand = (funct[2:0] == 3'b010);

// TRNG for random delay ==========================================
wire        trn_gen;
wire        trn_rdy;
wire [7:0]  randnum;
generate
    if (TB==0) begin: es_trng_gen
		trng_reg #(.W(8), .RI(4), .O(3)) trng_ins (
    		.clk(cop_clk),
    		.rst(cop_rst),
		    .gen(trn_gen),
		    .rdy(trn_rdy),
		    .rdn(randnum) );
		    
	end else begin: es_trng_tb
		reg [7:0] randnum_sim;
		reg       trn_rdy_sim;
		reg       trn_gen_sim;
        reg [3:0] dl;
		
		always @(posedge cop_clk) 
            if  (cop_rst)       trn_gen_sim <= 1'b0;
            else if (trn_gen)  begin
                trn_gen_sim <= 1'b1;
                @(posedge cop_clk)
                @(posedge cop_clk)
                @(posedge cop_clk)
                @(posedge cop_clk)
                @(posedge cop_clk)                
                trn_gen_sim <= 1'b0;
                end
             
        always @(posedge cop_clk)
             if  (cop_rst)  begin
                     trn_rdy_sim    <= 1'b0;
                     randnum_sim    <= 8'd0;
                     end
                 else if ((trn_gen==1'b1)&&(trn_gen_sim==1'b0)) begin
                     trn_rdy_sim    <= 1'b1;
                     randnum_sim    <= $random;
                     end
                 else begin
                     trn_rdy_sim    <= 1'b0;
                     end
                                    
		assign trn_rdy = trn_rdy_sim;
 		assign randnum = randnum_sim;
    end  
endgenerate
//=====================================================================

// PRNG for dummy operations ==========================================
wire           nondeter_bit = ^randnum;
wire  [31:0]   prng;

lfsr32  prng_ins(
    .clk(       cop_clk     ),      
    .rst(       cop_rst     ),   
    .update(    TRNGNoise   ), 
    .extra_tap( nondeter_bit), // Additional seed bit, possibly from TRNG.
    .prng(      prng        )  // PRNG value.
    );
//=====================================================================

//data-path ===========================================================    
reg  [31:0]  opr_ran;
wire [31:0]  opr_lhs, opr_rhs;

wire [31:0] xori;
wire [31:0] addi;
wire [31:0] arthres;

assign opr_lhs = cop_rs1;
assign opr_rhs = opr_ran;
assign xori    = opr_lhs ^ opr_rhs;
assign addi    = opr_lhs + opr_rhs;
assign arthres = (doXori)? xori : (doAddi)? addi:32'd0;
//=====================================================================

//control-unit ========================================================
wire       ranexp;  //random timing 
reg [31:0] resreg;

localparam IDLE    = 4'b0000;
localparam RDNGEN  = 4'b0001;
localparam ARTCAL1 = 4'b0010;
localparam ARTCAL2 = 4'b0011;
localparam ARTCAL3 = 4'b0100;
localparam SDONE   = 4'b0101;

reg [3:0] ctl_state;
always @(posedge cop_clk)
	if (cop_rst) 	ctl_state <= IDLE;
	else
		case (ctl_state)
        IDLE :      ctl_state <= (op_valid & rdrand) ?          RDNGEN:
								 (op_valid & (doXori|doAddi)) ? ARTCAL1: 
																IDLE;                    
		RDNGEN : 	ctl_state <= SDONE;
        ARTCAL1:    ctl_state <= ARTCAL2;

        ARTCAL2:    ctl_state <= (ranexp)? ARTCAL3 : ARTCAL2;          
        ARTCAL3:    ctl_state <= SDONE;		
        
        SDONE :     ctl_state <= IDLE; 
                
        default :   ctl_state <= IDLE;
        endcase

always @(posedge cop_clk)
	if (cop_rst)                   opr_ran <= 32'd0;     
    else if (ctl_state == ARTCAL2) opr_ran <= (ranexp==1'b1) ? cop_rs2: // waiting a random delay then caculate the result
	                                          /*dummy values*/ prng;                       

generate 
  if (TRNGNoise == 1'b1) begin : TRNGNoise_imp         
    assign 	trn_gen   = 1'b1;
  end else begin                    : No_TRNGNoise
    assign 	trn_gen   = (ctl_state == RDNGEN)||(ctl_state == ARTCAL3)||(ctl_state == SDONE);
  end
endgenerate

// time execution randomising =======================================
wire [3:0] timopt = funct[6:3];
wire       timena = timopt[3]; // time execution randomising is enable when timopt[3] (funct[6]) is set.
         
reg [7:0]  timmsk;
always @ (timopt) begin
    case(timopt[2:0])
        3'b000:   timmsk = 8'b0000_0001;
        3'b001:   timmsk = 8'b0000_0011;
        3'b010:   timmsk = 8'b0000_0111;
        3'b011:   timmsk = 8'b0000_1111;
        3'b100:   timmsk = 8'b0001_1111;
        3'b101:   timmsk = 8'b0011_1111;
        3'b110:   timmsk = 8'b0111_1111;
        3'b111:   timmsk = 8'b1111_1111;
    endcase        
end
reg [7:0]  timcnt;  
always @(posedge cop_clk) 
    if (cop_rst)                 timcnt <= 8'h00;
    else if (~timena)            timcnt <= 8'h00;
    else if (timcnt != 8'd0)     timcnt <= timcnt -1'b1;
    else if (ctl_state==ARTCAL1) timcnt <= (timmsk & randnum[ 7: 0]);       

assign ranexp = (timcnt == 8'h00);         
//====================================================

reg cop_wait;
always@(posedge cop_clk) begin
    if (cop_rst)                     cop_wait <= 1'b0;
    else if (ctl_state == ARTCAL2) 	 cop_wait <= 1'b1;
    else                             cop_wait <= 1'b0;
end

always@(posedge cop_clk) begin
    if (cop_rst)                        resreg <= 32'd0;
    else if (ctl_state == RDNGEN) 		resreg <= {24'd0,randnum};
    else if (ctl_state == ARTCAL3)      resreg <= arthres;
end

assign cop_ready = (ctl_state == SDONE);

assign cop_rd = resreg[31:0];
assign cop_wr = cop_ready;

endmodule

module lfsr32 #(
parameter RESET_VALUE = 32'h6789ABCD
)(
input  wire        clk        , // Clock to update PRNG
input  wire        rst        , // Syncrhonous active high reset.
input  wire        update     , // Update PRNG with new value.
input  wire        extra_tap  , // Additional seed bit, possibly from TRNG.
output reg  [31:0] prng         // Current PRNG value.
);

wire        n_prng_lsb =  prng[31] ~^ prng[21] ~^ prng[ 1] ~^ prng[ 0] ^
                          extra_tap ;

wire [31:0] n_prng     = {prng[31-1:0], n_prng_lsb};

always @(posedge clk) begin
    if     (rst)       prng <= RESET_VALUE;
    else if( update  ) prng <= n_prng;
end

endmodule
