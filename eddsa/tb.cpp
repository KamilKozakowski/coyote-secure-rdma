#include "eddsa.hpp"


int main() {

	hls::stream<AXIS_DATA  > inStream_t;
	hls::stream<AXIS_DATA  > inStream_t2;
	hls::stream<AXIS_DATA  > inStream_t3;
	hls::stream<AXIS_DATA  > outStream_t;
	hls::stream<AXIS_DATA  > outStream_t2;

	printf("START\n\n");
	AXIS_DATA dataStream_t;


	  dataStream_t.tdata = "0x41414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141";
	  dataStream_t.tkeep=-1;
	  dataStream_t.tlast=0;
	  inStream_t.write(dataStream_t);
	  inStream_t2.write(dataStream_t);
	  inStream_t.write(dataStream_t);
	  inStream_t2.write(dataStream_t);
	  inStream_t.write(dataStream_t);
	  inStream_t2.write(dataStream_t);

	  dataStream_t.tdata = "0x41414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141";
	  dataStream_t.tkeep = 11;
	  dataStream_t.tlast=1;
	  inStream_t.write(dataStream_t);
//	  inStream_t2.write(dataStream_t);


	  std::cout<<"INPUT: data!"<<std::hex<<dataStream_t.tdata<<std::endl;

	  sha512(inStream_t, outStream_t);
	  dataStream_t = outStream_t.read();

	  std::cout<<"SHA output"<<std::hex<<dataStream_t.tdata<<std::endl;
	  inStream_t3.write(dataStream_t);
	  eddsa(inStream_t3, outStream_t2);
	  dataStream_t = outStream_t2.read();
	  std::cout<<"EDDSA: data!"<<std::hex<<dataStream_t.tdata<<std::endl;
	  inStream_t2.write(dataStream_t);
//

	  hls::stream<ap_uint<1>  > resultStream;
	  sha512_eddsa_verify(inStream_t2,resultStream);
	  ap_uint<1> res = resultStream.read();
	  std::cout<<"RES "<<res<<std::endl;
	  if(res == 0){
		  std::cout<<"WIN:: 0"<<std::endl;
	  }else {

		  std::cout<<"LOSE:: 1"<<std::endl;
	  }
    return 0;
}
