`timescale 1ns / 1ps

import lynxTypes::*;

module abcd2 #(
    parameter integer AXIS_TDATA_WIDTH = 512
) (
    input wire aclk,
    input wire areset,
    AXI4SR.s   s_axis_host,
    AXI4SR.m   m_axis_host
);

AXI4SR qq1();
AXI4SR qq2();
AXI4SR qq2DATA();
AXI4SR qq2CHECKSUM();
AXI4SR qq2CHECKSUMout();

axisr_data_fifo_512 fifo1 (
    .s_axis_aclk   (aclk),
    .s_axis_aresetn(areset),

    .s_axis_tvalid(s_axis_host.tvalid),
    .s_axis_tready(s_axis_host.tready),
    .s_axis_tdata (s_axis_host.tdata),
    .s_axis_tkeep (s_axis_host.tkeep),
    .s_axis_tid   (s_axis_host.tid),
    .s_axis_tlast (s_axis_host.tlast),

    .m_axis_tvalid(qq1.tvalid),
    .m_axis_tready(qq1.tready),
    .m_axis_tdata (qq1.tdata),
    .m_axis_tkeep (qq1.tkeep),
    .m_axis_tid   (qq1.tid),
    .m_axis_tlast (qq1.tlast)
);

inputFIFODuplicate dupFifo(
    .clock(aclk),
    .reset(areset),

    .input_axis_tvalid(qq1.tvalid),
    .input_axis_tready(qq1.tready),
    .input_axis_tdata (qq1.tdata),
    .input_axis_tkeep (qq1.tkeep),
    .input_axis_tid   (qq1.tid),
    .input_axis_tlast (qq1.tlast),

    .output1_axis_tvalid(qq2DATA.tvalid),
    .output1_axis_tready(qq2DATA.tready),
    .output1_axis_tdata (qq2DATA.tdata),
    .output1_axis_tkeep (qq2DATA.tkeep),
    .output1_axis_tid   (qq2DATA.tid),
    .output1_axis_tlast (qq2DATA.tlast),

    .output2_axis_tvalid(qq2CHECKSUM.tvalid),
    .output2_axis_tready(qq2CHECKSUM.tready),
    .output2_axis_tdata (qq2CHECKSUM.tdata),
    .output2_axis_tkeep (qq2CHECKSUM.tkeep),
    .output2_axis_tid   (qq2CHECKSUM.tid),
    .output2_axis_tlast (qq2CHECKSUM.tlast)
);

logic a;
logic verify_data;
logic verify_ready;
logic verify_valid;
logic [511:0] expected_hmac;

hmac c1 (
    .ap_clk  (aclk),
    .ap_rst_n(areset),

    .input_r_TDATA ({1'b0, qq2CHECKSUM.tlast, qq2CHECKSUM.tid, qq2CHECKSUM.tkeep, qq2CHECKSUM.tdata}),
    .input_r_TVALID(qq2CHECKSUM.tvalid),
    .input_r_TREADY(qq2CHECKSUM.tready),

    .output_r_TREADY(qq2CHECKSUMout.tready),
    .output_r_TDATA ({a, qq2CHECKSUMout.tlast, qq2CHECKSUMout.tid, qq2CHECKSUMout.tkeep, qq2CHECKSUMout.tdata}),
    .output_r_TVALID(qq2CHECKSUMout.tvalid)
);

assign qq2CHECKSUMout.tready = verify_ready;

// Compare received HMAC with calculated HMAC
always_ff @(posedge aclk) begin
    if(!areset) begin
        verify_data <= 1'b1; // 0 if HMAC matches
        verify_valid <= 1'b0;
    end
    else begin
        verify_valid <= 1'b0;

        if (qq2CHECKSUM.tvalid && qq2CHECKSUM.tready && qq2CHECKSUM.tlast) begin
            expected_hmac <= qq2CHECKSUM.tdata;
        end

        if (qq2CHECKSUMout.tvalid && qq2CHECKSUMout.tready) begin
            // 0 if HMAC matches
            verify_data <= (qq2CHECKSUMout.tdata != expected_hmac);
            verify_valid <= 1'b1;
        end
    end
end

check_sha d1(
    .clock(aclk),
    .reset(areset),

    .inp_valid(qq2DATA.tvalid),
    .inp_ready(qq2DATA.tready),
    .inp_data (qq2DATA.tdata),
    .inp_keep (qq2DATA.tkeep),
    .inp_id   (qq2DATA.tid),
    .inp_last (qq2DATA.tlast),

    .comparison_valid (verify_valid),
    .comparison_ready (verify_ready),
    .comparison_result(verify_data),

    .out_valid(qq2.tvalid),
    .out_ready(qq2.tready),
    .out_data (qq2.tdata),
    .out_keep (qq2.tkeep),
    .out_id   (qq2.tid),
    .out_last (qq2.tlast)
);

axisr_data_fifo_512 fifo3 (
    .s_axis_aclk   (aclk),
    .s_axis_aresetn(areset),

    .s_axis_tvalid(qq2.tvalid),
    .s_axis_tready(qq2.tready),
    .s_axis_tdata (qq2.tdata),
    .s_axis_tkeep (qq2.tkeep),
    .s_axis_tid   (qq2.tid),
    .s_axis_tlast (qq2.tlast),

    .m_axis_tvalid(m_axis_host.tvalid),
    .m_axis_tready(m_axis_host.tready),
    .m_axis_tdata (m_axis_host.tdata),
    .m_axis_tkeep (m_axis_host.tkeep),
    .m_axis_tid   (m_axis_host.tid),
    .m_axis_tlast (m_axis_host.tlast)
);

endmodule