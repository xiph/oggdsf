@echo off
cd "Ogg Codecs"
del LangDLL.dll nsDialogs.dll StartMenu.dll System.dll uninst.exe modern-header.bmp modern-wizard.bmp xifish.ico 
cd ..

for %%i in ("Ogg Codecs\*.*") do echo "%cd%\%%i" >> files.ddf
makecab /L .  /D CompresionMemory=21 /D CompressionType=LZX  /D DestinationDir="Ogg Codecs" /F files.ddf 
del files.ddf
move disk1\1.cab "%1.cab"
rmdir disk1
del setup.inf
del setup.rpt
