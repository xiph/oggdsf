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
	const int LOC_BUFF_SIZE = 421;
	const int LOC_READ_BUFF_BIG_SIZE = 811;
	const int LOC_READ_BUFF_SMALL_SIZE = 379;
	const int TIMES_TO_LOOP = 300;
	CircularBuffer* locCircBuf = new CircularBuffer(CIRC_BUFF_SIZE);

	unsigned char* locBuf = new unsigned char[LOC_BUFF_SIZE];


	cout<<"* Testing fill to limit"<<endl;
	unsigned long locNumWritten = 0;
	for (int i = 0; i < 12; i++) {
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

	for (int i = 0; i < TIMES_TO_LOOP; i++) {
		locNumWritten = locCircBuf->write(locBuf, LOC_BUFF_SIZE);
		cout<<"Write attempted "<<LOC_BUFF_SIZE<<" got "<<locNumWritten<<endl;

		cout<<"** Short read test"<<endl;
		locNumRead = locCircBuf->read(locReadBuf, LOC_READ_BUFF_SMALL_SIZE);
		cout<<"Read attempted "<<LOC_READ_BUFF_SMALL_SIZE<<" got "<<locNumRead<<endl;

		locNumWritten = locCircBuf->write(locBuf, LOC_BUFF_SIZE);
		cout<<"Write attempted "<<LOC_BUFF_SIZE<<" got "<<locNumWritten<<endl;

		cout<<"** Long read test"<<endl;
		locNumRead = locCircBuf->read(locReadBuf, LOC_READ_BUFF_BIG_SIZE);
		cout<<"Read attempted "<<LOC_READ_BUFF_BIG_SIZE<<" got "<<locNumRead<<endl;

		cout<<"** Long read again to empty test"<<endl;
		locNumRead = locCircBuf->read(locReadBuf, LOC_READ_BUFF_BIG_SIZE);
		cout<<"Read attempted "<<LOC_READ_BUFF_BIG_SIZE<<" got "<<locNumRead<<endl;

	}
	return 0;
}

