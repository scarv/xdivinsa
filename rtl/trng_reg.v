module trng_reg (
    clk,
    rst,
    gen,
    rdy,
    rdn );
parameter W  = 32; // random bit-length
parameter O =  3;  // post-processing filter order

localparam b = $clog2(W);

input           clk, rst;
input           gen;		
output          rdy;
output [W-1:0]  rdn;

reg [W-1:0] rdn;
reg    rdy;
reg    trn_gen;
reg    trn_gen_done;
(* dont_touch = "true" *) reg [W-1:0] trn_reg;     //shift random bit to a register of W bits before writing into ram

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
always@(posedge clk) begin
    if (rst) begin
		rdn		<= {W{1'b0}};
		rdy	    <= 1'b0;
		trn_gen <= 1'b0;	
		end
    else if (gen) begin
		if (~trn_gen) begin			 //new random was ready
			rdy 	<= 1'b1;
			rdn		<= trn_reg;		 //get generated number
			trn_gen <= 1'b1;  		 //gen new number
			end
		else begin // else is generating, it has to wait	
			rdy<=1'b0;
    		if (trn_gen_done)  trn_gen <=1'b0; 
			end		
		end
	else begin // check for next number after get a number
		if (trn_gen_done)  trn_gen <=1'b0;
		end
end

wire    trn_rnb, trn_val;

(* dont_touch = "true" *) es_trng trng_ins(
    .rst(rst),
    .clk(clk),
    .gen(trn_gen),
    .rnb(trn_rnb),
    .val(trn_val) 
);

reg trn_bit;
reg trn_bit_val;

// post-processing with a 3-order parity filter

wire fil_val, fil_out;
parity_filter #(.ORD(O)) filt_ins (.rst(rst), .clk(clk), .trn_val(trn_val), .trn_rnb(trn_rnb), .val(fil_val), .rnb(fil_out));

always @(posedge clk) begin
    if  (rst)     begin
		trn_bit 	<= 1'b0;
		trn_bit_val	<= 1'b0;
	end    
    else if (fil_val) begin
		trn_bit 	<= fil_out;
		trn_bit_val	<= 1'b1;
	end    
	else begin
		trn_bit 	<= 1'b0;
		trn_bit_val	<= 1'b0;
	end
end

reg [b-1:0]  bit_cnt;
always @(posedge clk) begin
    if  (rst)       trn_reg <= {W{1'b0}};    
    else if (trn_bit_val)   trn_reg <= {trn_reg[W-2:0], trn_bit};    
end
always @(posedge clk) begin
    if  (rst)       bit_cnt <= {b{1'b0}};    
    else if (trn_bit_val)   bit_cnt <= bit_cnt + 1'b1;    
end

wire new_value = trn_bit_val && (bit_cnt == W-1);

always @(posedge clk) begin
    if  (rst)        trn_gen_done <= 1'b0;
    else if (new_value)  trn_gen_done <=1'b1;
    else                 trn_gen_done <= 1'b0;    
end

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


