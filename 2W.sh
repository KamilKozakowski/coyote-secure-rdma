#!/bin/bash
vivado=$1
vitis=$2

git checkout ../hw/scripts/wr_hdl/template_gen/user_logic.txt

sed -i '179s|.*|AXI4SR axis_rdma_0_sink2();\n|'         ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '180s|.*|AXI4SR axis_host_sink2();\n|'           ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '181s|.*|\n|'                                    ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '182s|.*|////////\n|'                            ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '183s|.*|// network to pcie\n|'                  ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '184s|.*|////////\n|'                            ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '185s|.*|abcd2 a1(\n|'                           ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '186s|.*|\n|'                                    ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '187s|.*|   // clock and reset\n|'               ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '188s|.*|   .aclk(aclk),\n|'                     ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '189s|.*|   .areset(aresetn),\n|'                ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '190s|.*|    \n|'                                ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '191s|.*|   .m_axis_host(axis_host_src),\n|'     ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '192s|.*|   .s_axis_host(axis_rdma_0_sink2)\n|'  ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '193s|.*|);\n|'                                  ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '194s|.*|\n|'                                    ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '195s|.*|////////\n|'                            ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '196s|.*|// pcie to network\n|'                  ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '197s|.*|////////\n|'                            ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '198s|.*|abcd a2(\n|'                            ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '199s|.*|\n|'                                    ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '200s|.*|    // clock and reset\n|'              ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '201s|.*|    .aclk(aclk),|\n'                    ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '202s|.*|    .areset(aresetn),\n|'               ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '203s|.*|    |\n'                                ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '204s|.*|    .m_axis_host(axis_rdma_0_src),\n|'  ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '205s|.*|    .s_axis_host(axis_host_sink2)\n|'   ../hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '206s|.*|);\n|'                                  ../hw/scripts/wr_hdl/template_gen/user_logic.txt


git checkout ../hw/hdl/operators/examples/perf_rdma/perf_rdma_card_c0_0.svh

sed -i 's|AXISR_ASSIGN(axis_host_sink, axis_rdma_0_src)|AXISR_ASSIGN(axis_host_sink, axis_host_sink2)|g' ../hw/hdl/operators/examples/perf_rdma/perf_rdma_card_c0_0.svh
sed -i 's|AXISR_ASSIGN(axis_rdma_0_sink, axis_host_src)|AXISR_ASSIGN(axis_rdma_0_sink, axis_rdma_0_sink2)|g' ../hw/hdl/operators/examples/perf_rdma/perf_rdma_card_c0_0.svh
