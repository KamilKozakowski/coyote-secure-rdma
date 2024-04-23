open_project ../hw/build_kamil/lynx/lynx.xpr

add_files vivado_code/sha_eddsa_2w/abcd.sv
add_files vivado_code/sha_eddsa_2w/abcd2.sv
add_files vivado_code/check_sha.sv
add_files vivado_code/data_chk_multiplexer.sv
add_files vivado_code/inputFIFODuplicate.sv
add_files vivado_code/replace_last_packet_with_sha.sv

add_files ../hw/secure-kamil/iprepo/sha512
add_files ../hw/secure-kamil/iprepo/eddsa

exit
