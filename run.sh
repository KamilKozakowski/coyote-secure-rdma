#!/bin/bash

if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
    exit 1
fi

SHA512_1W='sha1w'
SHA512_2W='sha2w'
HMAC_1W='hmac1w'
HMAC_2W='hmac2w'
SHA512_RSA_1W='sha_rsa1w'
SHA512_RSA_2W='sha_rsa2w'
SHA512_EDDSA_1W='sha_eddsa1w'
SHA512_EDDSA_2W='sha_eddsa2w'


function sha1w() {
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs services && cd ..

    cd coyote-secure-rdma

    bash ./1W.sh $vivado $vitis
}
function sha1w_out() {
    $vivado -mode tcl -source add_files_sha512_1W.tcl
}

function sha2w() {
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs services && cd ..

    cd coyote-secure-rdma

    bash ./2W.sh $vivado $vitis
}
function sha2w_out() {
    $vivado -mode tcl -source add_files_sha512_2W.tcl
}

function hmac1w() {
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs services && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_hmac.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_hmac.tcl -tclargs services && cd ..

    cd coyote-secure-rdma

    bash ./1W.sh $vivado $vitis
}
function hmac1w_out() {
    $vivado -mode tcl -source add_files_hmac_1W.tcl
}

function hmac2w() {
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs services && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_hmac.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_hmac.tcl -tclargs services && cd ..

    cd coyote-secure-rdma

    bash ./2W.sh $vivado $vitis
}
function hmac2w_out() {
    $vivado -mode tcl -source add_files_hmac_1W.tcl
}

function sharsa1w() {
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs services && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_rsa.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_rsa.tcl -tclargs services && cd ..

    cd coyote-secure-rdma

    bash ./1W.sh $vivado $vitis
}
function sharsa1w_out() {
    $vivado -mode tcl -source add_files_sha512_rsa_1W.tcl
}

function sharsa2w() {
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs services && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_rsa.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_rsa.tcl -tclargs services && cd ..

    cd coyote-secure-rdma

    bash ./2W.sh $vivado $vitis
}
function sharsa2w_out() {
    $vivado -mode tcl -source .tcl
}

function shaeddsa1w() {
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs services && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_eddsa.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_eddsa.tcl -tclargs services && cd ..

    cd coyote-secure-rdma

    bash ./1W.sh $vivado $vitis
}
function shaeddsa1w_out() {
    $vivado -mode tcl -source add_files_sha512_eddsa_1W.tcl
}

function shaeddsa2w() {
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_sha512.tcl -tclargs services && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_eddsa.tcl -tclargs synthesis && cd ..
    cd ./coyote-secure-rdma && $vitis -f make_eddsa.tcl -tclargs services && cd ..

    cd coyote-secure-rdma

    bash ./2W.sh $vivado $vitis
}
function shaeddsa2w_out() {
    $vivado -mode tcl -source add_files_sha512_eddsa_2W.tcl
}



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



case $1 in

  "$SHA512_1W")
    sha1w
    ;;

  "$SHA512_2W")
    sha2w
    ;;

  "$HMAC_1W")
    hmac1w
    ;;

  "$HMAC_2W")
    hmac2w
    ;;

  "$SHA512_RSA_1W")
    sharsa1w
    ;;

  "$SHA512_RSA_2W")
    sharsa2w
    ;;

  "$SHA512_EDDSA_1W")
    shaeddsa1w
    ;;

  "$SHA512_EDDSA_2W")
    shaeddsa2w
    ;;

  *)
    echo -n "unknown"
    ;;
esac

cd ../hw/
mkdir --parent build_kamil
cd build_kamil

cmake .. \
 -DVIVADO_ROOT_DIR=/share/xilinx/Vivado/2022.1 \
 -DVIVADO_HLS_ROOT_DIR=/share/xilinx/Vitis_HLS/2022.1 \
 -DFDEV_NAME=u280 -DEXAMPLE=perf_rdma_host -DACLK_F=250 

make shell

cd coyote-secure-rdma 


case $1 in

  "$SHA512_1W")
    sha1w_out
    ;;

  "$SHA512_2W")
    sha2w_out
    ;;

  "$HMAC_1W")
    hmac1w_out
    ;;

  "$HMAC_2W")
    hmac2w_out
    ;;

  "$SHA512_RSA_1W")
    sharsa1w_out
    ;;

  "$SHA512_RSA_2W")
    sharsa2w_out
    ;;

  "$SHA512_EDDSA_1W")
    shaeddsa1w_out
    ;;

  "$SHA512_EDDSA_2W")
    shaeddsa2w_out
    ;;

  *)
    echo -n "unknown"
    ;;
esac

cd ..
# make compile