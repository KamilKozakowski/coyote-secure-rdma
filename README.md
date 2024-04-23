# How to compile
clone this repo
run `run.sh` with positional argument for a flavour of coyote you want for U280 card. Then compile with `make compile`
for example 
```
    bash run.sh sha1w
```

# How to compile coyote driver
```
cd ~/Coyote/driver
make
```


# How to compile benchmark code
```
cd ~/Coyote/sw/
mkdir build_perf_rdma_sw
cd build_perf_rdma_sw
cmake .. -DTARGET_DIR=examples/perf_rdma
make
```

# How to run 

log in to clara and amy

```
ssh <USERNAME>@clara.dse.in.tum.de
ssh <USERNAME>@amy.dse.in.tum.de
```
copy bitstreams to your home folder
```
cp/scp <cyt_top.bit> ~/cyt_top.bit
```
flash bitstreams (assumes that Coyote repo is cloned to ~/Coyote)
```
sudo rmmod -f coyote_drv;
sudo bash ~/Coyote/sw/util/hot_reset.sh "e1:00.0";
cd ~/;
xilinx-shell
```
run
```
source /share/xilinx/Vivado/2022.1/settings64.sh
vivado -mode tcl -source stream.tcl
```
then
```
exit
exit
```
installing coyote driver  
*  amy 
    
```
sudo bash ~/Coyote/sw/util/hot_reset.sh "e1:00.0";
cd ~/Coyote/driver/;
sudo insmod coyote_drv.ko ip_addr_q0=0x0a000001 mac_addr_q0=000A350E24D6;
```
* clara
```
sudo bash ~/Coyote/sw/util/hot_reset.sh "e1:00.0";
cd ~/Coyote/driver/;
sudo insmod coyote_drv.ko ip_addr_q0=0a000002 mac_addr_q0=000A350E24F2;
```

## running test
* amy

        sudo FPGA_0_IP_ADDRESS=10.0.0.1 ./main -x 512 -w 1
* clara 
    
        sudo FPGA_0_IP_ADDRESS=10.0.0.2 ./main -t 131.159.102.20 -x 512 -w 1


### code for multiplexer
code for multiplexer is in `multiplexer` folder