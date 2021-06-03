// look in pins.pcf for all the pin names on the TinyFPGA BX board
module top (
    input CLK,    // 16MHz clock
    output LED,   // User/boot LED next to power LED
    output USBPU,  // USB pull-up resistor
    output PIN_1,
    output PIN_2,
    output PIN_3,
    output PIN_4,
    output PIN_5,
    output PIN_6,
    output PIN_7,
    output PIN_8,
    output PIN_9,
    output PIN_10,
    output PIN_11,
    output PIN_12

);
// drive USB pull-up resistor to '0' to disable USB
assign USBPU = 0;

reg [3:0] digit;

wire [6:0] sev_seg_digit;

SevSeg sevseg(CLK,digit,sev_seg_digit);

  integer number;
  reg [3:0] digit_enable_n;
  assign PIN_8 = sev_seg_digit[0];
  assign PIN_6 = sev_seg_digit[1];
  assign PIN_4 = sev_seg_digit[2];
  assign PIN_5 = sev_seg_digit[3];
  assign PIN_3 = sev_seg_digit[4];
  assign PIN_10 = sev_seg_digit[5];
  assign PIN_1 = sev_seg_digit[6];
  assign PIN_2 = 0; // dpx
  assign PIN_9 = digit_enable_n[0]; // d0
  assign PIN_7 = digit_enable_n[1]; // d1
  assign PIN_11 = digit_enable_n[2]; // d2
  assign PIN_12 = digit_enable_n[3]; // d3
  integer counter,counter2;
  integer value;
  integer display_digit;
  reg start;
  reg [15:0] bcd;
  reg [15:0] bcd_result;
  wire dv;

  DoubleDabble double_dabble(CLK,value,start,bcd,dv);

  always @(posedge CLK) begin
      counter <= counter + 1;
      counter2 <= counter2 + 1;
      if(counter>16_000_0)begin
        counter <= 0;
        value <= value+1;
        if(value>9999)begin
          value <= 0;
        end
        start <= 1;
      end
      if(dv)begin
        bcd_result <= bcd;
      end

      if(counter2>16_000)begin
        counter2<= 0;
        case (display_digit)
          0: begin
            digit <= bcd_result[3:0];
            digit_enable_n <= 4'b0111;
          end
          1: begin
            digit <= bcd_result[7:4];
            digit_enable_n <= 4'b1011;
          end
          2: begin
            digit <= bcd_result[11:8];
            digit_enable_n <= 4'b1101;
          end
          3: begin
            digit <= bcd_result[15:12];
            digit_enable_n <= 4'b1110;
          end
          default: ;
        endcase
        display_digit <= display_digit+1;
      end
       if(display_digit>3)begin
        display_digit <= 0;
       end
  end

endmodule
