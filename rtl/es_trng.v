module es_trng(
    rst,
    clk,
    gen,
    rnb,
    val 
);

input rst, clk;
input gen;
output rnb;
output val;

wire fro1_ena;
wire fro2_ena;
wire clr;

wire valid;
wire ran_bit; 

es_trng_ctrl ctrl_ins(  .rst(rst), .clk(clk), .gen(gen), .rnb(rnb), .val(val),
                        .fro1_ena(fro1_ena), 
                        .fro2_ena(fro2_ena), 
                        .clr(clr), 
                        .valid(valid), 
                        .ran_bit(ran_bit));

(* dont_touch = "true", KEEP = "true" *) wire [5:0] t_rand;
(* dont_touch = "true", KEEP = "true" *)  wire [5:0] t_val;
 
generate genvar i;
for (i=0;i<6;i=i+1) begin : gen_trng
     es es_inst (.fro1_ena(fro1_ena), .fro2_ena(fro2_ena), .clr(clr), .valid(t_val[i]), .ran_bit(t_rand[i]));
end
endgenerate

assign valid   = &t_val[5:0];
assign ran_bit = ^t_rand[5:0];

endmodule


module es(fro1_ena, fro2_ena, clr, valid, ran_bit);
input fro1_ena, fro2_ena, clr;
output valid, ran_bit;

//free-running ring oscillator 1
wire fro1_lut_nand_o;
(* dont_touch = "true", ALLOW_COMBINATORIAL_LOOPS = "true", KEEP = "true" *) wire fro1_fb;
LUT2 #( .INIT(4'b1000)) LUT2_fro1_inst ( .O(fro1_lut_nand_o), .I0(fro1_fb), .I1(fro1_ena));
LUT1 #( .INIT(2'b01))   LUT1_fro1_buf1_inst ( .O(fro1_fb), .I0(fro1_lut_nand_o));
wire fro1_out = fro1_fb;

//free-running ring oscillator 2
wire fro2_lut_nand_o;
(* dont_touch = "true", ALLOW_COMBINATORIAL_LOOPS = "true", KEEP = "true" *) wire fro2_fb;
LUT2 #( .INIT(4'b1000)) LUT2_fro2_inst ( .O(fro2_lut_nand_o), .I0(fro2_fb), .I1(fro2_ena));
(* dont_touch = "true" *) wire fro2_buf1_o, fro2_buf2_o;
LUT1 #( .INIT(2'b01))   LUT1_fro2_buf1_inst ( .O(fro2_buf1_o), .I0(fro2_lut_nand_o));
LUT1 #( .INIT(2'b01))   LUT1_fro2_buf2_inst ( .O(fro2_buf2_o), .I0(fro2_buf1_o));
LUT1 #( .INIT(2'b01))   LUT1_fro2_buf3_inst ( .O(fro2_fb),     .I0(fro2_buf2_o));

wire fro2_out = fro2_lut_nand_o; 

//tapped delay chain
(* dont_touch = "true" *) wire [3:0] tapped_delay;
wire [2:0] tapped_delay_n;
CARRY4 CARRY4_inst (
    .CO(),         // 4-bit carry out
    .O(tapped_delay),           // 4-bit carry chain XOR data out
    .CI(1'b0),         // 1-bit carry cascade input
    .CYINIT(fro1_fb), // 1-bit carry initialization
    .DI(4'b0000),         // 4-bit carry-MUX data in
    .S(4'b1111)            // 4-bit carry-MUX select input
);

wire [2:0] tapped_delay_out;
FDCE   #(.INIT(1'b0)) FDCE_tapped_delay_inst0 
        (.C(fro2_out), .CLR(1'b0), .CE(1'b1),   .D(tapped_delay[0]),    .Q(tapped_delay_out[0]));
FDCE   #(.INIT(1'b0)) FDCE_tapped_delay_inst1 
        (.C(fro2_out), .CLR(1'b0), .CE(1'b1),   .D(tapped_delay[1]),    .Q(tapped_delay_out[1]));
FDCE   #(.INIT(1'b0)) FDCE_tapped_delay_inst2 
        (.C(fro2_out), .CLR(1'b0), .CE(1'b1),   .D(tapped_delay[2]),    .Q(tapped_delay_out[2]));

//bit extractor
wire extractor_valid_out;
wire extractor_ranbit_out;
wire extractor_ranbit;
wire ranbit_encode;
wire edge_dectect;
LUT3   #(.INIT(8'b0100_0010)) LUT3_ranbit_encode_inst 
        (.O(ranbit_encode), .I0(tapped_delay_out[0]),.I1(tapped_delay_out[1]),.I2(tapped_delay_out[2]));
      
assign extractor_ranbit= (extractor_valid_out)?extractor_ranbit_out:ranbit_encode;
LUT3   #(.INIT(8'b1111_0110)) LUT3_edge_dectect_inst 
        (.O(edge_dectect),  .I0(tapped_delay_out[0]),.I1(tapped_delay_out[2]),.I2(extractor_valid_out));
   
FDCE   #(.INIT(1'b0)) FDCE_extractor_valid_inst 
        (.C(fro2_out), .CLR(clr), .CE(1'b1),    .D(edge_dectect),   .Q(extractor_valid_out));
FDCE   #(.INIT(1'b0)) FDCE_extractor_ranbit_inst 
        (.C(fro2_out), .CLR(clr), .CE(1'b1),    .D(extractor_ranbit),.Q(extractor_ranbit_out));

assign valid = extractor_valid_out;
assign ran_bit = extractor_ranbit_out;

endmodule


module es_trng_ctrl(
    rst,
    clk,
    gen,
    rnb,
    val,
    
    fro1_ena,
    fro2_ena,
    clr,  
    valid,
    ran_bit  
);

input rst, clk;
input gen;
output reg rnb;
output reg val;

output reg fro1_ena;
output reg fro2_ena;
output reg clr;
input valid;
input ran_bit; 

//FSM controller
localparam IDLE = 2'b00;
localparam ENA_FRO1 = 2'b01;
localparam ENA_FRO2 = 2'b10;
localparam DONE = 2'b11;

reg [1:0] state;

wire acc_done;
reg [2:0] acc_cnt;
always @(posedge clk) begin
    if (rst)                    acc_cnt <= 3'b000;
    else if (state == ENA_FRO1) acc_cnt <= acc_cnt + 1'b1;
    else                        acc_cnt <= 3'b000;
end

assign acc_done = (acc_cnt == 3'b101);

always @(posedge clk)
   if (rst) begin
      state <= IDLE;
      
      fro1_ena <= 1'b0;
      fro2_ena <= 1'b0;
      clr      <= 1'b1;
      
      rnb      <= 1'b0;
      val      <= 1'b0;
   end
   else
      case (state)
         IDLE : begin
            if (gen)            state <= ENA_FRO1;
            else                state <= IDLE;
            fro1_ena <= 1'b0;
            fro2_ena <= 1'b0;
            clr      <= 1'b1;               
            rnb      <= 1'b0;
            val      <= 1'b0;
         end
         ENA_FRO1 : begin
            if (acc_done)       state <= ENA_FRO2;
            else                state <= ENA_FRO1;        
            fro1_ena <= 1'b1;
            clr      <= 1'b0;
         end
         ENA_FRO2 : begin
            if (valid)          state <= DONE;
            else                state <= ENA_FRO2;
            fro2_ena <= 1'b1;
         end
         DONE : begin
            state <= IDLE;                                            
            rnb      <= ran_bit;
            val      <= 1'b1;
         end
         default : begin  // Fault Recovery
            state <= IDLE;            
         end
      endcase

endmodule


