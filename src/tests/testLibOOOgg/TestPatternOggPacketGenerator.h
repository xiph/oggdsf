#pragma once

class TestPatternOggPacketGenerator
{
public:
	TestPatternOggPacketGenerator(void);
	~TestPatternOggPacketGenerator(void);

	//Test pattern will be...
	//
	//	50 byte packet

	//Packet
	//======
	//All packets start with [[[[
	//All packets end with ]]]]
	//Will rotate through lowercase alphabetical letters.
	//Packets shorter than 8 bytes are filled with 012345... etc.

	//eg. [[[[abcdefghij...abcde...]]]]

	StampedOggPacket* generatePacket(unsigned long inPacketSize, __int64 inGranulePos);

protected:
	

};
