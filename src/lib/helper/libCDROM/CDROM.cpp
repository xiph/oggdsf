//===========================================================================
//Copyright (C) 2004 Zentaro Kavanagh
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
//  may be used to endorse or promote products derived from this software 
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================

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

DISK_GEOMETRY* CDROM::getDiskGeom() {
	
	if (mDriveHandle == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	DWORD locBytesRead = 0;
	
	BOOL locRet = DeviceIoControl(	mDriveHandle,
									IOCTL_CDROM_GET_DRIVE_GEOMETRY,
									NULL,
									0,
									&mGeom,
									sizeof(mGeom),
									&locBytesRead,
									NULL);

	if (locRet == FALSE) {
		CloseHandle(mDriveHandle);
		mDriveHandle = INVALID_HANDLE_VALUE;
		return NULL;
	}

	return &mGeom;

}
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

CDROM_TOC* CDROM::getTOC() {
	return &mTOC;


}