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
LINK32_FLAGS=libexpat.lib /nologo /dll /incremental:no /libpath:"$(MSVCDir)\Lib" /implib:".\libcmml.lib" /machine:I386 /out:".\libcmml.dll" 
PROG_LINK_FLAGS=/nologo /incremental:no /libpath:"$(MSVCDir)\Lib" /machine:I386 /exetype:dynamic

CFLAGS=/nologo /MD /W1 /GX /O2 /I"." /I"..\src" /I"$(MSVCDir)\Include"  /D "WIN32" /D "_USRDLL" /YX /FD


#====================================================================
# Targets

ALL :  "attgetopt.obj" "libcmml.dll" "cmml-validate.exe" "cmml-fix.exe"
# "cmml-fortune.exe" - is there a "fortune" command under Windows?

CLEAN:
	-@erase ".\*.obj"
	-@erase ".\vc60.idb"
	-@erase ".\libcmml.dll"
	-@erase ".\libcmml.exp"
	-@erase ".\libcmml.lib"

#====================================================================

LINK32_OBJS= \
	".\cmml.obj" \
	".\cmml_element.obj" \
	".\cmml_list.obj" \
	".\cmml_private.obj" \
	".\cmml_time.obj"

"libcmml.dll" : $(LINK32_OBJS) ".\libcmml.def"
    $(LINK32) $(LINK32_FLAGS) /def:".\libcmml.def" $(LINK32_OBJS)

"cmml-validate.exe" : "..\tools\cmml-validate.c"
    $(CPP) $(CFLAGS) /Fo".\cmml-validate.obj" /c "..\tools\cmml-validate.c"
    $(LINK32) $(PROG_LINK_FLAGS) /out:"cmml-validate.exe" ".\cmml-validate.obj" ".\attgetopt.obj" libcmml.lib

"cmml-fix.exe" : "..\tools\cmml-fix.c"
    $(CPP) $(CFLAGS) /Fo".\cmml-fix.obj" /c "..\tools\cmml-fix.c"
    $(LINK32) $(PROG_LINK_FLAGS) /out:"cmml-fix.exe" ".\cmml-fix.obj" ".\attgetopt.obj" libcmml.lib

#====================================================================
# C files in src

.\cmml.obj:
	$(CPP) $(CFLAGS) /Fo".\cmml.obj" /c "..\src\cmml.c"

.\cmml_private.obj:
	$(CPP) $(CFLAGS) /Fo".\cmml_private.obj" /c "..\src\cmml_private.c"

.\cmml_element.obj:
	$(CPP) $(CFLAGS) /Fo".\cmml_element.obj" /c "..\src\cmml_element.c"

.\cmml_list.obj:
	$(CPP) $(CFLAGS) /Fo".\cmml_list.obj" /c "..\src\cmml_list.c"

.\cmml_time.obj:
	$(CPP) $(CFLAGS) /Fo".\cmml_time.obj" /c "..\src\cmml_time.c"

.\attgetopt.obj:
	$(CPP) $(CFLAGS) /Fo".\attgetopt.obj" /c ".\attgetopt.c"

# End of Makefile
#====================================================================

