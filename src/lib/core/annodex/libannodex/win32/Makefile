# Set the value of $(MSVCDir) for your installation. 
# Please note that none of the directory names are allowed to have spaces 
# in them. You must use the short name 
#
# "C:\Program Files\Microsoft Visual Studio\VC98" will not work.
# "C:\Progra~1\Micros~1\VC98" will work.
MSVCDir="C:\PROGRA~1\MICROS~1.NET\Vc7"

# MSVCDir="C:\PROGRA~1\Micros~4\VC98"

CPP=cl.exe
LINK32=link.exe

# libannodex
LINK32_FLAGS_ANX=ogg.lib liboggz.lib /nologo /dll /incremental:no /libpath:"$(MSVCDir)\Lib" /implib:".\libannodex.lib" /machine:I386 /out:".\libannodex.dll" 

CFLAGS_ANX=/nologo /MD /W1 /GX /O2 /I"." /I"..\src\libannodex" /I"..\include" /I"$(MSVCDir)\Include"  /D "WIN32" /D "_USRDLL" /YX /FD

# importers
LINK32_FLAGS_I_CMML=libannodex.lib libexpat.lib libcmml.lib /nologo /dll /incremental:no /libpath:"$(MSVCDir)\Lib" /implib:".\libanx_import_cmml.lib" /machine:I386 /out:".\libanx_import_cmml.dll" 

LINK32_FLAGS_I_ANX=libannodex.lib /nologo /dll /incremental:no /libpath:"$(MSVCDir)\Lib" /implib:".\libanx_import_anx.lib" /machine:I386 /out:".\libanx_import_anx.dll" 

LINK32_FLAGS_I_OGG=libannodex.lib liboggz.lib /nologo /dll /incremental:no /libpath:"$(MSVCDir)\Lib" /implib:".\libanx_import_ogg.lib" /machine:I386 /out:".\libanx_import_ogg.dll" 

LINK32_FLAGS_I_SNDFILE=libannodex.lib libsndfile.lib /nologo /dll /incremental:no /libpath:"$(MSVCDir)\Lib" /implib:".\libanx_import_sndfile.lib" /machine:I386 /out:".\libanx_import_sndfile.dll" 

CFLAGS_I=/nologo /MD /W1 /GX /O2 /I"." /I"..\src\importers" /I"..\include" /I"$(MSVCDir)\Include"  /D "WIN32" /D "_USRDLL" /YX /FD

# executables
PROG_LINK_FLAGS=/nologo /incremental:no /libpath:"$(MSVCDir)\Lib" /machine:I386 /exetype:dynamic


#====================================================================
# Targets

ALL :  "libannodex.dll" \
	"libanx_import_cmml.dll" "libanx_import_anx.dll" \
	"libanx_import_ogg.dll" "libanx_import_sndfile.dll" \
	"anxrip.exe" "anxcheck.exe" "anxinfo.exe" \
	"anxenc.exe" "anxed.exe"

CLEAN:
	-@erase ".\*.obj"
	-@erase ".\vc60.idb"
	-@erase ".\vc60.pch"
	-@erase ".\*.plg"
	-@erase ".\*.opt"
	-@erase ".\liban*.dll"
	-@erase ".\liban*.exp"
	-@erase ".\liban*.lib"

#====================================================================

LINK32_OBJS= \
	".\anx_compat.obj" \
	".\dirent.obj" \
	".\xtag.obj" \
	".\fix_dl.obj" \
	".\anx_list.obj" \
	".\anx_params.obj" \
	".\anx_tree.obj" \
	".\annodex.obj" \
	".\anx_track.obj" \
	".\anx_snprint.obj" \
	".\anx_command.obj" \
	".\anx_element.obj" \
	".\anx_import.obj" \
	".\anx_time.obj" \
	".\anx_read.obj" \
	".\anx_write.obj" \
	".\content_type.obj"

"libannodex.dll" : $(LINK32_OBJS) ".\libannodex.def"
    $(LINK32) $(LINK32_FLAGS_ANX) /def:".\libannodex.def" $(LINK32_OBJS)


"libanx_import_cmml.dll" : ".\anx_import_cmml.obj" ".\importers.def"
    $(LINK32) $(LINK32_FLAGS_I_CMML) /def:".\importers.def" ".\anx_import_cmml.obj"

"libanx_import_anx.dll" : ".\anx_import_anx.obj" ".\importers.def"
    $(LINK32) $(LINK32_FLAGS_I_ANX) /def:".\importers.def" ".\anx_import_anx.obj"

"libanx_import_ogg.dll" : ".\anx_import_ogg.obj" ".\importers.def"
    $(LINK32) $(LINK32_FLAGS_I_OGG) /def:".\importers.def" ".\anx_import_ogg.obj"

