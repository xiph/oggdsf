#include "StdAfx.h"
#include "cdrom.h"

CDROM::CDROM(void)
	:	mDriveHandle(INVALID_HANDLE_VALUE)
{
}

CDROM::~CDROM(void)
{
}

//IOCTL_CDROM_GET_DRIVE_GEOMETRY

//DISK_GEOMETRY CDROM::getDiskGeom() {
//	if (mDriveHandle == INVALID_HANDLE_VALUE) {
//		return NO_DRIVE_HANDLE;
//	}
//
//	DWORD locBytesRead = 0;
//	DISK_GEOMETRY locGeom;
//	BOOL locRet = DeviceIoControl(	mDriveHandle,
//									IOCTL_CDROM_GET_DRIVE_GEOMETRY,
//									NULL,
//									0,
//									&locGeom,
//									sizeof(locGeom),
//									&locBytesRead,
//									NULL);
//
//	if (locRet == FALSE) {
//		CloseHandle(mDriveHandle);
//		mDriveHandle = INVALID_HANDLE_VALUE;
//		return READ_TOC_FAILED;
//	}
//
//}
int CDROM::ejectDraw() {
	if (mDriveHandle == INVALID_HANDLE_VALUE) {
		return NO_DRIVE_HANDLE;
	}

	DWORD locBytesRead = 0;
	BOOL locRet = DeviceIoControl(	mDriveHandle,
									IOCTL_STORAGE_EJECT_MEDIA,
									NULL,
									0,
									NULL,
									0,
									&locBytesRead,
									NULL);

	if (locRet == FALSE) {
		CloseHandle(mDriveHandle);
		mDriveHandle = INVALID_HANDLE_VALUE;
		return READ_TOC_FAILED;
	}
	return 0;
}
int CDROM::closeDraw() {
	return 0;
}


int CDROM::initDrive(string inDrive) {
	//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/base/createfile.asp

	//HANDLE CreateFile(
	//	LPCTSTR lpFileName,
	//	DWORD dwDesiredAccess,
	//	DWORD dwShareMode,
	//	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	//	DWORD dwCreationDisposition,
	//	DWORD dwFlagsAndAttributes,
	//	HANDLE hTemplateFile
	//);

	if ((inDrive.length() != 2) || (inDrive[1] != ':')) {
		return INVALID_DRIVE_STRING;
	}

	if (mDriveHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(mDriveHandle);
		mDriveHandle = INVALID_HANDLE_VALUE;
	}

	string locDeviceString = "\\\\.\\"+inDrive;

	mDriveHandle = CreateFile(	locDeviceString.c_str(),
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_READONLY,
								NULL);

	if (mDriveHandle == INVALID_HANDLE_VALUE) {
		return CREATE_HANDLE_FAILED;
	}

	
	return 0;
}

int CDROM::readTOC() {

	//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/devio/base/deviceiocontrol.asp

	//BOOL DeviceIoControl(
	//	HANDLE hDevice,
	//	DWORD dwIoControlCode,
	//	LPVOID lpInBuffer,
	//	DWORD nInBufferSize,
	//	LPVOID lpOutBuffer,
	//	DWORD nOutBufferSize,
	//	LPDWORD lpBytesReturned,
	//	LPOVERLAPPED lpOverlapped
	//);

	//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/storage/hh/storage/k307_8e0f2b70-edd0-424e-abb4-a81ea9f382fe.xml.asp

	//typedef struct _CDROM_TOC {
	//	UCHAR  Length[2];
	//	UCHAR  FirstTrack;
	//	UCHAR  LastTrack;
	//	TRACK_DATA  TrackData[MAXIMUM_NUMBER_TRACKS];
	//} CDROM_TOC, *PCDROM_TOC;

	if (mDriveHandle == INVALID_HANDLE_VALUE) {
		return NO_DRIVE_HANDLE;
	}

	DWORD locBytesRead = 0;
	BOOL locRet = DeviceIoControl(	mDriveHandle,
									IOCTL_CDROM_READ_TOC,
									NULL,
									0,
									&mTOC,
									sizeof(CDROM_TOC),
									&locBytesRead,
									NULL);

	if (locRet == FALSE) {
		CloseHandle(mDriveHandle);
		mDriveHandle = INVALID_HANDLE_VALUE;
		return READ_TOC_FAILED;
	}

	return 0;
									

}