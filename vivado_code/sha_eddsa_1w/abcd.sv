`timescale 1ns / 1ps

import lynxTypes::*;

module abcd #( parameter integer AXIS_TDATA_WIDTH = 512) (

    // clock and reset

    input wire aclk,
    input wire areset,
    AXI4SR.s                                 s_axis_host,
    AXI4SR.m                                 m_axis_host
);

// user logic

inputFIFODuplicate dupFifo(

    .clock(aclk),
    .reset(areset),

    .input_axis_tvalid(s_axis_host.tvalid),
    .input_axis_tready(s_axis_host.tready),
    .input_axis_tdata(s_axis_host.tdata),
    .input_axis_tkeep(s_axis_host.tkeep),
    .input_axis_tid(s_axis_host.tid),
    .input_axis_tlast(s_axis_host.tlast),

    
    .output1_axis_tvalid(qq2DATA.tvalid),
    .output1_axis_tready(qq2DATA.tready),
    .output1_axis_tdata(qq2DATA.tdata),
    .output1_axis_tkeep(qq2DATA.tkeep),
    .output1_axis_tid(qq2DATA.tid),
    .output1_axis_tlast(qq2DATA.tlast),


    .output2_axis_tvalid(qq2CHECKSUM.tvalid),
    .output2_axis_tready(qq2CHECKSUM.tready),
    .output2_axis_tdata(qq2CHECKSUM.tdata),
    .output2_axis_tkeep(qq2CHECKSUM.tkeep),
    .output2_axis_tid(qq2CHECKSUM.tid),
    .output2_axis_tlast(qq2CHECKSUM.tlast)
);

AXI4SR qq2DATA();
AXI4SR qq2CHECKSUM();
AXI4SR qq2CHECKSUMout();

logic a;
sha512 c1(.ap_clk(aclk),
        .ap_rst_n(areset),
        .input_r_TDATA({1'b0,qq2CHECKSUM.tlast, qq2CHECKSUM.tid, qq2CHECKSUM.tkeep, qq2CHECKSUM.tdata}),
        .input_r_TVALID(qq2CHECKSUM.tvalid),
        .input_r_TREADY(qq2CHECKSUM.tready),
        .output_r_TREADY(qq2CHECKSUMout2.tready),
        .output_r_TDATA({a, qq2CHECKSUMout2.tlast, qq2CHECKSUMout2.tid, qq2CHECKSUMout2.tkeep, qq2CHECKSUMout2.tdata}),
        .output_r_TVALID(qq2CHECKSUMout2.tvalid));

logic a1;

eddsa c1(.ap_clk(aclk),
        .ap_rst_n(areset),
        .input_r_TDATA({1'b0,qq2CHECKSUMout2.tlast, qq2CHECKSUMout2.tid, qq2CHECKSUMout2.tkeep, qq2CHECKSUMout2.tdata}),
        .input_r_TVALID(qq2CHECKSUMout2.tvalid),
        .input_r_TREADY(qq2CHECKSUMout2.tready),
        .output_r_TREADY(qq2CHECKSUMout4.tready),
        .output_r_TDATA({a1, qq2CHECKSUMout4.tlast, qq2CHECKSUMout4.tid, qq2CHECKSUMout4.tkeep, qq2CHECKSUMout4.tdata}),
        .output_r_TVALID(qq2CHECKSUMout4.tvalid));


AXI4SR qq2CHECKSUMout2();
AXI4SR qq2CHECKSUMout3();
AXI4SR qq2CHECKSUMout4();

axisr_data_fifo_512 fifo2 (
    .s_axis_aresetn(areset),
    .s_axis_aclk(aclk),
    .s_axis_tvalid(qq2CHECKSUMout4.tvalid),
    .s_axis_tready(qq2CHECKSUMout4.tready),
    .s_axis_tdata(qq2CHECKSUMout4.tdata),
    .s_axis_tkeep(qq2CHECKSUMout4.tkeep),
    .s_axis_tid  (qq2CHECKSUMout4.tid),
    .s_axis_tlast(qq2CHECKSUMout4.tlast),

    
    .m_axis_tvalid(qq2CHECKSUMout3.tvalid),
    .m_axis_tready(qq2CHECKSUMout3.tready),
    .m_axis_tdata(qq2CHECKSUMout3.tdata),
    .m_axis_tkeep(qq2CHECKSUMout3.tkeep),
    .m_axis_tid  (qq2CHECKSUMout3.tid),
    .m_axis_tlast(qq2CHECKSUMout3.tlast)
);


AXI4SR qq2();
replace_last_packet_with_sha d1(
    .clock(aclk),
    .reset(areset),

    .inp_data(qq2DATA.tdata),
    .inp_valid(qq2DATA.tvalid),
    .inp_ready(qq2DATA.tready),
    .inp_keep(qq2DATA.tkeep),
    .inp_id(qq2DATA.tid),
    .inp_last(qq2DATA.tlast),

    .chk_data(qq2CHECKSUMout3.tdata),
    .chk_valid(qq2CHECKSUMout3.tvalid),
    .chk_ready(qq2CHECKSUMout3.tready),
    .chk_keep(qq2CHECKSUMout3.tkeep),
    .chk_id(qq2CHECKSUMout3.tid),
    .chk_last(qq2CHECKSUMout3.tlast),

    .out(qq2.tdata),
    .out_valid(qq2.tvalid),
    .out_ready(qq2.tready),
    .out_keep(qq2.tkeep),
    .out_id(qq2.tid),
    .out_last(qq2.tlast)
);

axisr_data_fifo_512 fifo3 (
    .s_axis_aresetn(areset),
    .s_axis_aclk(aclk),
    .s_axis_tvalid(qq2.tvalid),
    .s_axis_tready(qq2.tready),
    .s_axis_tdata(qq2.tdata),
    .s_axis_tkeep(qq2.tkeep),
    .s_axis_tid  (qq2.tid),
    .s_axis_tlast(qq2.tlast),

    
    .m_axis_tvalid(m_axis_host.tvalid),
    .m_axis_tready(m_axis_host.tready),
    .m_axis_tdata(m_axis_host.tdata),
    .m_axis_tkeep(m_axis_host.tkeep),
    .m_axis_tid  (m_axis_host.tid),
    .m_axis_tlast(m_axis_host.tlast)
);

endmodule