// testTemporal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "libTemporalURI/C_TimeStamp.h"

int __cdecl _tmain(int argc, _TCHAR* argv[])
{

	C_TimeStamp locStamp;

	string locStr;

	int x;
	cin>>x;

	do {
		cout<<" : ";
		cin>>locStr;

		if (locStr != "x") {
			if ( locStamp.parseTimeStamp(locStr) ) {
				cout<<"Time = "<<locStamp.toHunNanos()<<endl;
			} else {
				cout<<"Invalid"<<endl;
			}
		}



	} while (locStr != "x");
	return 0;
}

