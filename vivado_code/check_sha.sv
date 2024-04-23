module check_sha
(
    input logic clock,
    input logic reset,

    input logic [511:0] inp_data,
    input logic inp_valid,
    output logic inp_ready,
    input logic [63:0] inp_keep,
    input logic [5:0] inp_id,
    input logic inp_last,

    input logic comparison_result,
    input logic comparison_valid,
    output logic comparison_ready,

    output logic [511:0] out,
    output logic out_valid,
    input logic out_ready,
    output logic [63:0] out_keep,
    output logic [5:0] out_id,
    output logic out_last
);


/// we don't care for out_ready for now
// WE ASSUME THAT THE OUTPUT CAN ALWAYS HANDLE A NEW PACKET

localparam [1:0]
    START = 2'b00,
    S_CORRECT = 2'b01,
    S_INCORRECT = 2'b10;

reg [1:0] current_state, next_state;


reg inp_ready;
reg comparison_ready;
reg [511:0] out;
reg out_valid;
reg [63:0] out_keep;
reg [5:0] out_id;
reg out_last;


always @(posedge clock)
    begin
        if(reset == 1'b0) 
            begin
                current_state <= START;
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
            START: begin
                    if (comparison_valid)
                        if(comparison_result == 0)
                            next_state = S_CORRECT;
                        else
                            next_state = S_INCORRECT;
                end
            S_CORRECT: begin
                    if (inp_last && inp_valid)
                        next_state = START;
                end
            S_INCORRECT: begin
                    if (inp_last && inp_valid)
                        next_state = START;
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
        comparison_ready = 0;

        out_keep = 0;
        out_id = 0;
        out_last = 0;
        
        //////////////////////////////
        // case statement
        case (current_state)
            START: begin
                    comparison_ready = 1;
                end
            S_CORRECT: begin
                    if (inp_valid) 
                        begin
                            // send data
                            inp_ready = 1;
                            out = inp_data;
                            out_valid = inp_valid;
                            out_keep = inp_keep;
                            out_id = inp_id;
                            out_last = inp_last;
                        end
                end
            S_INCORRECT: begin
                    if (inp_valid) 
                        begin
                            // send data
                            inp_ready = 1;
                            out = 0;
                            out_valid = inp_valid;
                            out_keep = inp_keep;
                            out_id = inp_id;
                            out_last = inp_last;
                        end
                end
        endcase
        //////////////////////////////
    end

endmodule
