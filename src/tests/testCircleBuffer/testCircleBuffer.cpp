// testCircleBuffer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CircularBuffer.h"

#include <iostream>
using namespace std;

int __cdecl _tmain(int argc, _TCHAR* argv[])
{

	int x;
	cin>>x;
	const int CIRC_BUFF_SIZE = 4000;
	const int LOC_BUFF_SIZE = 300;
	
	const int LOC_READ_BUFF_BIG_SIZE = 1000;
	const int LOC_READ_BUFF_SMALL_SIZE = 50;
	const int TIMES_TO_LOOP = 300;
	CircularBuffer* locCircBuf = new CircularBuffer(CIRC_BUFF_SIZE);

	unsigned char* locBuf = new unsigned char[LOC_BUFF_SIZE];


	cout<<"* Testing fill to limit"<<endl;
	unsigned long locNumWritten = 0;
	for (int i = 0; i < (CIRC_BUFF_SIZE/LOC_BUFF_SIZE) + 2; i++) {
		locNumWritten = locCircBuf->write(locBuf, LOC_BUFF_SIZE);
		if (locNumWritten == 0) {
			cout<<"Buffer full... Wrote 0"<<endl;
		} else {
			cout<<"Wrote "<<locNumWritten<<" bytes to buffer"<<endl;
		}

		cout<< "Space Left : "<<locCircBuf->spaceLeft()<<"   ---   Avail : "<<locCircBuf->numBytesAvail()<<endl;
	}

	locCircBuf->reset();

	cout<<"* Testing rolling alternate read write"<<endl;
	
	unsigned char* locReadBuf = new unsigned char[LOC_READ_BUFF_BIG_SIZE];
	unsigned long locNumRead = 0;
	locNumWritten = 0;
	//Assert buffersize > LOC_BUFF_SIZE
	//Assert buffersize > LOC_READ_BUFF_SMALL_SIZE
	//Assert buffersize > LOC_READ_BUFF_BIG_SIZE

	//buffersize = 0;
	for (int i = 0; i < TIMES_TO_LOOP; i++) {
		
		cout<<"Round : "<<i<<endl;
		locNumWritten = locCircBuf->write(locBuf, LOC_BUFF_SIZE);
		cout<<"Write attempted "<<LOC_BUFF_SIZE<<" got "<<locNumWritten<<endl;
		cout<<"Avail : "<<locCircBuf->numBytesAvail()<<"  --  Space Left : "<<locCircBuf->spaceLeft()<<endl<<endl;
		//buffersize = LOC_BUFF_SIZE

		cout<<"** Short read test"<<endl;
		locNumRead = locCircBuf->read(locReadBuf, LOC_READ_BUFF_SMALL_SIZE);
		cout<<"Read attempted "<<LOC_READ_BUFF_SMALL_SIZE<<" got "<<locNumRead<<endl;
		cout<<"Avail : "<<locCircBuf->numBytesAvail()<<"  --  Space Left : "<<locCircBuf->spaceLeft()<<endl<<endl;
		//buffersize = LOC_BUFF_SIZE - LOC_READ_BUFF_SMALL_SIZE

		locNumWritten = locCircBuf->write(locBuf, LOC_BUFF_SIZE);
		cout<<"Write attempted "<<LOC_BUFF_SIZE<<" got "<<locNumWritten<<endl;
		cout<<"Avail : "<<locCircBuf->numBytesAvail()<<"  --  Space Left : "<<locCircBuf->spaceLeft()<<endl<<endl;
		//buffersize = (2 * LOC_BUFF_SIZE) - LOC_READ_BUFF_SMALL_SIZE

		cout<<"** Long read / Empty Buffer test"<<endl;
		locNumRead = locCircBuf->read(locReadBuf, LOC_READ_BUFF_BIG_SIZE);
		cout<<"Read attempted "<<LOC_READ_BUFF_BIG_SIZE<<" got "<<locNumRead<<endl;
		cout<<"Avail : "<<locCircBuf->numBytesAvail()<<"  --  Space Left : "<<locCircBuf->spaceLeft()<<endl<<endl;
		//buffersize = (2 * LOC_BUFF_SIZE) - LOC_READ_BUFF_SMALL_SIZE - LOC_READ_BUFF_BIG_SIZE

		cout<<"** Read while empty test"<<endl;
		locNumRead = locCircBuf->read(locReadBuf, LOC_READ_BUFF_BIG_SIZE);
		cout<<"Read attempted "<<LOC_READ_BUFF_BIG_SIZE<<" got "<<locNumRead<<endl;
		cout<<"Avail : "<<locCircBuf->numBytesAvail()<<"  --  Space Left : "<<locCircBuf->spaceLeft()<<endl<<endl;
		//buffersize = (2 * LOC_BUFF_SIZE) - LOC_READ_BUFF_SMALL_SIZE - (2 * LOC_READ_BUFF_BIG_SIZE)

	}

	delete locCircBuf;
	delete locBuf;
	delete locReadBuf;
	return 0;
}

