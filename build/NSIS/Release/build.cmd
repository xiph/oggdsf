@echo off
set COMPILER=VS2008
set PRODUCT_VERSION=0.80.15413

call copy_binaries.cmd
call copy_pdbs.cmd

signtool sign /a /t http://time.certum.pl/ bin\OOOggCommentDump.exe 
signtool sign /a /t http://time.certum.pl/ bin\OOOggDump.exe 
signtool sign /a /t http://time.certum.pl/ bin\OOOggStat.exe 
signtool sign /a /t http://time.certum.pl/ bin\OOOggValidate.exe 

"%ProgramFiles%\nsis\makensisw.exe" oggcodecs_release.nsi 
signtool sign /a /t http://time.certum.pl/ oggcodecs_%PRODUCT_VERSION%.exe 
"%ProgramFiles%\7-zip\7z.exe" a oggcodecs-%PRODUCT_VERSION%-pdbs.7z pdb\*
