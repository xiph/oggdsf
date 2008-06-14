@echo off
call copy_binaries.cmd
set PRODUCT_VERSION=0.80.15039

signtool sign /a /t http://time.certum.pl/ bin\OOOggCommentDump.exe 
signtool sign /a /t http://time.certum.pl/ bin\OOOggDump.exe 
signtool sign /a /t http://time.certum.pl/ bin\OOOggStat.exe 
signtool sign /a /t http://time.certum.pl/ bin\OOOggValidate.exe 

"%ProgramFiles%\nsis\makensisw.exe" oggcodecs_release.nsi 
signtool sign /a /t http://time.certum.pl/ oggcodecs_%PRODUCT_VERSION%.exe 
