@set OGGCODECS_ROOT_DIR=..\..\..

if [%X64%] == [] (
	@set OGGCODECS_CONFIG_PATH=Release
	@set OGGCODECS_VORBIS_CONFIG_PATH=%COMPILER%\libvorbis\Release
) else (
	@set OGGCODECS_CONFIG_PATH=x64\Release
	@set OGGCODECS_VORBIS_CONFIG_PATH=%COMPILER%\libvorbis\x64\Release
)

rmdir /s /q pdb
mkdir pdb

rem Libraries - 11
copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\ogg\libOOOgg\%OGGCODECS_CONFIG_PATH%\libOOOgg.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\ogg\libOOOggSeek\%OGGCODECS_CONFIG_PATH%\libOOOggSeek.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\cmml\libCMMLTags\%OGGCODECS_CONFIG_PATH%\libCMMLTags.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\cmml\libCMMLParse\%OGGCODECS_CONFIG_PATH%\libCMMLParse.pdb" pdb\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\vorbis\libs\libvorbis\win32\%OGGCODECS_VORBIS_CONFIG_PATH%\vorbis.pdb" pdb\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\theora\libs\libOOTheora\%OGGCODECS_CONFIG_PATH%\libOOTheora.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\flac\libs\libflac\obj\%OGGCODECS_CONFIG_PATH%\lib\libFLAC.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\flac\libs\libflac\obj\%OGGCODECS_CONFIG_PATH%\lib\libFLAC++.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\ogg\libVorbisComment\%OGGCODECS_CONFIG_PATH%\libVorbisComment.pdb" pdb\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\helper\libTemporalURI\%OGGCODECS_CONFIG_PATH%\libTemporalURI.pdb" pdb\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\helper\libTemporalURI\%OGGCODECS_CONFIG_PATH%\libTemporalURI.pdb" pdb\

rem Utilites - 4
copy  "%OGGCODECS_ROOT_DIR%\src\tools\OOOggDump\%OGGCODECS_CONFIG_PATH%\OOOggDump.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\tools\OOOggStat\%OGGCODECS_CONFIG_PATH%\OOOggStat.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\tools\OOOggValidate\%OGGCODECS_CONFIG_PATH%\OOOggValidate.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\tools\OOOggCommentDump\%OGGCODECS_CONFIG_PATH%\OOOggCommentDump.pdb" pdb\

rem Install Filters - 16
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\flac\filters\dsfFLACEncoder\%OGGCODECS_CONFIG_PATH%\dsfFLACEncoder.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\speex\filters\dsfSpeexEncoder\%OGGCODECS_CONFIG_PATH%\dsfSpeexEncoder.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\theora\filters\dsfTheoraEncoder\%OGGCODECS_CONFIG_PATH%\dsfTheoraEncoder.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\vorbis\filters\dsfVorbisEncoder\%OGGCODECS_CONFIG_PATH%\dsfVorbisEncoder.pdb" pdb\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\flac\filters\dsfNativeFLACSource\%OGGCODECS_CONFIG_PATH%\dsfNativeFLACSource.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\speex\filters\dsfSpeexDecoder\%OGGCODECS_CONFIG_PATH%\dsfSpeexDecoder.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\theora\filters\dsfTheoraDecoder\%OGGCODECS_CONFIG_PATH%\dsfTheoraDecoder.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\flac\filters\dsfFLACDecoder\%OGGCODECS_CONFIG_PATH%\dsfFLACDecoder.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\vorbis\filters\dsfVorbisDecoder\%OGGCODECS_CONFIG_PATH%\dsfVorbisDecoder.pdb" pdb\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\ogm\filters\dsfOGMDecoder\%OGGCODECS_CONFIG_PATH%\dsfOGMDecoder.pdb" pdb\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\directshow\dsfOggDemux2\%OGGCODECS_CONFIG_PATH%\dsfOggDemux2.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\directshow\dsfOggMux\%OGGCODECS_CONFIG_PATH%\dsfOggMux.pdb" pdb\

rem copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\directshow\dsfSeeking\%OGGCODECS_CONFIG_PATH%\dsfSeeking.pdb" pdb\

copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\cmml\dsfCMMLDecoder\%OGGCODECS_CONFIG_PATH%\dsfCMMLDecoder.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\codecs\cmml\dsfCMMLRawSource\%OGGCODECS_CONFIG_PATH%\dsfCMMLRawSource.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\directshow\dsfSubtitleVMR9\%OGGCODECS_CONFIG_PATH%\dsfSubtitleVMR9.pdb" pdb\

rem copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\directshow\dsfAnxDemux\%OGGCODECS_CONFIG_PATH%\dsfAnxDemux.pdb" pdb\
copy  "%OGGCODECS_ROOT_DIR%\src\lib\core\directshow\dsfAnxMux\%OGGCODECS_CONFIG_PATH%\dsfAnxMux.pdb" pdb\