"libanx_import_sndfile.dll" : ".\anx_import_sndfile.obj" ".\importers.def"
    $(LINK32) $(LINK32_FLAGS_I_SNDFILE) /def:".\importers.def" ".\anx_import_sndfile.obj"


"anxrip.exe" :  ".\common.obj" "..\src\tools\anxrip.c"
        $(CPP) $(CFLAGS_ANX) /Fo".\anxrip.obj" /c "..\src\tools\anxrip.c"
        $(LINK32) $(PROG_LINK_FLAGS) /out:"anxrip.exe" ".\anxrip.obj" ".\common.obj" libannodex.lib liboggz.lib

"anxcheck.exe" :  ".\common.obj" "..\src\tools\anxcheck.c"
        $(CPP) $(CFLAGS_ANX) /Fo".\anxcheck.obj" /c "..\src\tools\anxcheck.c"
        $(LINK32) $(PROG_LINK_FLAGS) /out:"anxcheck.exe" ".\anxcheck.obj" ".\common.obj" libannodex.lib liboggz.lib

"anxinfo.exe" :  ".\common.obj" "..\src\tools\anxinfo.c"
        $(CPP) $(CFLAGS_ANX) /Fo".\anxinfo.obj" /c "..\src\tools\anxinfo.c"
        $(LINK32) $(PROG_LINK_FLAGS) /out:"anxinfo.exe" ".\anxinfo.obj" ".\common.obj" libannodex.lib liboggz.lib

"anxenc.exe" :  ".\common.obj" "..\src\tools\anxenc.c"
        $(CPP) $(CFLAGS_ANX) /Fo".\anxenc.obj" /c "..\src\tools\anxenc.c"
        $(LINK32) $(PROG_LINK_FLAGS) /out:"anxenc.exe" ".\anxenc.obj" ".\common.obj" libannodex.lib liboggz.lib

"anxed.exe" :  ".\common.obj" "..\src\tools\anxed.c"
        $(CPP) $(CFLAGS_ANX) /Fo".\anxed.obj" /c "..\src\tools\anxed.c"
        $(LINK32) $(PROG_LINK_FLAGS) /out:"anxed.exe" ".\anxed.obj" ".\common.obj" libannodex.lib liboggz.lib


#====================================================================
# C files in src

.\anx_import_cmml.obj:
	$(CPP) $(CFLAGS_I) /Fo".\anx_import_cmml.obj" /c "..\src\importers\anx_import_cmml.c"

.\anx_import_anx.obj:
	$(CPP) $(CFLAGS_I) /Fo".\anx_import_anx.obj" /c "..\src\importers\anx_import_anx.c"

.\anx_import_ogg.obj:
	$(CPP) $(CFLAGS_I) /Fo".\anx_import_ogg.obj" /c "..\src\importers\anx_import_ogg.c"

.\anx_import_sndfile.obj:
	$(CPP) $(CFLAGS_I) /Fo".\anx_import_sndfile.obj" /c "..\src\importers\anx_import_sndfile.c"

.\anx_compat.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\anx_compat.obj" /c "..\src\libannodex\anx_compat.c"

.\dirent.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\dirent.obj" /c ".\dirent.c"

.\common.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\common.obj" /c "..\src\tools\common.c"

.\xtag.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\xtag.obj" /c "..\src\libannodex\xtag.c"

.\fix_dl.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\fix_dl.obj" /c "..\src\libannodex\fix_dl.c"

.\anx_list.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\anx_list.obj" /c "..\src\libannodex\anx_list.c"

.\anx_params.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\anx_params.obj" /c "..\src\libannodex\anx_params.c"

.\anx_tree.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\anx_tree.obj" /c "..\src\libannodex\anx_tree.c"

.\annodex.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\annodex.obj" /c "..\src\libannodex\annodex.c"

.\anx_track.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\anx_track.obj" /c "..\src\libannodex\anx_track.c"

.\anx_snprint.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\anx_snprint.obj" /c "..\src\libannodex\anx_snprint.c"

.\anx_command.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\anx_command.obj" /c "..\src\libannodex\anx_command.c"

.\anx_element.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\anx_element.obj" /c "..\src\libannodex\anx_element.c"

.\anx_import.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\anx_import.obj" /c "..\src\libannodex\anx_import.c"

.\anx_time.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\anx_time.obj" /c "..\src\libannodex\anx_time.c"

.\anx_read.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\anx_read.obj" /c "..\src\libannodex\anx_read.c"

.\anx_write.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\anx_write.obj" /c "..\src\libannodex\anx_write.c"

.\content_type.obj:
	$(CPP) $(CFLAGS_ANX) /Fo".\content_type.obj" /c "..\src\libannodex\content_type.c"

# End of Makefile
#====================================================================

