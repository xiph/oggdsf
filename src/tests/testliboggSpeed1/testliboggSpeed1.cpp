// testliboggSpeed1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ogg\ogg.h"
#include <fstream>
#include <iostream>

using namespace std;
int __cdecl _tmain(int argc, _TCHAR* argv[])
{
		LARGE_INTEGER perfStart;
		LARGE_INTEGER perfEnd;
		QueryPerformanceCounter(&perfStart);
  int pageCount = 0;
  int packetCount = 0;
  const unsigned short BUFF_SIZE = 8092;
  ogg_sync_state   syncState; /* sync and verify incoming physical bitstream */
 
  ogg_page         oggPage; /* one Ogg bitstream page.  Vorbis packets are inside */
  ogg_packet       oggPacket; /* one raw packet of data for decode */ 
  ogg_stream_state streamState; 
  char* buffer;
  int bytes;



  ogg_sync_init( &syncState );

 

  fstream testFile;
  testFile.open(argv[1], ios_base::in | ios_base::binary);

   buffer = ogg_sync_buffer(&syncState, BUFF_SIZE);
   testFile.read(buffer, BUFF_SIZE);
			
   ogg_sync_wrote(&syncState, testFile.gcount());
   ogg_sync_pageout(&syncState, &oggPage);
   ogg_stream_init(&streamState,ogg_page_serialno(&oggPage)); 

  	while (!testFile.eof()) {
		 buffer = ogg_sync_buffer(&syncState, BUFF_SIZE);
			testFile.read(buffer, BUFF_SIZE);
			
    		ogg_sync_wrote(&syncState, testFile.gcount());
			while (	ogg_sync_pageout(&syncState, &oggPage) > 0) {
				ogg_stream_pagein(&streamState, &oggPage);
				while (ogg_stream_packetout(&streamState, &oggPacket) > 0) {
					packetCount++;
				}
				pageCount++;

			}
	}
	QueryPerformanceCounter(&perfEnd);
    cout<<"Packet Count = "<<packetCount<<endl;
	cout<<"Page count = "<<pageCount<<endl;
	cout<<perfStart.QuadPart<<" - "<<perfEnd.QuadPart<<endl;
	cout<<"Time = "<<perfEnd.QuadPart - perfStart.QuadPart<<endl;

	cout<<"PageCount = "<<pageCount<<endl;
	return 0;
}

