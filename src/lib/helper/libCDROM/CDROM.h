#pragma once

#include <string>
using namespace std;
//#include <ntddcdrm.h>
#include "cdwin32api.h"
#include <winioctl.h>
class LIBCDROM_API CDROM
{
public:
	CDROM(void);
	~CDROM(void);

	enum eInitErrors {
		INVALID_DRIVE_STRING = 1,
		CREATE_HANDLE_FAILED = 2
	};

	enum eTOCErrors {
		NO_DRIVE_HANDLE = 1,
		READ_TOC_FAILED = 2

	};
	int initDrive(string inDrive);
	
	int readTOC();
	CDROM_TOC* getTOC();

	DISK_GEOMETRY* getDiskGeom();

	int ejectDraw();
	int closeDraw();


protected:
	HANDLE mDriveHandle;
	CDROM_TOC mTOC;
	DISK_GEOMETRY mGeom;
};
