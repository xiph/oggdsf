@set OGGCODECS_ROOT_DIR=..\..\..
@set OGGCODECS_CONFIG_PATH=Release
@set OGGCODECS_VORBIS_CONFIG_PATH=%COMPILER%\libvorbis\Release

rmdir /s /q bin
mkdir bin

rem Libraries - 11
copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\ogg\libOOOgg\%OGGCODECS_CONFIG_PATH%\libOOOgg.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\ogg\libOOOggSeek\%OGGCODECS_CONFIG_PATH%\libOOOggSeek.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\cmml\libCMMLTags\%OGGCODECS_CONFIG_PATH%\libCMMLTags.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\cmml\libCMMLParse\%OGGCODECS_CONFIG_PATH%\libCMMLParse.dll" bin\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\vorbis\libs\libvorbis\win32\%OGGCODECS_VORBIS_CONFIG_PATH%\vorbis.dll" bin\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\theora\libs\libOOTheora\%OGGCODECS_CONFIG_PATH%\libOOTheora.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\flac\libs\libflac\obj\%OGGCODECS_CONFIG_PATH%\lib\libFLAC.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\flac\libs\libflac\obj\%OGGCODECS_CONFIG_PATH%\lib\libFLAC++.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\ogg\libVorbisComment\%OGGCODECS_CONFIG_PATH%\libVorbisComment.dll" bin\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\helper\libTemporalURI\%OGGCODECS_CONFIG_PATH%\libTemporalURI.dll" bin\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\helper\libTemporalURI\%OGGCODECS_CONFIG_PATH%\libTemporalURI.dll" bin\

rem Utilites - 4
copy  "%OGGCODECS_ROOT_DIR%\src\tools\OOOggDump\%OGGCODECS_CONFIG_PATH%\OOOggDump.exe" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\tools\OOOggStat\%OGGCODECS_CONFIG_PATH%\OOOggStat.exe" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\tools\OOOggValidate\%OGGCODECS_CONFIG_PATH%\OOOggValidate.exe" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\tools\OOOggCommentDump\%OGGCODECS_CONFIG_PATH%\OOOggCommentDump.exe" bin\

rem Install Filters - 16
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\flac\filters\dsfFLACEncoder\%OGGCODECS_CONFIG_PATH%\dsfFLACEncoder.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\speex\filters\dsfSpeexEncoder\%OGGCODECS_CONFIG_PATH%\dsfSpeexEncoder.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\theora\filters\dsfTheoraEncoder\%OGGCODECS_CONFIG_PATH%\dsfTheoraEncoder.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\vorbis\filters\dsfVorbisEncoder\%OGGCODECS_CONFIG_PATH%\dsfVorbisEncoder.dll" bin\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\flac\filters\dsfNativeFLACSource\%OGGCODECS_CONFIG_PATH%\dsfNativeFLACSource.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\speex\filters\dsfSpeexDecoder\%OGGCODECS_CONFIG_PATH%\dsfSpeexDecoder.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\theora\filters\dsfTheoraDecoder\%OGGCODECS_CONFIG_PATH%\dsfTheoraDecoder.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\flac\filters\dsfFLACDecoder\%OGGCODECS_CONFIG_PATH%\dsfFLACDecoder.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\vorbis\filters\dsfVorbisDecoder\%OGGCODECS_CONFIG_PATH%\dsfVorbisDecoder.dll" bin\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\ogm\filters\dsfOGMDecoder\%OGGCODECS_CONFIG_PATH%\dsfOGMDecoder.dll" bin\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\directshow\dsfOggDemux2\%OGGCODECS_CONFIG_PATH%\dsfOggDemux2.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\directshow\dsfOggMux\%OGGCODECS_CONFIG_PATH%\dsfOggMux.dll" bin\

rem copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\directshow\dsfSeeking\%OGGCODECS_CONFIG_PATH%\dsfSeeking.dll" bin\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\cmml\dsfCMMLDecoder\%OGGCODECS_CONFIG_PATH%\dsfCMMLDecoder.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\cmml\dsfCMMLRawSource\%OGGCODECS_CONFIG_PATH%\dsfCMMLRawSource.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\directshow\dsfSubtitleVMR9\%OGGCODECS_CONFIG_PATH%\dsfSubtitleVMR9.dll" bin\

rem copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\directshow\dsfAnxDemux\%OGGCODECS_CONFIG_PATH%\dsfAnxDemux.dll" bin\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\directshow\dsfAnxMux\%OGGCODECS_CONFIG_PATH%\dsfAnxMux.dll" bin\
