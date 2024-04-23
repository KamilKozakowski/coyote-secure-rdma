#include <iostream>

#include "rsa.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(void) {
	SHA256_CTX ctx;                   // used to generate software hashes for validation

	hls::stream<AXIS_DATA  > inStream_t;
	hls::stream<AXIS_DATA  > inStream_t2;
	hls::stream<AXIS_DATA  > outStream_t;

	printf("START\n\n");
	AXIS_DATA dataStream_t;


	  dataStream_t.tdata = "0x41414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141";
	  dataStream_t.tkeep=-1;
	  dataStream_t.tlast=0;
	  inStream_t.write(dataStream_t);
	  inStream_t2.write(dataStream_t);

	  dataStream_t.tdata = "0x41414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141";
	  dataStream_t.tkeep = 11;
	  dataStream_t.tlast=0;
	  inStream_t.write(dataStream_t);
	  inStream_t2.write(dataStream_t);


	  dataStream_t.tdata = "0x41414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141";
	  dataStream_t.tkeep = 3;
	  dataStream_t.tlast=1;
	  inStream_t.write(dataStream_t);

	  std::cout<<"INPUT: data!"<<std::hex<<dataStream_t.tdata<<std::endl;

	  rsa(inStream_t, outStream_t);

	  outStream_t.read(dataStream_t);
	  inStream_t2.write(dataStream_t);

	  hls::stream<ap_uint<1>  > o;
	  rsa_decrypt(inStream_t2, o);
	  ap_uint<1> r;
	  o.read(r);
	  std::cout<<" "<<r<<std::endl;
//	  	do {
//	  //		outStream_t.read(dataStream_t);
//	  		outStream_t.read(dataStream_t);
//	  		std::cout<<" data: "<<std::hex<<dataStream_t.tdata<<" keep: "<<dataStream_t.tkeep<<" tlast: "<<dataStream_t.tlast<<std::endl;
//	  	}while(!outStream_t.empty());
//	  	std::cout<<"\nEND\n";
	return 0;
}
