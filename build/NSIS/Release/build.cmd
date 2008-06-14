@echo off
call copy_binaries.cmd
set PRODUCT_VERSION=0.80.15038
"%ProgramFiles%\nsis\makensisw.exe" oggcodecs_release.nsi 
signtool sign /a /t http://time.certum.pl/ oggcodecs_%PRODUCT_VERSION%.exe 