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

//BOOL WINAPI DeviceIoControl(
//		HANDLE hDevice,
//		DWORD dwIoControlCode,
//		LPVOID lpInBuffer,
//		DWORD nInBufferSize,
//		LPVOID lpOutBuffer,
//		DWORD nOutBufferSize,
//		LPDWORD lpBytesReturned,
//		LPOVERLAPPED lpOverlapped
//);

#define MAXIMUM_NUMBER_TRACKS  100
#define IOCTL_CDROM_READ_TOC 0x00024000
#define IOCTL_GET_CONTROL	0x00024034
#define IOCTL_CDROM_GET_DRIVE_GEOMETRY  0x0002404C
#define IOCTL_CDROM_RAW_READ       0x0002403E

//typedef struct _DISK_GEOMETRY {
//  LARGE_INTEGER  Cylinders;
//  MEDIA_TYPE  MediaType;
//  ULONG  TracksPerCylinder;
//  ULONG  SectorsPerTrack;
//  ULONG  BytesPerSector;
//} DISK_GEOMETRY, *PDISK_GEOMETRY;

typedef struct _TRACK_DATA {
  UCHAR  Reserved;
  UCHAR  Control : 4;
  UCHAR  Adr : 4;
  UCHAR  TrackNumber;
  UCHAR  Reserved1;
  UCHAR  Address[4];
} TRACK_DATA, *PTRACK_DATA;

typedef struct _CDROM_TOC {
		UCHAR  Length[2];
		UCHAR  FirstTrack;
		UCHAR  LastTrack;
		TRACK_DATA  TrackData[MAXIMUM_NUMBER_TRACKS];
} CDROM_TOC, *PCDROM_TOC;

