#!/bin/bash

coyote_repo="https://github.com/fpgasystems/Coyote.git"
scripts_repo="https://github.com/KamilKozakowski/coyote-secure-rdma.git"

if [ -f /share/xilinx/Vivado/2022.1/bin/vivado ]; then
    echo 'Vivado mounted at /share/xilinx/Vivado/2022.1/bin/vivado'
else
    echo 'Vivado is not mounted at /share/xilinx. Mount it there with'
    echo 'sudo mount -t nfs [2a09:80c0:102::f000:0]:/export/share /share'
    exit 1
    # sudo mount -t nfs [2a09:80c0:102::f000:0]:/export/share /share
fi
if [ -f /share/xilinx/Vitis_HLS/2022.1/bin/vitis_hls ]; then
    echo 'Vitis mounted at /share/xilinx/Vitis_HLS/2022.1/bin/vitis_hls'
else
    echo 'Vitis is not mounted at /share/xilinx. Mount it there with'
    echo 'sudo mount -t nfs [2a09:80c0:102::f000:0]:/export/share /share'
    exit 1
    # sudo mount -t nfs [2a09:80c0:102::f000:0]:/export/share /share
fi

vivado="/share/xilinx/Vivado/2022.1/bin/vivado"
vitis="/share/xilinx/Vitis_HLS/2022.1/bin/vitis_hls"

git clone $coyote_repo
cd Coyote
git reset --hard c00458ff3422f001036c9df19569af5979d210ba

mkdir --parent ./hw/secure-kamil/iprepo/sha512
mkdir --parent ./hw/secure-kamil/iprepo/hmac
mkdir --parent ./hw/secure-kamil/iprepo/eddsa
mkdir --parent ./hw/secure-kamil/iprepo/rsa


# git clone $scripts_repo


cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs synthesis && cd ..
cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs services && cd ..

cd coyote-secure-rdma

bash ./sha512_1W.sh $vivado $vitis

cd ../hw/
mkdir --parent build_kamil
cd build_kamil

cmake .. \
 -DVIVADO_ROOT_DIR=/share/xilinx/Vivado/2022.1 \
 -DVIVADO_HLS_ROOT_DIR=/share/xilinx/Vitis_HLS/2022.1 \
 -DFDEV_NAME=u280 -DEXAMPLE=perf_rdma_host -DACLK_F=250 

make shell

cd coyote-secure-rdma 
$vivado -mode tcl -source add_files_sha512_1W.tcl
cd ..
# $vivado -mode tcl -source add_files_sha512_2W.tcl
