open_project rsa_proj

open_solution "solution1"
set_part xcu280-fsvh2892-2L-e
create_clock -period 4 -name default

set_top rsa

add_files ./rsa/rsa.cpp
add_files ./rsa/rsa.hpp


#Check which command
set command [lindex $argv 2]

if {$command == "synthesis"} {
   csynth_designa
} elseif {$command == "services"} {
  file mkdir --parent ../hw/secure-kamil/iprepo
  file delete -force ../hw/secure-kamil/iprepo/rsa
  file copy -force ./rsa_proj/solution1/impl/verilog ../hw/secure-kamil/iprepo/rsa/
} else {
   puts "No valid command specified. Use vivado_hls -f make.tcl <synthesis|services> ."
}

exit