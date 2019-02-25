`timescale 1ns / 1ps
module cop_ise #( parameter [6:0] CUSOPCODE = 7'b0001011) // custom 0 is used by default
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

assign cop_wait = 1'b0;
wire isopcode= (cop_insn[6:0] == CUSOPCODE); 
 
reg [31:0] op1;
reg [31:0] op2;
reg [6:0]  funct;
reg        op_valid;
always@(posedge cop_clk) begin
    if (cop_rst) begin
        op1      <= 32'd0;
        op2      <= 32'd0;
        funct    <= 7'd0;
        op_valid <= 1'b0;
    end    
    else if (cop_valid && isopcode) begin
        op1      <= cop_rs1;
        op2      <= cop_rs2;
        funct    <= cop_insn[31:25];   
        op_valid <= 1'b1;
    end
    else op_valid <= 1'b0;
end

wire doAddi = (funct[2:0] == 3'b000);
wire doMult = (funct[2:0] == 3'b001);
wire rdrand = (funct[2:0] == 3'b010);

wire [31:0] addi;
wire [31:0] mult;
wire [31:0] arthres;
wire [31:0] randnum; 

//data-path ==========================================
reg trn_gen;
wire        trn_rdy;
trng_reg #(.W(32), .O(3)) trng_ins (
    .clk(cop_clk),
    .rst(cop_rst),
    .gen(trn_gen),
    .rdy(trn_rdy),
    .rdn(randnum) );

reg [31:0] 	artran; //random number for arith. calculation masking
reg [1:0]   msk_ctl;
wire [31:0] op2mask;
assign op2mask = (msk_ctl==2'b00)? (op2 ^ artran) : (msk_ctl==2'b01)?(op2 & artran) : (msk_ctl==2'b10)?(~op2 & artran) : 32'd0;
//assign op1mask = (msk_ctl)? (op1 & artran) : (op1 & ~artran);
assign addi = op1 + op2mask;
assign mult = op1 * op2mask;
assign arthres=(doAddi)?addi:(doMult)?mult:32'd0;
//====================================================

//control-unit =======================================
/*
digraph G {
    IDLE      [label="IDLE"];
    RDNGEN    [label="RDNGEN/trn_gen"];
    RDNFIN    [label="RDNFIN/~trn_gen; resreg=randnum; resval"];
    ARTCAL1   [label="ARTCAL1/trn_gen"];
    ARTCAL2   [label="ARTCAL2/~trn_gen; artran=randnum; msk_ctl=00"];
    ARTCAL2T  [label="ARTCAL2T"];
    ARTCAL3   [label="ARTCAL3/resreg=arthres; msk_ctl=01"];
    ARTCAL3T  [label="ARTCAL3T"];
    ARTCAL4   [label="ARTCAL4/resreg+=arthres; msk_ctl=10"];
    ARTCAL4T  [label="ARTCAL4T"];
    ARTCAL5   [label="ARTCAL5/resreg-=arthres;"];
    ARTCAL5T  [label="ARTCAL5T"];
    ARTFIN    [label="ARTFIN/resval"];

    IDLE -> RDNGEN [label="op_valid & rdrand"];
    RDNGEN -> RDNFIN[label="trn_rdy"];
    RDNGEN -> RDNGEN;
    RDNFIN -> IDLE;

    IDLE -> ARTCAL1 [label="op_valid & (doAddi | doMult)"];
    ARTCAL1 -> ARTCAL2[label="trn_rdy"];    ARTCAL1 -> ARTCAL1;

    ARTCAL2 -> ARTCAL2T;
    ARTCAL2T-> ARTCAL3[label="ranexp"]; ARTCAL2T ->ARTCAL2T;
    ARTCAL3 -> ARTCAL3T;
    ARTCAL3T-> ARTCAL4[label="ranexp"]; ARTCAL3T ->ARTCAL3T;
    ARTCAL4 -> ARTCAL4T;
    ARTCAL4T-> ARTCAL5[label="ranexp"]; ARTCAL4T ->ARTCAL4T;
    ARTCAL5 -> ARTCAL5T;
    ARTCAL5T-> ARTFIN[label="ranexp"]; ARTCAL5T ->ARTCAL5T;
    ARTFIN -> IDLE[];
}
}
*/
wire       ranexp;  //random timing 

reg [31:0] resreg;
reg        resval;

localparam IDLE    = 4'b0000;
localparam RDNGEN  = 4'b0001;
localparam RDNFIN  = 4'b0010;
localparam ARTCAL1 = 4'b0011;
localparam ARTCAL2 = 4'b0100;
localparam ARTCAL2T= 4'b0101;
localparam ARTCAL3 = 4'b0110;
localparam ARTCAL3T= 4'b0111;
localparam ARTCAL4 = 4'b1000;
localparam ARTCAL4T= 4'b1001;
localparam ARTCAL5 = 4'b1010;
localparam ARTCAL5T= 4'b1011;
localparam ARTFIN  = 4'b1100;

reg [3:0] ctl_state;
always @(posedge cop_clk)
	if (cop_rst) begin
		ctl_state <= IDLE;
        artran  <= 32'd0;
		resreg  <= 32'd0;
		resval  <= 1'b0;
		trn_gen <= 1'b0;
        msk_ctl <= 2'b00;
		end
	else
		case (ctl_state)
        IDLE : begin
        	if (op_valid & rdrand)
            	ctl_state <= RDNGEN;
            else if (op_valid & (doAddi|doMult))
                ctl_state <= ARTCAL1;
            else
                ctl_state <= IDLE;
            resreg  <= 32'd0;
			resval  <= 1'b0;
			trn_gen <= 1'b0;
			msk_ctl <= 2'b00;
            end
		RDNGEN : begin
            ctl_state <= (trn_rdy)? RDNFIN : RDNGEN;
			trn_gen <= 1'b1;
            end
        RDNFIN : begin
            ctl_state <= IDLE;
            resreg  <= randnum;
			resval  <= 1'b1;
			trn_gen <= 1'b0;
            end
        ARTCAL1: begin
            ctl_state <= (trn_rdy)? ARTCAL2 : ARTCAL1;
            trn_gen <= 1'b1;
            end
        ARTCAL2: begin
            ctl_state <= ARTCAL2T;
            trn_gen <= 1'b0;
            artran  <= randnum;
            msk_ctl <= 2'b00;            
            end
        ARTCAL2T: begin
            ctl_state <= (ranexp)? ARTCAL3 : ARTCAL2T;
            end            
        ARTCAL3: begin
            ctl_state <= ARTCAL3T;
			resreg  <= arthres;
			msk_ctl <= 2'b01;
			end
        ARTCAL3T: begin
            ctl_state <= (ranexp)? ARTCAL4 : ARTCAL3T;
            end   			
        ARTCAL4: begin
            ctl_state <= ARTCAL4T;
            resreg  <= resreg + arthres;
            msk_ctl <= 2'b10;
            end		
        ARTCAL4T: begin
            ctl_state <= (ranexp)? ARTCAL5 : ARTCAL4T;
            end   	
        ARTCAL5 : begin
            ctl_state <= ARTCAL5T;
            resreg  <= resreg - arthres;
            end
        ARTCAL5T: begin
            ctl_state <= (ranexp)? ARTFIN : ARTCAL5T;
            end   
        ARTFIN : begin
            ctl_state <= IDLE; 
            resval  <= 1'b1;
            end
        default : begin  // Fault Recovery
            ctl_state <= IDLE;
            artran  <= 32'd0;
	   	    resreg  <= 32'd0;
		    resval  <= 1'b0;
            trn_gen <= 1'b0;
            msk_ctl <= 2'b00;
            end
         endcase
         
// time execution randomising ========================
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
    else if (ctl_state==ARTCAL2) timcnt <= timmsk & randnum[ 7: 0];
    else if (ctl_state==ARTCAL3) timcnt <= timmsk & randnum[15: 8];
    else if (ctl_state==ARTCAL4) timcnt <= timmsk & randnum[23:16];
    else if (ctl_state==ARTCAL5) timcnt <= timmsk & randnum[31:24];         

assign ranexp = (timcnt == 8'h00);         
//====================================================

assign cop_ready = resval;

assign cop_rd = resreg;
assign cop_wr = resval;

endmodule


