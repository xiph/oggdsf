::---------------------------------------------------------------------------------------------------------------------------------
:: Copyright (C) 2008 - 2010 Cristian Adam
::---------------------------------------------------------------------------------------------------------------------------------

set COMPILER=VS2008

:: Get revision number
svn info http://svn.xiph.org/trunk/oggdsf | findstr Revision > revision_text
set /p SVN_REVISION_FULL=<revision_text
set SVN_REVISION=%SVN_REVISION_FULL:~-5%
del revision_text

set PRODUCT_VERSION=0.85.%SVN_REVISION%
set OPENCODECS_ROOT_DIR=..\..\..

@set FILTERS=dsfFLACEncoder dsfNativeFLACSource dsfFLACDecoder
@set FILTERS=%FILTERS% dsfTheoraEncoder dsfTheoraDecoder
@set FILTERS=%FILTERS% dsfSpeexEncoder dsfSpeexDecoder
@set FILTERS=%FILTERS% dsfVorbisEncoder dsfVorbisDecoder
@set FILTERS=%FILTERS% dsfOggMux dsfOggDemux2
@set FILTERS=%FILTERS% vp8decoder vp8encoder
@set FILTERS=%FILTERS% webmmux webmsplit
@set FILTERS=%FILTERS% wmpinfo AxPlayer

call:copy_binaries
call:make_installer

goto:eof

::---------------------------------------------------------------------------------------------------------------------------------

:make_installer

signtool sign /a /t http://time.certum.pl/ bin\win32\AxPlayer.dll
signtool sign /a /t http://time.certum.pl/ bin\x64\AxPlayer.dll
"%ProgramFiles%\nsis\unicode\makensis.exe" opencodecs.nsi 

signtool sign /a /t http://time.certum.pl/ opencodecs_%PRODUCT_VERSION%.exe 
"%ProgramFiles%\7-zip\7z.exe" a opencodecs_%PRODUCT_VERSION%_pdbs.7z pdb\*

goto:eof
::---------------------------------------------------------------------------------------------------------------------------------

:copy_binaries

rmdir /s /q bin

mkdir bin\win32
for %%i in (%FILTERS%) do (
copy "%OPENCODECS_ROOT_DIR%\sln\oggdsf_%COMPILER%\win32\Release\%%i.dll" bin\win32
)

mkdir bin\x64
for %%i in (%FILTERS%) do (
copy "%OPENCODECS_ROOT_DIR%\sln\oggdsf_%COMPILER%\x64\Release\%%i.dll" bin\x64
)

rmdir /s /q pdb

mkdir pdb\win32
for %%i in (%FILTERS%) do (
copy "%OPENCODECS_ROOT_DIR%\sln\oggdsf_%COMPILER%\win32\Release\%%i.pdb" pdb\win32
)

mkdir pdb\x64
for %%i in (%FILTERS%) do (
copy "%OPENCODECS_ROOT_DIR%\sln\oggdsf_%COMPILER%\x64\Release\%%i.pdb" pdb\x64
)

goto:eof
