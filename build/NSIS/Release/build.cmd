set COMPILER=VS2005

rem Get revision number
svn info http://svn.xiph.org/trunk/oggdsf | findstr Revision > revision_text
set /p SVN_REVISION_FULL=<revision_text
set SVN_REVISION=%SVN_REVISION_FULL:~-5%
del revision_text

set PRODUCT_VERSION=0.81.%SVN_REVISION%

call copy_binaries.cmd
call copy_pdbs.cmd

signtool sign /a /t http://time.certum.pl/ bin\OOOggCommentDump.exe 
signtool sign /a /t http://time.certum.pl/ bin\OOOggDump.exe 
signtool sign /a /t http://time.certum.pl/ bin\OOOggStat.exe 
signtool sign /a /t http://time.certum.pl/ bin\OOOggValidate.exe 

"%ProgramFiles%\nsis\makensisw.exe" oggcodecs_release.nsi 

if [%X64%] == [] (
	set SUFFIX=win32
) else (
	set SUFFIX=x64
)

signtool sign /a /t http://time.certum.pl/ oggcodecs_%PRODUCT_VERSION%-%SUFFIX%.exe 
"%ProgramFiles%\7-zip\7z.exe" a oggcodecs_%PRODUCT_VERSION%_pdbs-%SUFFIX%.7z pdb\*
