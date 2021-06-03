//https://vlsicoding.blogspot.com/2016/04/verilog-code-for-bcd-to-7-segment-using-verilog-coding.html
module SevSeg(clk,bcd,seven_seg);

input [3:0] bcd;
input clk;
output reg [6:0] seven_seg;

always @(posedge clk)
 begin
  case (bcd)
   4'b0000 : begin seven_seg = 7'b1111110; end
   4'b0001 : begin seven_seg = 7'b0110000; end
   4'b0010 : begin seven_seg = 7'b1101101; end
   4'b0011 : begin seven_seg = 7'b1111001; end
   4'b0100 : begin seven_seg = 7'b0110011; end
   4'b0101 : begin seven_seg = 7'b1011011; end
   4'b0110 : begin seven_seg = 7'b1011111; end
   4'b0111 : begin seven_seg = 7'b1110000; end
   4'b1000 : begin seven_seg = 7'b1111111; end
   4'b1001 : begin seven_seg = 7'b1110011; end
   default : begin seven_seg = 7'b0000000; end
  endcase
 end
endmodule

// parameter [10*8-1:0] digitCodeMap = {
//   // GFEDCBA  Segments      7-segment map:
//   8'b00111111, // 0   "0"          AAA
//   8'b00000110, // 1   "1"         F   B
//   8'b01011011, // 2   "2"         F   B
//   8'b01001111, // 3   "3"          GGG
//   8'b01100110, // 4   "4"         E   C
//   8'b01101101, // 5   "5"         E   C
//   8'b01111101, // 6   "6"          DDD
//   8'b00000111, // 7   "7"
//   8'b01111111, // 8   "8"
//   8'b01101111 // 9   "9"
// };
