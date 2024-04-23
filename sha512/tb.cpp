
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sha512.hpp"

int main(void) {
	hls::stream<AXIS_DATA  > inStream_t;
	hls::stream<ap_uint<1>  > verify_stream;
	hls::stream<AXIS_DATA  > outStream_t;
	hls::stream<AXIS_DATA  > outStream_t2;

	printf("START\n\n");
	AXIS_DATA dataStream_t;


	  dataStream_t.tdata = "0x41414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141";
	  dataStream_t.tkeep=-1;
	  dataStream_t.tlast=0;
	  inStream_t.write(dataStream_t);
	  outStream_t2.write(dataStream_t);

	  dataStream_t.tdata = "0x41414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141";
	  dataStream_t.tkeep = 11;
	  dataStream_t.tlast=0;
	  inStream_t.write(dataStream_t);
	  outStream_t2.write(dataStream_t);


	  dataStream_t.tdata = "0x41414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141";
	  dataStream_t.tkeep = 3;
	  dataStream_t.tlast=1;
	  inStream_t.write(dataStream_t);

	  std::cout<<"INPUT: data!"<<std::hex<<dataStream_t.tdata<<std::endl;

	  sha512(inStream_t, outStream_t);
	  outStream_t.read(dataStream_t);
	  std::cout<<" data: "<<std::hex<<dataStream_t.tdata<<" keep: "<<dataStream_t.tkeep<<" tlast: "<<dataStream_t.tlast<<std::endl;
	  outStream_t2.write(dataStream_t);
	  ap_uint<1> dataStream_t2;
	  sha512_verify(outStream_t2,verify_stream);
	  	do {
	  //		outStream_t.read(dataStream_t);
	  		verify_stream.read(dataStream_t2);
	  		std::cout<<" \n data: "<<std::hex<<dataStream_t2<<std::endl;
	  	}while(!verify_stream.empty());
	  	std::cout<<"\nEND\n";

//	  	do {
//	  //		outStream_t.read(dataStream_t);
//	  		outStream_t.read(dataStream_t);
//	  		std::cout<<" data: "<<std::hex<<dataStream_t.tdata<<" keep: "<<dataStream_t.tkeep<<" tlast: "<<dataStream_t.tlast<<std::endl;
//	  	}while(!outStream_t.empty());
//	  	std::cout<<"\nEND\n";

	return 0;
}

/*
int main(void) {                  // used to generate software hashes for validation

	hls::stream<AXIS_DATA  > inStream_t;
	hls::stream<AXIS_DATA  > outStream_t;

	printf("START\n\n");
	AXIS_DATA dataStream_t;


	  dataStream_t.tdata = "0x42414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141";
	  dataStream_t.tkeep=-1;
	  dataStream_t.tlast=0;
	  inStream_t.write(dataStream_t);dataStream_t.tdata = "0x41414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141";

	  dataStream_t.tlast=1;
	  inStream_t.write(dataStream_t);
		  std::cout<<"INPUT: data!"<<dataStream_t.tdata<<std::endl;

	sha512(inStream_t,outStream_t);

	do {
		outStream_t.read(dataStream_t);
		std::cout<<" data: "<<std::hex<<dataStream_t.tdata<<" keep: "<<dataStream_t.tkeep<<" tlast: "<<dataStream_t.tlast<<std::endl;
	}while(!outStream_t.empty());
	std::cout<<"\nEND\n";

	return 0;
}*/
