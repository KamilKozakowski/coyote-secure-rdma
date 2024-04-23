module replace_last_packet_with_sha
(
    input logic clock,
    input logic reset,

    input logic [511:0] inp_data,
    input logic inp_valid,
    output logic inp_ready,
    input logic [63:0] inp_keep,
    input logic [5:0] inp_id,
    input logic inp_last,

    input logic [511:0] chk_data,
    input logic chk_valid,
    output logic chk_ready,
    input logic [63:0] chk_keep,
    input logic [5:0] chk_id,
    input logic chk_last,

    output logic [511:0] out,
    output logic out_valid,
    input logic out_ready,
    output logic [63:0] out_keep,
    output logic [5:0] out_id,
    output logic out_last
);


/// we don't care for out_ready for now
// WE ASSUME THAT THE OUTPUT CAN ALWAYS HANDLE A NEW PACKET

localparam [0:0]
    S0 = 1'b0,
    S1 = 1'b1;

reg [0:0] current_state, next_state;


reg inp_ready;
reg chk_ready;
reg [511:0] out;
reg out_valid;
reg [63:0] out_keep;
reg [5:0] out_id;
reg out_last;


always @(posedge clock)
    begin
        if(reset == 1'b0) 
            begin
                current_state <= S0;
            end
        else
            begin
                current_state <= next_state;
            end
    end

// choosing the next state
always @(*)
	begin
		next_state = current_state;
        
        case (current_state)
            S0: begin
                    if (inp_valid && inp_last)
                        next_state = S1;
                end
            S1: begin
                    if (chk_valid && inp_valid)
                        next_state = S0;
                end
        endcase
	end


always @*
    begin
        //////////////////////////////
        // defualts to prevent latches
        //////////////////////////////
        out = 0;
        out_valid = 0;
        inp_ready = 0;
        chk_ready = 0;

        out_keep = 0;
        out_id = 0;
        out_last = 0;
        
        //////////////////////////////
        // case statement
        case (current_state)
            S0: begin
                    if (inp_valid && !inp_last) 
                        begin
                            // send data
                            inp_ready = 1;
                            out = inp_data;
                            out_valid = inp_valid;
                            out_keep = inp_keep;
                            out_id = inp_id;
                            out_last = 0; // input is never last
                        end
                end
            S1: begin
                    if (chk_valid && inp_valid && inp_last) 
                        begin
                            // send data
                            chk_ready = 1;
                            // consume the tlast package from input
                            inp_ready = 1;
                            out = chk_data;
                            out_valid = 1;
                            out_keep = chk_keep;
                            out_id = chk_id;
                            out_last = 1;
                        end
                end
        endcase
        //////////////////////////////
    end

endmodule
