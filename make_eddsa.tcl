open_project eddsa_prj

open_solution "solution1"
set_part xcu280-fsvh2892-2L-e
create_clock -period 4 -name default

set_top eddsa

add_files ./eddsa/eddsa.cpp
add_files ./eddsa/eddsa.hpp


#Check which command
set command [lindex $argv 2]

if {$command == "synthesis"} {
   csynth_designa
} elseif {$command == "services"} {
  file mkdir --parent ../hw/secure-kamil/iprepo
  file delete -force ../hw/secure-kamil/iprepo/eddsa
  file copy -force ./eddsa_prj/solution1/impl/verilog ../hw/secure-kamil/iprepo/eddsa/
} else {
   puts "No valid command specified. Use vivado_hls -f make.tcl <synthesis|services> ."
}

exit