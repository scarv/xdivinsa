module trng_reg (
    clk,
    rst,
    gen,
    rdy,
    rdn );
parameter W  = 32; // random bit-length
parameter O  =  3;  // post-processing filter order
parameter RI = 32; // 32/16/8/4/2 instances generating 16 bits in parallel trng nubmers

localparam WI = W/RI; 
localparam b = $clog2(WI)+1;

input           clk, rst;
input           gen;		
output          rdy;
output [W-1:0]  rdn;

//reg [W-1:0] rdn;
wire   [W-1:0] rdn;
reg    rdy;
reg    trn_gen;
reg    trn_gen_done;


/*
{ "signal" : [
  { "name": "clk",         "wave": "P...|........" },
  { "name": "gen",         "wave": "01.0|.1....0." },
  { "name": "rdy",         "wave": "0.10|.....10." },
  { "name": "rdn",         "wave": "x.3x|.....3x.", "data": ["rdn0", "rdn1"] },
  { "name": "trn_gen",     "wave": "0.1.|....01.." },
  { "name": "trn_gen_done","wave": "0...|...10..." },
],
  "config" : { "hscale" : 1 }
}
*/

localparam IDLE    = 2'b00;
localparam RGEN    = 2'b01;
localparam PROC    = 2'b10;

reg [3:0] ctl_state;
always @(posedge clk)
	if (rst) begin
		ctl_state <= IDLE;
		end
	else
		case (ctl_state)
        IDLE : ctl_state  <= (gen)? RGEN : IDLE;
        RGEN : ctl_state  <= PROC;
		PROC : ctl_state  <= (trn_gen_done)? IDLE: PROC;
        default : begin  // Fault Recovery
            ctl_state <= IDLE;
        end
         endcase
         
always @(ctl_state)
     case (ctl_state)
     IDLE : begin          
         rdy        <= 1'b0;
         trn_gen    <= 1'b0;
         end
     RGEN : begin         
         rdy        <= 1'b1;
         trn_gen    <= 1'b1;
         end
     PROC: begin      
         rdy        <= 1'b0;
         trn_gen    <= 1'b1;
         end
     default : begin  // Fault Recovery
         rdy        <= 1'b0;
         trn_gen    <= 1'b0;
         end
      endcase

wire [RI-1: 0]   trn_rnb; 
wire [RI-1: 0]   trn_val;
wire [RI-1: 0]   fil_val;
wire [RI-1: 0]   fil_out;

genvar i;
generate
   for (i=0; i < RI; i=i+1)
   begin: filtered_es_trng
       (* dont_touch = "true" *) es_trng trng_ins(
       .rst(rst),
       .clk(clk),
       .gen(trn_gen),
       .rnb(trn_rnb[i]),
       .val(trn_val[i]) 
       );
       // post-processing with a 3-order parity filter
       parity_filter #(.ORD(O)) filt_ins (.rst(rst), .clk(clk), .trn_val(trn_val[i]), .trn_rnb(trn_rnb[i]), .val(fil_val[i]), .rnb(fil_out[i]));
   end
endgenerate

integer j;
reg [RI-1:0] trn_bit;
always @(posedge clk) begin
    if  (rst)               	trn_bit 	<= {RI{1'b0}};
    else begin
        for (j=0; j<RI; j=j+1)
            trn_bit[j]	<= (fil_val[j])?fil_out[j]:trn_bit[j];		
	end    
end

reg [RI-1:0] RI_bits_val;
wire all_val = &(RI_bits_val);
always @(posedge clk) begin
    if  (rst)          RI_bits_val	<= {RI{1'b0}};
    else if (all_val)  RI_bits_val	<= {RI{1'b0}};
    else		       RI_bits_val	<= RI_bits_val | fil_val;
end

wire new_value;
reg [W-1:0] trn_reg;     //shift random bit to a register of W bits 

generate
      if (W==RI) begin: condgen1
          always @(posedge clk) begin
              if      (rst)       trn_reg <= {W{1'b0}};    
              else if (all_val)   trn_reg <= trn_bit;    
          end
          assign new_value = all_val;
      end else begin:condgen2
          reg [b-1:0]  RIbits_cnt;
          always @(posedge clk) begin
              if      (rst)       trn_reg <= {W{1'b0}};    
              else if (all_val)   trn_reg <= {trn_reg[W-RI-1:0], trn_bit};    
          end
          
          always @(posedge clk) begin
              if      (rst)       RIbits_cnt <= {b{1'b0}};    
              else if (all_val)   RIbits_cnt <= RIbits_cnt + 1'b1;    
          end
      
          assign new_value = all_val && (RIbits_cnt == WI-1);
      end
   endgenerate

always @(posedge clk) begin
    if  (rst)        trn_gen_done <= 1'b0;
    else if (new_value)  trn_gen_done <=1'b1;
    else                 trn_gen_done <= 1'b0;    
end

assign rdn = trn_reg;

endmodule



module parity_filter #(parameter ORD=3)(input rst, input clk, input trn_val, input trn_rnb, output val, output rnb);
localparam CW = $clog2(ORD);

reg [ORD-1:0] trn_bit_reg;
always @(posedge clk) begin
    if		(rst)       trn_bit_reg <= {ORD{1'b0}};    
    else if (trn_val)   trn_bit_reg <= {trn_bit_reg[ORD-2:0], trn_rnb};    
end

reg [CW-1:0] trn_bit_cnt;
always @(posedge clk) begin
    if		(rst)       trn_bit_cnt <= {CW{1'b0}};    
    else if (trn_val)   trn_bit_cnt <= (trn_bit_cnt==(ORD-1))?{CW{1'b0}}:trn_bit_cnt+1'd1;    
end

reg  trn_bit_reg_full;
always @(posedge clk) begin
    if		(rst)	      							trn_bit_reg_full <= 1'b0;    
    else if ((trn_val & (trn_bit_cnt==(ORD-1))))	trn_bit_reg_full <= 1'b1;
	else     										trn_bit_reg_full <= 1'b0;
end

assign val = trn_bit_reg_full;
assign rnb = ^trn_bit_reg[ORD-1:0];

endmodule


