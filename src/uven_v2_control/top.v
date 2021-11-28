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
    output PIN_13,
    input PIN_14,
    input PIN_15,
    input PIN_16,
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


  wire [2:0] button_input;
  SB_IO #( .PIN_TYPE(6'b000001), .PULLUP(1'b1)) button4_input( .PACKAGE_PIN(PIN_18), .D_IN_0(button_input[0]));
  SB_IO #( .PIN_TYPE(6'b000001), .PULLUP(1'b1)) button5_input( .PACKAGE_PIN(PIN_17), .D_IN_0(button_input[1]));

  assign button_input[2] = PIN_16;
  integer encoder_count;
  wire [1:0] ab_input = {PIN_15,PIN_14};
  wire [1:0] ab;
  quad q(CLK,ab[0],ab[1],encoder_count);

  assign LED = button[0];
  wire [2:0]button;
  reg [2:0]button_prev;
  debounce debounce_buttons(
    .clk(CLK),
    .reset_n(1'b1),
    .data_in(button_input),
    .data_out(button)
    );

  debounce debounce_encoder(
    .clk(CLK),
    .reset_n(1'b1),
    .data_in(ab_input),
    .data_out(ab)
    );

  parameter IDLE = 0, ARMED = 1, FIRE = 2;

  assign PIN_13 = (control_state==FIRE);
  reg [1:0] control_state;

  integer on_time=100, off_time=100, repetitions=1, intensity=75, encoder_count_offset;
  integer on_time_actual, off_time_actual, repetitions_actual;
  integer on_time_saved, off_time_saved, repetitions_saved, intensity_saved;
  integer ms_clock_counter;
  reg [1:0] value_selector;
  reg value_toggle = 0;
  reg encoder_button_prev;
  reg fire_sleep;
  reg init = 1;

  parameter ON_TIME = 0, OFF_TIME = 1, REPETITIONS = 2, INTENSITY = 3;

  always @ ( posedge CLK ) begin
    if(init)begin
      enable <= 1;
      data_in <= 0;
      init <= 0;
    end else begin
      if(control_state==IDLE)begin
        // value <= value_selector;
        if(button_prev[2] && !button[2])begin
          value_toggle <= !value_toggle;
          encoder_count_offset <= encoder_count;
          on_time_saved <= on_time;
          off_time_saved <= off_time;
          repetitions_saved <= repetitions;
          intensity_saved <= intensity;
        end
        if(control_state==IDLE)begin
          if(!value_toggle)begin
            value_selector <= value_selector+(encoder_count-encoder_count_offset)>>3;
          end else begin
            case (value_selector)
              ON_TIME: begin
                on_time <= on_time_saved+(encoder_count-encoder_count_offset);
                if(on_time>9999)begin
                  on_time <= 9999;
                end else if(on_time<0) begin
                  on_time <= 0;
                end
                value <= on_time;
              end
              OFF_TIME: begin
                off_time <= off_time_saved+(encoder_count-encoder_count_offset);
                if(off_time>9999)begin
                  off_time <= 9999;
                end else if(off_time<0) begin
                  off_time <= 0;
                end
                value <= off_time;
              end
              REPETITIONS: begin
                repetitions <= repetitions_saved+((encoder_count-encoder_count_offset)>>>1);
                if(repetitions>9999)begin
                  repetitions <= 9999;
                end else if(repetitions<1) begin
                  repetitions <= 1;
                end
                value <= repetitions;
              end
              INTENSITY: begin
                intensity <= intensity_saved+((encoder_count-encoder_count_offset)>>>4);
                if(intensity>100)begin
                  intensity <= 100;
                end else if(intensity<0) begin
                  intensity <= 0;
                end
                value <= intensity;
              end
            endcase
          end
        end
      end
      enable <= 0;
      button_prev <= button;
      case (button[1:0])
        2'b10: control_state <= IDLE;
        2'b11: begin
          value_toggle <= 0;
          control_state <= ARMED;
          on_time_actual <= 0;
          off_time_actual <= 0;
          repetitions_actual <= 0;
          fire_sleep <= 0;
          if(button_prev[1:0]!=2'b11)begin
            // one more sanity check
            if(intensity>100)begin
              data_in <= 100;
            end else if(intensity<0)begin
              data_in <= 0;
            end else begin
              data_in <= intensity;
            end
            enable <= 1;
          end
        end
        2'b01: if(button_prev[1:0]==2'b11)begin
          value_toggle <= 1;
          control_state <= FIRE;
        end
      endcase
      if(control_state==FIRE)begin
        if(repetitions_actual<repetitions)begin
          if(!fire_sleep)begin
            // one more sanity check
            if(intensity>100)begin
              data_in <= 100;
            end else if(intensity<0)begin
              data_in <= 0;
            end else begin
              data_in <= intensity;
            end
            enable <= 1;
            ms_clock_counter <= ms_clock_counter+1;
            if(ms_clock_counter>=16_000)begin
              ms_clock_counter <= 0;
              on_time_actual <= on_time_actual+1;
              value <= on_time_actual;
              if(on_time_actual>on_time-1)begin
                on_time_actual <= 0;
                fire_sleep <= 1;
              end
            end
          end else begin
            data_in <= 0;
            enable <= 1;
            ms_clock_counter <= ms_clock_counter+1;
            if(ms_clock_counter>=16_000)begin
              ms_clock_counter <= 0;
              off_time_actual <= off_time_actual+1;
              value <= off_time_actual;
              if(off_time_actual>off_time-1)begin
                off_time_actual <= 0;
                fire_sleep <= 0;
                repetitions_actual <= repetitions_actual+1;
              end
            end
          end
        end else begin
          control_state <= ARMED;
        end
      end
    end
  end

  reg [3:0] digit;

  wire [6:0] sev_seg_digit;
  reg [6:0] sev_seg_digit_manual;

  SevSeg sevseg(CLK,digit,sev_seg_digit);

  integer number;
  reg [3:0] digit_enable_n;
  assign PIN_8 = (value_toggle?sev_seg_digit[0]:sev_seg_digit_manual[0]);
  assign PIN_6 = (value_toggle?sev_seg_digit[1]:sev_seg_digit_manual[1]);
  assign PIN_4 = (value_toggle?sev_seg_digit[2]:sev_seg_digit_manual[2]);
  assign PIN_5 = (value_toggle?sev_seg_digit[3]:sev_seg_digit_manual[3]);
  assign PIN_3 = (value_toggle?sev_seg_digit[4]:sev_seg_digit_manual[4]);
  assign PIN_10 = (value_toggle?sev_seg_digit[5]:sev_seg_digit_manual[5]);
  assign PIN_1 = (value_toggle?sev_seg_digit[6]:sev_seg_digit_manual[6]);
  assign PIN_2 = 0; // dpx
  assign PIN_9 = digit_enable_n[0]; // d0
  assign PIN_7 = digit_enable_n[1]; // d1
  assign PIN_11 = digit_enable_n[2]; // d2
  assign PIN_12 = digit_enable_n[3]; // d3
  integer refresh_counter;
  integer value, value_prev = -1;
  integer display_digit;
  reg start;
  reg [15:0] bcd;
  reg [15:0] bcd_result;
  wire dv;

  DoubleDabble double_dabble(CLK,value,start,bcd,dv);

  always @(posedge CLK) begin
      if(value!=value_prev)begin
        value_prev <= value;
        start <= 1;
      end
      if(dv)begin
        bcd_result <= bcd;
      end
      refresh_counter <= refresh_counter + 1;
      if(refresh_counter>16_000)begin // 1000Hz
        refresh_counter <= 0;
        case (display_digit)
          0: begin
            if(value_toggle)begin
              digit <= bcd_result[3:0];
            end else begin
              if(control_state==IDLE)begin
                case (value_selector)
                  ON_TIME: begin sev_seg_digit_manual <= 0; end
                  OFF_TIME: begin sev_seg_digit_manual <= 7'b1000111; end
                  REPETITIONS: begin sev_seg_digit_manual <= 7'b1100111; end
                  INTENSITY: begin sev_seg_digit_manual <= 7'b0001111; end
                endcase
              end else if(control_state==ARMED)begin
                sev_seg_digit_manual <= 7'b0001111;
              end
            end
            digit_enable_n <= 4'b0111;
          end
          1: begin
            if(value_toggle)begin
              digit <= bcd_result[7:4];
            end else begin
              if(control_state==IDLE)begin
                case (value_selector)
                  ON_TIME: begin sev_seg_digit_manual <= 7'b0010101; end
                  OFF_TIME: begin sev_seg_digit_manual <= 7'b1000111; end
                  REPETITIONS: begin sev_seg_digit_manual <= 7'b1001111; end
                  INTENSITY: begin sev_seg_digit_manual <= 7'b0010101; end
                  default : sev_seg_digit_manual<= 0;
                endcase
              end else if(control_state==ARMED)begin
                sev_seg_digit_manual <= 7'b1001111;
              end
            end
            digit_enable_n <= 4'b1011;
          end
          2: begin
            if(value_toggle)begin
              digit <= bcd_result[11:8];
            end else begin
              if(control_state==IDLE)begin
                case (value_selector)
                  ON_TIME: begin sev_seg_digit_manual <= 7'b1111110; end
                  OFF_TIME: begin sev_seg_digit_manual <= 7'b1111110; end
                  REPETITIONS: begin sev_seg_digit_manual <= 7'b0000101; end
                  INTENSITY: begin sev_seg_digit_manual <= 7'b0000110; end
                  default : sev_seg_digit_manual<= 0;
                endcase
              end else if(control_state==ARMED)begin
                sev_seg_digit_manual <= 7'b1011011;
              end
            end
            digit_enable_n <= 4'b1101;
          end
          3: begin
            if(value_toggle)begin
              digit <= bcd_result[15:12];
            end else begin
              sev_seg_digit_manual <= 0;
            end
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
