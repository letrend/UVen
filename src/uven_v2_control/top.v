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
    output PIN_12,
    input PIN_17,
    input PIN_18,
    input PIN_19,
    input PIN_20,
    input PIN_21,
    input PIN_22,
    inout PIN_23,
    inout PIN_24
);
// drive USB pull-up resistor to '0' to disable USB
assign USBPU = 0;

wire sda_out, sda_in, sda_enable, scl, scl_enable;

// tristated PULLUP for i2c
SB_IO #(
  .PIN_TYPE(6'b101001),
  .PULLUP(1'b1)
) sda_output(
  .PACKAGE_PIN(PIN_24),
  .D_OUT_0(sda_out),
  .D_IN_0(sda_in),
  .OUTPUT_ENABLE(sda_enable)
);

SB_IO #(
  .PIN_TYPE(6'b101001),
  .PULLUP(1'b1)
) scl_output(
  .PACKAGE_PIN(PIN_23),
  .D_OUT_0(scl),
  .OUTPUT_ENABLE(scl_enable)
);

wire ready;
reg enable;
reg [7:0] data_in;
wire [7:0] data_out;

i2c_controller i2c(
  .clk(CLK),
  .rst(1'b0),
  .addr(7'h2F),
  .data_in(data_in),
  .rw(1'b0),//'0' is write, '1' is read
  .enable(enable),
  .data_out(data_out),
  .ready(ready),
  .i2c_sda_out(sda_out),
  .i2c_sda_in(sda_in),
  .i2c_scl(scl),
  .sda_enable(sda_enable),
  .scl_enable(scl_enable)
  );


  wire [5:0] button_input;
  SB_IO #( .PIN_TYPE(6'b000001), .PULLUP(1'b1)) button0_input( .PACKAGE_PIN(PIN_22), .D_IN_0(button_input[0]));
  SB_IO #( .PIN_TYPE(6'b000001), .PULLUP(1'b1)) button1_input( .PACKAGE_PIN(PIN_21), .D_IN_0(button_input[1]));
  SB_IO #( .PIN_TYPE(6'b000001), .PULLUP(1'b1)) button2_input( .PACKAGE_PIN(PIN_20), .D_IN_0(button_input[2]));
  SB_IO #( .PIN_TYPE(6'b000001), .PULLUP(1'b1)) button3_input( .PACKAGE_PIN(PIN_19), .D_IN_0(button_input[3]));
  SB_IO #( .PIN_TYPE(6'b000001), .PULLUP(1'b1)) button4_input( .PACKAGE_PIN(PIN_18), .D_IN_0(button_input[4]));
  SB_IO #( .PIN_TYPE(6'b000001), .PULLUP(1'b1)) button5_input( .PACKAGE_PIN(PIN_17), .D_IN_0(button_input[5]));

  assign LED = button[0];
  wire [5:0]button;
  reg [5:0]button_prev;
  debounce debounce_buttons(
    .clk(CLK),
    .reset_n(1'b1),
    .data_in(button_input),
    .data_out(button)
    );

  always @ ( posedge CLK ) begin: BUTTON_STATE
    enable <= 0;
    if(button[0]==0 && button_prev[0]==1)begin
      value <= 0;
      data_in <= 0;
      enable <= 1;
    end else if(button[1]==0 && button_prev[1]==1)begin
      if(value<100)begin
        value <= value+1;
      end
      data_in <= value;
      enable <= 1;
    end else if(button[2]==0 && button_prev[2]==1)begin
      if(value>0)begin
        value <= value-1;
      end
      data_in <= value;
      enable <= 1;
    end


    button_prev <= button;
    // counter <= counter+1;
    // enable <= 0;
    // if(counter>16_000_0)begin
    //   enable <= 1;
    //   counter <= 0;
    //   if(state)begin
    //     data_in <= 0;
    //   end else begin
    //     data_in <= 100;
    //   end
    //   state <= !state;
    //   // data_in <= data_in+1;
    //   // if(data_in>=100)begin
    //   //   data_in <= 0;
    //   // end
    // end

  end

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
        // value <= value+1;
        // if(value>9999)begin
        //   value <= 0;
        // end
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
