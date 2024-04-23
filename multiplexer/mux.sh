
git clone https://github.com/fpgasystems/Coyote.git
cd Coyote
git reset --hard c00458ff3422f001036c9df19569af5979d210ba


#####
# !!!!!!!!!!!!!!!!!!
## DEBUG INFO
# !!!!!!!!!!!!!!!!!!
sed -i '21s|.*|#define VERBOSE_DEBUG_3 // Perf|' ./sw/include/cDefs.hpp
sed -i '514s|$|\n	std::cout << "READ cpid \t" <<  cpid << std::endl;|' ./sw/src/cProcess.cpp
########


#####
# !!!!!!!!!!!!!!!!!!
## DRIVER FIX
# !!!!!!!!!!!!!!!!!!
sed -i '31s|.*|#include <functional>\n|' ./sw/include/cService.hpp
########



#####
git checkout ./hw/hdl/slave/static_slave.sv

sed -i '103s|.*|  output logic [63:0]       kamil,\n|' ./hw/hdl/slave/static_slave.sv
sed -i '342s|.*|localparam integer KAMIL                  = 122;\n|' ./hw/hdl/slave/static_slave.sv
sed -i '354s|.*|    slv_reg[KAMIL] <= 0;\n|' ./hw/hdl/slave/static_slave.sv
    
sed -i '571s|.*|        KAMIL: // Kamil Context\n|' ./hw/hdl/slave/static_slave.sv
sed -i '572s|.*|          for (int i = 0; i < AXIL_DATA_BITS/8; i++) begin\n|' ./hw/hdl/slave/static_slave.sv
sed -i '573s|.*|            if(s_axi_ctrl.wstrb[i]) begin\n|' ./hw/hdl/slave/static_slave.sv
sed -i '574s|.*|              slv_reg[KAMIL][(i*8)+:8] <= s_axi_ctrl.wdata[(i*8)+:8];\n|' ./hw/hdl/slave/static_slave.sv
sed -i '575s|.*|            end\n|' ./hw/hdl/slave/static_slave.sv
sed -i '576s|.*|          end\n|' ./hw/hdl/slave/static_slave.sv


sed -i '1103s|.*|\nassign kamil = slv_reg[KAMIL];\n|' ./hw/hdl/slave/static_slave.sv

#####
git checkout ./hw/scripts/wr_hdl/template_gen/user_logic.txt

sed -i '108s|$|,\n|' ./hw/scripts/wr_hdl/template_gen/user_logic.txt
sed -i '109s|.*|    input  wire[63:0]           kamil|' ./hw/scripts/wr_hdl/template_gen/user_logic.txt

#####
git checkout ./hw/scripts/wr_hdl/template_gen/dynamic_wrapper.txt

sed -i '10s|$|\n|' ./hw/scripts/wr_hdl/template_gen/dynamic_wrapper.txt
sed -i '11s|.*|    input logic[63:0]                       kamil,\n|' ./hw/scripts/wr_hdl/template_gen/dynamic_wrapper.txt

sed -i '1439s|$|\n|' ./hw/scripts/wr_hdl/template_gen/dynamic_wrapper.txt
sed -i '1440s|.*|        .kamil                  (kamil),|' ./hw/scripts/wr_hdl/template_gen/dynamic_wrapper.txt

#####

git checkout ./hw/scripts/wr_hdl/template_gen/user_wrapper.txt

sed -i '13s|^|\n|' ./hw/scripts/wr_hdl/template_gen/user_wrapper.txt
sed -i '13s|.*|    input  logic[63:0]                          kamil,|' ./hw/scripts/wr_hdl/template_gen/user_wrapper.txt

sed -i '1170s|$|,\n|' ./hw/scripts/wr_hdl/template_gen/user_wrapper.txt
sed -i '1171s|.*|	        .kamil(kamil)|' ./hw/scripts/wr_hdl/template_gen/user_wrapper.txt

####
git checkout ./hw/scripts/wr_hdl/template_gen/top_shell.txt

sed -i '1s|^|\n|' ./hw/scripts/wr_hdl/template_gen/top_shell.txt
sed -i '1s|.*|    logic[63:0] kamil1;\n|' ./hw/scripts/wr_hdl/template_gen/top_shell.txt

