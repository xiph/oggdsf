# Microsoft Developer Studio Project File - Name="ogg_dynamic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ogg_dynamic - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ogg_dynamic.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ogg_dynamic.mak" CFG="ogg_dynamic - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ogg_dynamic - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogg_dynamic - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogg_dynamic - Win32 MakeFile" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ogg_dynamic - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Dynamic_Debug"
# PROP BASE Intermediate_Dir ".\Dynamic_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Dynamic_Debug"
# PROP Intermediate_Dir ".\Dynamic_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /I "..\include" /ZI /W3 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_MBCS" /Fp".\Dynamic_Debug/ogg_dynamic.pch" /Fo".\Dynamic_Debug/" /Fd".\Dynamic_Debug/" /FR /GZ /c /GX 
# ADD CPP /nologo /MDd /I "..\include" /ZI /W3 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_MBCS" /Fp".\Dynamic_Debug/ogg_dynamic.pch" /Fo".\Dynamic_Debug/" /Fd".\Dynamic_Debug/" /FR /GZ /c /GX 
# ADD BASE MTL /nologo /D"_DEBUG" /mktyplib203 /tlb".\Dynamic_Debug\ogg_dynamic.tlb" /win32 
# ADD MTL /nologo /D"_DEBUG" /mktyplib203 /tlb".\Dynamic_Debug\ogg_dynamic.tlb" /win32 
# ADD BASE RSC /l 1033 /d "_DEBUG" 
# ADD RSC /l 1033 /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /out:"Dynamic_Debug\ogg_d.dll" /incremental:no /def:".\ogg.def" /debug /pdb:".\Dynamic_Debug\ogg_d.pdb" /pdbtype:sept /subsystem:windows /implib:".\Dynamic_Debug/ogg_d.lib" /machine:ix86 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /out:"Dynamic_Debug\ogg_d.dll" /incremental:no /def:".\ogg.def" /debug /pdb:".\Dynamic_Debug\ogg_d.pdb" /pdbtype:sept /subsystem:windows /implib:".\Dynamic_Debug/ogg_d.lib" /machine:ix86 

!ELSEIF  "$(CFG)" == "ogg_dynamic - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Dynamic_Release"
# PROP BASE Intermediate_Dir ".\Dynamic_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Dynamic_Release"
# PROP Intermediate_Dir ".\Dynamic_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /I "..\include" /W3 /O2 /Ob1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_MBCS" /GF /Gy /YX /Fp".\Dynamic_Release/ogg_dynamic.pch" /Fo".\Dynamic_Release/" /Fd".\Dynamic_Release/" /c /GX 
# ADD CPP /nologo /MD /I "..\include" /W3 /O2 /Ob1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_MBCS" /GF /Gy /YX /Fp".\Dynamic_Release/ogg_dynamic.pch" /Fo".\Dynamic_Release/" /Fd".\Dynamic_Release/" /c /GX 
# ADD BASE MTL /nologo /D"NDEBUG" /mktyplib203 /tlb".\Dynamic_Release\ogg_dynamic.tlb" /win32 
# ADD MTL /nologo /D"NDEBUG" /mktyplib203 /tlb".\Dynamic_Release\ogg_dynamic.tlb" /win32 
# ADD BASE RSC /l 1033 /d "NDEBUG" 
# ADD RSC /l 1033 /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /out:"Dynamic_Release\ogg.dll" /incremental:no /def:".\ogg.def" /pdb:".\Dynamic_Release\ogg.pdb" /pdbtype:sept /subsystem:windows /implib:".\Dynamic_Release/ogg.lib" /machine:ix86 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /out:"Dynamic_Release\ogg.dll" /incremental:no /def:".\ogg.def" /pdb:".\Dynamic_Release\ogg.pdb" /pdbtype:sept /subsystem:windows /implib:".\Dynamic_Release/ogg.lib" /machine:ix86 

!ELSEIF  "$(CFG)" == "ogg_dynamic - Win32 MakeFile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "$(ConfigurationName)"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "$(ConfigurationName)"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /c /GX 
# ADD CPP /nologo /c /GX 
# ADD BASE MTL /nologo /win32 
# ADD MTL /nologo /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /pdbtype:sept /subsystem:windows 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /pdbtype:sept /subsystem:windows 

!ENDIF

# Begin Target

# Name "ogg_dynamic - Win32 Debug"
# Name "ogg_dynamic - Win32 Release"
# Name "ogg_dynamic - Win32 MakeFile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\bitwise.c

!IF  "$(CFG)" == "ogg_dynamic - Win32 Debug"

# ADD CPP /nologo /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR /GZ /GX 
!ELSEIF  "$(CFG)" == "ogg_dynamic - Win32 Release"

# ADD CPP /nologo /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /GX 
!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\framing.c

!IF  "$(CFG)" == "ogg_dynamic - Win32 Debug"

# ADD CPP /nologo /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR /GZ /GX 
!ELSEIF  "$(CFG)" == "ogg_dynamic - Win32 Release"

# ADD CPP /nologo /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /GX 
!ENDIF

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\ogg\ogg.h
# End Source File
# Begin Source File

SOURCE=..\include\ogg\os_types.h
# End Source File
# End Group
# Begin Group "Other Files"

# PROP Default_Filter ".def"
# Begin Source File

SOURCE=ogg.def
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

