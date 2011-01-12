::---------------------------------------------------------------------------------------------------------------------------------
:: Copyright (C) 2011 Cristian Adam
::---------------------------------------------------------------------------------------------------------------------------------

set COMPILER=VS2008
set OLD_VERSION=9.0.30729.1
set NEW_VERSION=9.0.30729.4148

set OPENCODECS_ROOT_DIR=..\..\..

@set FILTERS=dsfFLACEncoder dsfNativeFLACSource dsfFLACDecoder
@set FILTERS=%FILTERS% dsfTheoraEncoder dsfTheoraDecoder
@set FILTERS=%FILTERS% dsfSpeexEncoder dsfSpeexDecoder
@set FILTERS=%FILTERS% dsfVorbisEncoder dsfVorbisDecoder
@set FILTERS=%FILTERS% dsfOggMux dsfOggDemux2
@set FILTERS=%FILTERS% vp8decoder vp8encoder
@set FILTERS=%FILTERS% webmmux webmsplit
@set FILTERS=%FILTERS% wmpinfo AxPlayer

for %%i in (%FILTERS%) do (
call:fix_binary "%OPENCODECS_ROOT_DIR%\sln\oggdsf_%COMPILER%\win32\Release\%%i.dll" 
)

for %%i in (%FILTERS%) do (
call:fix_binary "%OPENCODECS_ROOT_DIR%\sln\oggdsf_%COMPILER%\x64\Release\%%i.dll"
)

goto:eof

:fix_binary

mt.exe -inputresource:%1;#2 -out:%1.manifest
cscript.exe replace_string.js %1.manifest  %OLD_VERSION% %NEW_VERSION%
mt.exe -outputresource:%1;#2 -manifest %1.manifest 

goto:eof
