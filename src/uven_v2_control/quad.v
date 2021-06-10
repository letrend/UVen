module quad(clk, quadA, quadB, count);
input clk, quadA, quadB;
output integer count;

reg quadA_delayed, quadB_delayed;
always @(posedge clk) quadA_delayed <= quadA;
always @(posedge clk) quadB_delayed <= quadB;

wire count_enable = quadA ^ quadA_delayed ^ quadB ^ quadB_delayed;
wire count_direction = quadA ^ quadB_delayed;

integer count;
always @(posedge clk)
begin
  if(count_enable)
  begin
    if(count_direction) count<=count+1; else count<=count-1;
  end
end

endmodule
