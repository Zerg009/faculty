module lab5_a (
                input [7:0] in,
                output a
              );
              
assign a = in[0] & in[1] & in[2] & in[3] & in[4] & in[5]& in[6] & in[7];

endmodule
