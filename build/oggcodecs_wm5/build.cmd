::---------------------------------------------------------------------------------------------------------------------------------
:: Copyright (C)  2010 Cristian Adam
::---------------------------------------------------------------------------------------------------------------------------------

set COMPILER=VS2005
set PLATFORM=windows mobile 5.0 smartphone sdk (armv4i)
set SUFFIX=wm5

:: Get revision number
svn info http://svn.xiph.org/trunk/oggdsf | findstr Revision > revision_text
set /p SVN_REVISION_FULL=<revision_text
set SVN_REVISION=%SVN_REVISION_FULL:~-5%
del revision_text

set PRODUCT_VERSION=0.82.%SVN_REVISION%
set OGGCODECS_ROOT_DIR=..\..

@set FILTERS=dsfNativeFLACSource dsfOggDemux2
@set FILTERS=%FILTERS% dsfSpeexDecoder dsfTheoraDecoder
@set FILTERS=%FILTERS% dsfVorbisDecoder wmpinfo

call:copy_binaries
call:make_installer

goto:eof

::---------------------------------------------------------------------------------------------------------------------------------

:make_installer

for %%i in (%FILTERS%) do (
signtool sign /a /t http://time.certum.pl/ bin\%%i.dll
)

mkdir Release
devenv oggcodecs_wm5.sln /Rebuild "Release"
move Release\oggcodecs_wm5.CAB oggcodecs_%PRODUCT_VERSION%-wm5.cab

signtool sign /a /t http://time.certum.pl/  oggcodecs_%PRODUCT_VERSION%-%SUFFIX%.cab
"%ProgramFiles%\7-zip\7z.exe" a oggcodecs_%PRODUCT_VERSION%_pdbs-%SUFFIX%.7z pdb\*

goto:eof
::---------------------------------------------------------------------------------------------------------------------------------

:copy_binaries

rmdir /s /q bin
mkdir bin

for %%i in (%FILTERS%) do (
copy "%OGGCODECS_ROOT_DIR%\sln\oggdsf_%COMPILER%\%PLATFORM%\Release\%%i.dll" bin\
)

rmdir /s /q pdb
mkdir pdb

for %%i in (%FILTERS%) do (
copy "%OGGCODECS_ROOT_DIR%\sln\oggdsf_%COMPILER%\%PLATFORM%\Release\%%i.pdb" pdb\
)

goto:eof