sed -i '39s|^|\n|' ./hw/scripts/wr_hdl/template_gen/top_shell.txt
sed -i '39s|.*|        .kamil(kamil1),|' ./hw/scripts/wr_hdl/template_gen/top_shell.txt

sed -i '353s|^|\n|' ./hw/scripts/wr_hdl/template_gen/top_shell.txt
sed -i '39s|.*|        .kamil(kamil1),|' ./hw/scripts/wr_hdl/template_gen/top_shell.txt

cd hw
mkdir dddtr
cd dddtr

cmake ..  -DVIVADO_ROOT_DIR=/share/xilinx/Vivado/2022.1 -DVIVADO_HLS_ROOT_DIR=/share/xilinx/Vitis_HLS/2022.1 -DFDEV_NAME=u280 -DEXAMPLE=perf_rdma_host -DACLK_F=250
make shell

cat <<EOT > kamil_add_files.tcl
open_project ./lynx/lynx.xpr
add_files kamil_module.sv
exit
EOT

vivado -mode tcl -source kamil_add_files.tcl 

####################################

#########
# DRIVER
#########

####
git checkout ./driver/coyote_dev.h

sed -i "327s|.*|#define KAMIL_CTX _IOW('D', 40, unsigned long)     // qp context\n|" ./driver/coyote_dev.h
sed -i "328s|.*|#define KAMIL_CTX_READ _IOR('D', 41, unsigned long)     // qp context\n|" ./driver/coyote_dev.h

####
git checkout ./driver/fpga_dev.c

sed -i '311s|^|\n|' ./driver/fpga_dev.c

sed -i '311s|.*|#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,4,0)\n|' ./driver/fpga_dev.c
sed -i '312s|.*|    fpga_class = class_create(DEV_NAME);\n|' ./driver/fpga_dev.c
sed -i '313s|.*|#else|' ./driver/fpga_dev.c
sed -i '315s|.*|#endif|' ./driver/fpga_dev.c


####
git checkout ./driver/fpga_fops.c

sed -i '449s|.*|    // kamil context\n|' ./driver/fpga_fops.c
sed -i '450s|.*|    case KAMIL_CTX:\n|' ./driver/fpga_fops.c
sed -i '451s|.*|        ret_val = copy_from_user(&tmp, (unsigned long*) arg, 4 * sizeof(unsigned long));\n|' ./driver/fpga_fops.c
sed -i '452s|.*|        if (ret_val != 0) {\n|' ./driver/fpga_fops.c
sed -i '453s|.*|            dbg_info("user data could not be coppied, return %d\\n", ret_val);\n|' ./driver/fpga_fops.c
sed -i '454s|.*|        } else {\n|' ./driver/fpga_fops.c
sed -i '455s|.*|            dbg_info("writing qp context ...");\n|' ./driver/fpga_fops.c
sed -i '456s|.*|            spin_lock(&pd->stat_lock);\n|' ./driver/fpga_fops.c
sed -i '457s|.*|\n|' ./driver/fpga_fops.c
sed -i '458s|.*|            for (i = 0; i < 4; i++) {\n|' ./driver/fpga_fops.c
sed -i '459s|.*|                // 0 - 96\n|' ./driver/fpga_fops.c
sed -i '460s|.*|                // 1 - 97\n|' ./driver/fpga_fops.c
sed -i '461s|.*|                pd->fpga_stat_cnfg->net_0_debug[26+i] = tmp[i];\n|' ./driver/fpga_fops.c
sed -i '462s|.*|            }\n|' ./driver/fpga_fops.c
sed -i '463s|.*|\n|' ./driver/fpga_fops.c
sed -i '464s|.*|            spin_unlock(&pd->stat_lock);\n|' ./driver/fpga_fops.c
sed -i '465s|.*|        }\n|' ./driver/fpga_fops.c
sed -i '466s|.*|        break;\n|' ./driver/fpga_fops.c
sed -i '467s|.*|    // kamil context read\n|' ./driver/fpga_fops.c
sed -i '468s|.*|    case KAMIL_CTX_READ:\n|' ./driver/fpga_fops.c
sed -i '469s|.*|        tmp[0] = ((uint64_t)pd->fpga_stat_cnfg->net_0_debug[26]);\n|' ./driver/fpga_fops.c
sed -i '470s|.*|        // tmp[1] = ((uint64_t)pd->fpga_stat_cnfg->net_0_debug[27]);\n|' ./driver/fpga_fops.c
sed -i '471s|.*|        // tmp[2] = ((uint64_t)pd->fpga_stat_cnfg->net_0_debug[28]);\n|' ./driver/fpga_fops.c
sed -i '472s|.*|        // tmp[3] = ((uint64_t)pd->fpga_stat_cnfg->net_0_debug[29]);\n|' ./driver/fpga_fops.c
sed -i '473s|.*|\n|' ./driver/fpga_fops.c
sed -i '474s|.*|        dbg_info("reading config 0x%llx\\n", tmp[0]);\n|' ./driver/fpga_fops.c
sed -i '475s|.*|        // dbg_info("reading config 0x%llx\\n", tmp[1]);\n|' ./driver/fpga_fops.c
sed -i '476s|.*|        // dbg_info("reading config 0x%llx\\n", tmp[2]);\n|' ./driver/fpga_fops.c
sed -i '477s|.*|        // dbg_info("reading config 0x%llx\\n", tmp[3]);\n|' ./driver/fpga_fops.c
sed -i '478s|.*|        ret_val = copy_to_user((unsigned long *)arg, &tmp, sizeof(unsigned long));\n|' ./driver/fpga_fops.c
sed -i '479s|.*|        break;\n|' ./driver/fpga_fops.c

