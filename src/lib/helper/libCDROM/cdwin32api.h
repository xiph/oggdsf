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

