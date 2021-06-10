//https://vlsicoding.blogspot.com/2016/04/verilog-code-for-bcd-to-7-segment-using-verilog-coding.html
module SevSeg(clk,bcd,seven_seg);

input [3:0] bcd;
input clk;
output reg [6:0] seven_seg;

always @(posedge clk)
 begin
  case (bcd)
   4'b0000 : begin seven_seg = 7'b1111110; end //0
   4'b0001 : begin seven_seg = 7'b0110000; end //1
   4'b0010 : begin seven_seg = 7'b1101101; end //2
   4'b0011 : begin seven_seg = 7'b1111001; end //3
   4'b0100 : begin seven_seg = 7'b0110011; end //4
   4'b0101 : begin seven_seg = 7'b1011011; end //5
   4'b0110 : begin seven_seg = 7'b1011111; end //6
   4'b0111 : begin seven_seg = 7'b1110000; end //7
   4'b1000 : begin seven_seg = 7'b1111111; end //8
   4'b1001 : begin seven_seg = 7'b1110011; end //9
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
// 0b01110111, // 65  'A'
//   0b01111100, // 66  'b'
//   0b00111001, // 67  'C'
//   0b01011110, // 68  'd'
//   0b01111001, // 69  'E'
//   0b01110001, // 70  'F'
//   0b00111101, // 71  'G'
//   0b01110110, // 72  'H'
//   0b00110000, // 73  'I'
//   0b00001110, // 74  'J'
//   0b01110110, // 75  'K'  Same as 'H'
//   0b00111000, // 76  'L'
//   0b00000000, // 77  'M'  NO DISPLAY
//   0b01010100, // 78  'n'
//   0b00111111, // 79  'O'
//   0b01110011, // 80  'P'
//   0b01100111, // 81  'q'
//   0b01010000, // 82  'r'
//   0b01101101, // 83  'S'
//   0b01111000, // 84  't'
//   0b00111110, // 85  'U'
//   0b00111110, // 86  'V'  Same as 'U'
//   0b00000000, // 87  'W'  NO DISPLAY
//   0b01110110, // 88  'X'  Same as 'H'
//   0b01101110, // 89  'y'
//   0b01011011, // 90  'Z'  Same as '2'
//   0b00000000, // 32  ' '  BLANK
//   0b01000000, // 45  '-'  DASH
//   0b10000000, // 46  '.'  PERIOD
//   0b01100011, // 42 '*'  DEGREE ..
//   0b00001000, // 95 '_'  UNDERSCORE
// };