####
git checkout ./driver/fpga_mmu.c

sed -i 's|ret_val = get_user_pages_remote(curr_mm, (unsigned long)start, n_pages, 1, user_pg->hpages, NULL, NULL);|ret_val = get_user_pages_remote(curr_mm, (unsigned long)start, n_pages, 1, user_pg->hpages, NULL);|g' ./driver/fpga_mmu.c
sed -i 's|ret_val = get_user_pages_remote(curr_task, curr_mm, (unsigned long)start, n_pages, 1, user_pg->hpages, NULL, NULL);|ret_val = get_user_pages_remote(curr_task, curr_mm, (unsigned long)start, n_pages, 1, user_pg->hpages, NULL);|g' ./driver/fpga_mmu.c

####
git checkout ./sw/include/cProcess.hpp

sed -i '214s|.*|\n    //kamil\n    void ioc(unsigned int cmd,uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4);\n|' ./sw/include/cProcess.hpp


####
git checkout ./sw/src/cProcess.cpp


sed -i '866s|.*|void cProcess::ioc(unsigned int cmd,uint64_t data1, uint64_t data2, uint64_t data3, uint64_t data4)\n|' ./sw/src/cProcess.cpp
sed -i '867s|.*|{\n|' ./sw/src/cProcess.cpp
sed -i '868s|.*|   uint64_t tmp[4];\n|' ./sw/src/cProcess.cpp
sed -i '869s|.*|\n|' ./sw/src/cProcess.cpp
sed -i '870s|.*|   tmp[0]= data1;\n|' ./sw/src/cProcess.cpp
sed -i '871s|.*|   tmp[1]= data2;\n|' ./sw/src/cProcess.cpp
sed -i '872s|.*|   tmp[2]= data3;\n|' ./sw/src/cProcess.cpp
sed -i '873s|.*|   tmp[3]= data4;\n|' ./sw/src/cProcess.cpp
sed -i '874s|.*|\n|' ./sw/src/cProcess.cpp
sed -i '875s|.*|   std::cout << "!" << std::hex <<tmp[0] << " " << tmp[1] << " " << tmp[2] << " "  << tmp[3] << std::endl;\n|' ./sw/src/cProcess.cpp
sed -i '876s|.*|   // register pid\n|' ./sw/src/cProcess.cpp
sed -i '877s|.*|   ioctl(fd, cmd, &tmp);\n|' ./sw/src/cProcess.cpp
sed -i '878s|.*|   std::cout << "!!" << std::hex <<tmp[0] << " " << tmp[1] << " " << tmp[2] << " " << tmp[3] << std::endl;\n|' ./sw/src/cProcess.cpp
sed -i '879s|.*|}\n|' ./sw/src/cProcess.cpp

