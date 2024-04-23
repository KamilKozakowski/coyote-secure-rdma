open_project sha512_prj

open_solution "solution1"
set_part xcu280-fsvh2892-2L-e
create_clock -period 4 -name default

set_top sha512

add_files ./sha512/sha512.cpp
add_files ./sha512/sha512.hpp


#Check which command
set command [lindex $argv 2]

if {$command == "synthesis"} {
   csynth_design
} elseif {$command == "services"} {
  file mkdir ../hw/secure-kamil/iprepo
  file delete -force ../hw/secure-kamil/iprepo/sha512
  file copy -force ./sha512_prj/solution1/impl/verilog ../hw/secure-kamil/iprepo/sha512/
} else {
   puts "No valid command specified. Use vivado_hls -f make.tcl <synthesis|services> ."
}

exit