;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; Copyright (C) 2005, 2006 Zentaro Kavanagh 
; Copyright (C) 2008 Cristian Adam
;
; NSIS install script for oggcodecs
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; Location of Visual Studio runtime libraries on the compiling system
!if "$%COMPILER%" == "VS2008"
	!define VS_RUNTIME_LOCATION "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\"
	!define VS_RUNTIME_SUFFIX 90
!else if "$%COMPILER%" == "VS2005"
	!define VS_RUNTIME_LOCATION "C:\Program Files\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT\"
	!define VS_RUNTIME_SUFFIX 80
!endif

!define VS_RUNTIME_PREFIX msvc
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!define PRODUCT_NAME "Ogg Codecs"

; Product version is setup in build.cmd
!define PRODUCT_VERSION "$%PRODUCT_VERSION%"

!ifndef PRODUCT_VERSION
  !define PRODUCT_VERSION 'anonymous-build'
!endif

!define PRODUCT_PUBLISHER "Xiph.Org"
!define PRODUCT_WEB_SITE "http://xiph.org/dshow/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\OOOggDump.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"
!define PRODUCT_SUPPORT "http://www.xiph.org/dshow/pmwiki.php/Main/ReportIssues"

; Path from .nsi to oggcodecs root
!define OGGCODECS_ROOT_DIR "..\..\.."

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; All the code below needed to create a signed uninstaller
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!ifdef INNER
  !echo "Inner invocation"                  		; just to see what's going on
  OutFile "$%TEMP%\tempinstaller.exe"       	; not really important where this is
  SetCompress off                           		; for speed
  
!else
  !echo "Outer invocation"
 
  ; Call makensis again, defining INNER.  This writes an installer for us which, when
  ; it is invoked, will just write the uninstaller to some location, and then exit.
  ; Be sure to substitute the name of this script here.
 
  !system "$\"${NSISDIR}\makensis$\" /DINNER oggcodecs_release.nsi" = 0
 
  ; So now run that installer we just created as %TEMP%\tempinstaller.exe.  Since it
  ; calls quit the return value isn't zero.
 
  !system "$%TEMP%\tempinstaller.exe" = 2
 
  ; That will have written an uninstaller binary for us.  Now we sign it with your
  ; favourite code signing tool.
 
  !system "signtool sign /a /t http://time.certum.pl/ $%TEMP%\uninst.exe" = 0
 
  ; Good.  Now we can carry on writing the real installer.
 
  OutFile "oggcodecs_${PRODUCT_VERSION}.exe"
  SetCompressor /SOLID lzma
  
!endif
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!include "extra\DumpLog.nsh"
!include "Library.nsh"
!include "MUI2.nsh"
!include "FileFunc.nsh"
!include "Memento.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${OGGCODECS_ROOT_DIR}\bin\xifish.ico"
!define MUI_UNICON "${OGGCODECS_ROOT_DIR}\bin\xifish.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "extra\header.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "extra\header_uninstall.bmp"

!define MUI_COMPONENTSPAGE_SMALLDESC

VIProductVersion "${PRODUCT_VERSION}.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Directshow Filters for Ogg Vorbis, Speex, Theora and FLAC"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "${PRODUCT_WEB_SITE}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (c) 2008 ${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "The Xiph Fish Logo and the Vorbis.com many-fish logos are trademarks (tm) of ${PRODUCT_PUBLISHER}"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Welcome page
!define MUI_WELCOMEPAGE_TITLE "Welcome to the ${PRODUCT_NAME} ${PRODUCT_VERSION} Setup Wizard"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of Directshow Filters for Ogg Vorbis, Speex, Theora and FLAC ${PRODUCT_VERSION}.$\r$\n$\r$\n${PRODUCT_PUBLISHER} is a collection of open source, multimedia-related projects. The most aggressive effort works to put the foundation standards of Internet audio and video into the public domain, where all Internet standards belong.$\r$\n$\r$\n$_CLICK"
!insertmacro MUI_PAGE_WELCOME
; License page
;!define MUI_LICENSEPAGE_CHECKBOX
!insertmacro MUI_PAGE_LICENSE "${OGGCODECS_ROOT_DIR}\COPYRIGHTS.rtf"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; COMPONENTS
!insertmacro MUI_PAGE_COMPONENTS

; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Instfiles page
!define MUI_FINISHPAGE_SHOWREADME
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Show release notes"
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION ShowReleaseNotes

; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!define MUI_PAGE_CUSTOMFUNCTION_PRE un.confirm
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English" 


Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
InstallDir "$PROGRAMFILES\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""

;Memento Settings
!define MEMENTO_REGISTRY_ROOT HKLM
!define MEMENTO_REGISTRY_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Function .onInit

  ${MementoSectionRestore}

!ifdef INNER
 
  ; If INNER is defined, then we aren't supposed to do anything except write out
  ; the installer.  This is better than processing a command line option as it means
  ; this entire code path is not present in the final (real) installer.
 
  WriteUninstaller "$%TEMP%\uninst.exe"
  Quit  ; just bail out quickly when running the "inner" installer
!endif

  !insertmacro MUI_LANGDLL_DISPLAY

  ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString"
  StrCmp $R0 "" job_done
 
  IfSilent +3
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "${PRODUCT_NAME} is already installed. $\n$\nClick `OK` to remove the existing version or `Cancel` to cancel this installation." IDOK uninst
  Abort

;Run the uninstaller
uninst:
  ClearErrors
  ; Copy the uninstaller to a temp location
  GetTempFileName $0
  CopyFiles $R0 $0
 
  ;Start the uninstaller using the option to not copy itself
  IfSilent 0 +3
  ExecWait '$0 /S /FromInstaller _?=$INSTDIR'
  Goto AfterSilent 
  ExecWait '$0 /FromInstaller _?=$INSTDIR'
AfterSilent:
 
  IfErrors no_remove_uninstaller
    ; In most cases the uninstall is successful at this point.
    ; You may also consider using a registry key to check whether 
    ; the user has chosen to uninstall. If you are using an uninstaller
    ; components page, make sure all sections are uninstalled.
    goto job_done
  no_remove_uninstaller:
    IfSilent +4
    MessageBox MB_ICONEXCLAMATION \
    "Unable to remove previous version of ${PRODUCT_NAME}"
    Abort
  
job_done:
  ; remove the copied uninstaller
  Delete '$0'
    
FunctionEnd

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; COM registration macros, with fallbacks on regsvr32
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
!macro RegisterCOM file
	!define LIBRARY_COM
	RegDLL "${file}"
	!undef LIBRARY_COM
	IfErrors 0 +2
	ExecWait '$SYSDIR\regsvr32.exe "/s" "${file}"'
!macroend

!macro UnRegisterCOM file
	!define LIBRARY_COM
	UnRegDLL "${file}"
	!undef LIBRARY_COM
	IfErrors 0 +2
	ExecWait '$SYSDIR\regsvr32.exe "/u" "/s" "${file}"'
!macroend


;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; Windows Media Player type registraton macro
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro WMPRegisterType typeName description

  WriteRegStr HKCR "WMP.${typeName}" "" "${description}"
  WriteRegStr HKCR "WMP.${typeName}\shell" "" "open"

  WriteRegStr HKCR "WMP.${typeName}\shell\open" "" "&Open"
  WriteRegStr HKCR "WMP.${typeName}\shell\open\command" "" "$WMP_LOCATION /Open $\"%L$\""
  
  WriteRegStr HKCR "WMP.${typeName}\shell\play" "" "&Play"
  WriteRegStr HKCR "WMP.${typeName}\shell\play\command" "" "$WMP_LOCATION /Play $\"%L$\""    

  ; WMP extra integration
  WriteRegStr HKCR "WMP.${typeName}\shellex\ContextMenuHandlers\WMPAddToPlaylist" "" "{F1B9284F-E9DC-4e68-9D7E-42362A59F0FD}"
  WriteRegStr HKCR "WMP.${typeName}\shellex\ContextMenuHandlers\WMPPlayAsPlaylist" "" "{CE3FB1D1-02AE-4a5f-A6E9-D9F1B4073E6C}"

  WriteRegStr HKCR "WMP.${typeName}\DefaultIcon" "" "$INSTDIR\xifish.ico"

!macroend

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Section "Oggcodecs Core Files" SEC_CORE
  SectionIn 1 RO
  
  SetShellVarContext all
  
  SetOutPath "$INSTDIR"
  SetOverwrite on

  SetDetailsPrint textonly
  DetailPrint "Copying Files ..."
  SetDetailsPrint listonly
  
  ; Runtime libraries from visual studio - 2
  File "${VS_RUNTIME_LOCATION}\${VS_RUNTIME_PREFIX}r${VS_RUNTIME_SUFFIX}.dll"
  File "${VS_RUNTIME_LOCATION}\${VS_RUNTIME_PREFIX}p${VS_RUNTIME_SUFFIX}.dll"
  File "${VS_RUNTIME_LOCATION}\Microsoft.VC${VS_RUNTIME_SUFFIX}.CRT.manifest"

  ; ico files - 1 (One file contains all these packed)
  File "${OGGCODECS_ROOT_DIR}\bin\xifish.ico"

  ; Libraries - 10
  File "bin\libOOOgg.dll"
  File "bin\libOOOggSeek.dll"
  File "bin\libCMMLTags.dll"
  File "bin\libCMMLParse.dll"
  File "bin\vorbis.dll"
 
  File "bin\libOOTheora.dll"
  File "bin\libFLAC.dll"
  File "bin\libFLAC++.dll"
  File "bin\libVorbisComment.dll"

  File "bin\libTemporalURI.dll"


  ; Utilites - 4
  File "bin\OOOggDump.exe"
  File "bin\OOOggStat.exe"
  File "bin\OOOggValidate.exe"
  File "bin\OOOggCommentDump.exe"


  ; Text files - 9
  File "${OGGCODECS_ROOT_DIR}\ABOUT.txt"
  File "${OGGCODECS_ROOT_DIR}\VERSIONS"
  File "${OGGCODECS_ROOT_DIR}\README"
  File "${OGGCODECS_ROOT_DIR}\COPYRIGHTS.rtf"
  File "${OGGCODECS_ROOT_DIR}\COPYRIGHTS"

  File "${OGGCODECS_ROOT_DIR}\AUTHORS"
  File "${OGGCODECS_ROOT_DIR}\HISTORY"
  File "${OGGCODECS_ROOT_DIR}\ChangeLog.txt"
  File "${OGGCODECS_ROOT_DIR}\bin\Ogg Codecs.manifest" 
  
  ; Install Filters - 16  

  File "bin\dsfFLACEncoder.dll"
  File "bin\dsfSpeexEncoder.dll"
  File "bin\dsfTheoraEncoder.dll"
  File "bin\dsfVorbisEncoder.dll"

  File "bin\dsfNativeFLACSource.dll"
  File "bin\dsfSpeexDecoder.dll"
  File "bin\dsfTheoraDecoder.dll"
  File "bin\dsfFLACDecoder.dll"
  File "bin\dsfVorbisDecoder.dll"

  File "bin\dsfOGMDecoder.dll"

  File "bin\dsfOggDemux2.dll"
  File "bin\dsfOggMux.dll"

  ; File "bin\dsfSeeking.dll"

  File "bin\dsfCMMLDecoder.dll"
  File "bin\dsfCMMLRawSource.dll"
  File "bin\dsfSubtitleVMR9.dll"

  ; File "bin\dsfAnxDemux.dll"
  File "bin\dsfAnxMux.dll"                                           

  SetDetailsPrint textonly
  DetailPrint "Registering DirectShow Filters ..."
  SetDetailsPrint listonly

  SetOutPath "$INSTDIR"
  ; Register libraries - 16

  !insertmacro RegisterCOM "$INSTDIR\dsfFLACEncoder.dll"
  !insertmacro RegisterCOM "$INSTDIR\dsfSpeexEncoder.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfTheoraEncoder.dll"
  !insertmacro RegisterCOM "$INSTDIR\dsfVorbisEncoder.dll" 
  
  !insertmacro RegisterCOM "$INSTDIR\dsfNativeFLACSource.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfSpeexDecoder.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfTheoraDecoder.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfFLACDecoder.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfVorbisDecoder.dll" 
  
  !insertmacro RegisterCOM "$INSTDIR\dsfOGMDecoder.dll" 
  
  !insertmacro RegisterCOM "$INSTDIR\dsfOggDemux2.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfOggMux.dll" 
  
  !insertmacro RegisterCOM "$INSTDIR\dsfCMMLDecoder.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfCMMLRawSource.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfSubtitleVMR9.dll" 
  
  ;!insertmacro RegisterCOM "$INSTDIR\dsfAnxDemux.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfAnxMux.dll"

  IfSilent +3
  Push $INSTDIR\Install.log
  Call DumpLog

  SetDetailsPrint textonly
  DetailPrint "Writing Registry Entries ..."
  SetDetailsPrint listonly

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Registry Entries for directshow and WMP
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	*	Media Group Entries for WMP
;;;			-	flac (audio)
;;;			-	oga
;;;			-	ogv
;;;			-	axa
;;;			-	axv
;;;			-	spx
;;;			-	ogm(????? TODO:::)
;;;			-	ogg(TODO::: Check if can have no group)
;;;	*	Mime Type Entries for WMP
;;;	*	Extension Entries for WMP - TODO::: Other entries, icons
;;;	*	Media Type Entries/Filter association for Directshow
;;;	*	MLS(?) Entries for WMP






;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Media Group Entries - 6
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\FLAC" "" "FLAC File (flac)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\FLAC" "Extensions" ".flac"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\FLAC" "MIME Types" "audio/x-flac"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGA" "" "Ogg Audio File (oga)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGA" "Extensions" ".oga"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGA" "MIME Types" "audio/x-ogg"
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\OGV" "" "Ogg Video File (ogv)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\OGV" "Extensions" ".ogv"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\OGV" "MIME Types" "video/x-ogg"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\AXA" "" "Annodex Audio File (axa)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\AXA" "Extensions" ".axa"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\AXA" "MIME Types" "audio/x-annodex"
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\AXV" "" "Annodex Video File (axv)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\AXV" "Extensions" ".axv"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\AXV" "MIME Types" "video/x-annodex"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\SPX" "" "Speex File (spx)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\SPX" "Extensions" ".spx"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\SPX" "MIME Types" "audio/x-ogg"
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	WMP Mime type entries - 7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;





  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/x-annodex" "" "Annodex File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/x-annodex" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/x-annodex" "Extension.Key" ".anx"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/x-annodex" "Extensions.CommaSep" "anx,axa,axv"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/x-annodex" "Extensions.SpaceSep" ".anx .axa .axv"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/ogg" "" "Ogg File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/ogg" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/ogg" "Extension.Key" ".ogg"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/ogg" "Extensions.CommaSep" "ogg,oga,ogv,spx"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/ogg" "Extensions.SpaceSep" ".ogg .oga .ogv .spx"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-flac" "" "FLAC Audio File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-flac" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-flac" "Extension.Key" ".flac"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-ogg" "" "Ogg Audio File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-ogg" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-ogg" "Extension.Key" ".oga"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-ogg" "Extensions.CommaSep" "oga,spx"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-ogg" "Extensions.SpaceSep" ".oga .spx"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-ogg" "" "Ogg Video File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-ogg" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-ogg" "Extension.Key" ".ogv"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-annodex" "" "Annodex Audio File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-annodex" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-annodex" "Extension.Key" ".axa"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-annodex" "Extensions.CommaSep" "axa"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-annodex" "Extensions.SpaceSep" ".axa"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-annodex" "" "Annodex Video File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-annodex" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-annodex" "Extension.Key" ".axv"
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	WMP extension entries - 8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.anx" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.anx" "MediaType.Description" "Annodex"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.anx" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.anx" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.anx" "Extension.MIME" "application/x-annodex"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa" "MediaType.Description" "Annodex Audio"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa" "PerceivedType" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa" "Extension.MIME" "audio/x-annodex"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv" "MediaType.Description" "Annodex Video"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv" "PerceivedType" "video"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv" "Extension.MIME" "video/x-annodex"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac" "MediaType.Description" "FLAC Audio"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac" "PerceivedType" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac" "Extension.MIME" "audio/x-flac"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga" "MediaType.Description" "Ogg Audio"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga" "PerceivedType" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga" "Extension.MIME" "audio/x-ogg"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg" "MediaType.Description" "Ogg"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg" "Extension.MIME" "application/ogg"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv" "MediaType.Description" "Ogg Video"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv" "PerceivedType" "video"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv" "Extension.MIME" "video/x-ogg"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx" "MediaType.Description" "Speex Audio"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx" "PerceivedType" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx" "Extension.MIME" "audio/x-ogg"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	Directshow extension to filter mapping - 8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.anx" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.axa" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.axv" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.flac" "Source Filter" "{6DDA37BA-0553-499a-AE0D-BEBA67204548}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.oga" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.ogg" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.ogv" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.spx" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	Directshow extension to filter mapping for HTTP - 7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  WriteRegStr HKCR "http\Extensions" ".OGG" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "http\Extensions" ".OGV" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "http\Extensions" ".OGA" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "http\Extensions" ".SPX" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "http\Extensions" ".ANX" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "http\Extensions" ".AXV" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "http\Extensions" ".AXA" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	MLS Perceived type - 6
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "ogv" "video"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "oga" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "axv" "video"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "axa" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "spx" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "flac" "audio"  
  
; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  !insertmacro MUI_STARTMENU_WRITE_END

; Check for Windows Media player
  Var /GLOBAL WMP_LOCATION  
 
  ReadRegStr $WMP_LOCATION HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer" "Player.Path"
  StrCmp $WMP_LOCATION "" 0 +3
  IfSilent +2
  MessageBox MB_OK|MB_ICONEXCLAMATION "A recognised version of Windows Media Player was not found. $\n File extenstion association must be done manually." IDOK 0

SectionEnd


;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
${MementoSection} ".ogg defaults to audio" SEC_OGG_AUDIO_DEFAULT
  SectionIn 1
  
  ; Make .ogg recognised as audio
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGG" "" "Ogg File (ogg)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGG" "Extensions" ".ogg"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGG" "MIME Types" "application/ogg"  
    
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg" "PerceivedType" "audio"
  
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "ogg" "audio"  

${MementoSectionEnd}

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SectionGroup "File type associations" SEC_USE_WMP_FOR_OGG

${MementoSection} ".ogg"  SecOgg
  SectionIn 1
  WriteRegStr HKCR ".ogg" "" "WMP.OggFile"
  !insertmacro WMPRegisterType "OggFile" "Ogg File"
${MementoSectionEnd}

${MementoSection} ".oga" SecOga
  SectionIn 1
  WriteRegStr HKCR ".oga" "" "WMP.OgaFile"
  !insertmacro WMPRegisterType "OgaFile" "Ogg Audio File"
${MementoSectionEnd}

${MementoSection} ".ogv"  SecOgv
  SectionIn 1
  WriteRegStr HKCR ".ogv" "" "WMP.OgvFile"
  !insertmacro WMPRegisterType "OgvFile" "Ogg Video File"
${MementoSectionEnd}

${MementoSection} ".spx"  SecSpx
  SectionIn 1
  WriteRegStr HKCR ".spx" "" "WMP.SpxFile"
  !insertmacro WMPRegisterType "SpxFile" "Speex File"
${MementoSectionEnd}

${MementoSection} ".flac" SecFlac
  SectionIn 1
  WriteRegStr HKCR ".flac" "" "WMP.FlacFile"
  !insertmacro WMPRegisterType "FlacFile" "FLAC File"
${MementoSectionEnd}

SectionGroupEnd

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LangString DESC_OggCoreSection ${LANG_ENGLISH} "Core files for ${PRODUCT_NAME}"
LangString DESC_OggExtensionAudioByDefault ${LANG_ENGLISH} "Makes files with .ogg extension default to the audio section in Windows Media Player Library."
LangString DESC_OggOpensInWMP ${LANG_ENGLISH} "Associates Ogg Files with Windows Media Player, so you can double click them in explorer."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_CORE} $(DESC_OggCoreSection)
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_OGG_AUDIO_DEFAULT} $(DESC_OggExtensionAudioByDefault)
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_USE_WMP_FOR_OGG} $(DESC_OggOpensInWMP)
!insertmacro MUI_FUNCTION_DESCRIPTION_END


;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Section -AdditionalIcons
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  WriteIniStr "$SMPROGRAMS\$ICONS_GROUP\Website.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

${MementoSectionDone}
	
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Function ShowReleaseNotes
	ExecShell "open" "$INSTDIR\ChangeLog.txt"
FunctionEnd

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Function .onInstSuccess
  ${MementoSectionSave}
FunctionEnd
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Section -Post

!ifndef INNER

  SetOutPath "$INSTDIR"
 
  ; this packages the signed uninstaller
 
  File "$%TEMP%\uninst.exe"
!endif  

  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\OOOggDump.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\xifish.ico"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"  "HelpLink" "${PRODUCT_SUPPORT}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  
  WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoModify" "1"
  WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoRepair" "1"

SetDetailsPrint both

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SectionEnd

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
!insertmacro GetOptions
!insertmacro un.GetOptions
!insertmacro un.GetParameters

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Function un.confirm
  var /GLOBAL cmdLineParams
  Push $R0

  ${un.GetParameters} $cmdLineParams

  Push $R0
  Var /GLOBAL option_runFromInstaller
  StrCpy $option_runFromInstaller	  0
  ${un.GetOptions} $cmdLineParams '/FromInstaller' $R0
  IfErrors +2 0
  StrCpy $option_runFromInstaller 	1
  Pop $R0

  StrCmp $option_runFromInstaller "1" 0 +2
  Abort
FunctionEnd

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
!ifdef INNER
Section Uninstall

  SetShellVarContext all

  SetDetailsPrint textonly
  DetailPrint "Unregistering DirectShow Filters ..."
  SetDetailsPrint listonly
  
  ; Unregister libraries - 16

  ; Unregister core annodex libraries

  !insertmacro UnRegisterCOM "$INSTDIR\dsfSubtitleVMR9.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfCMMLDecoder.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfCMMLRawSource.dll"
  
  ; !insertmacro UnRegisterCOM "$INSTDIR\dsfAnxDemux.dll"'
  !insertmacro UnRegisterCOM "$INSTDIR\dsfAnxMux.dll"

  
  ; Unregister core ogg libraries
  !insertmacro UnRegisterCOM "$INSTDIR\dsfOggDemux2.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfOggMux.dll"


  ; Unregister encoders
  !insertmacro UnRegisterCOM "$INSTDIR\dsfFLACEncoder.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfSpeexEncoder.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfTheoraEncoder.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfVorbisEncoder.dll"

  
  ; Unregister decoders
  !insertmacro UnRegisterCOM "$INSTDIR\dsfNativeFLACSource.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfSpeexDecoder.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfTheoraDecoder.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfFLACDecoder.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfVorbisDecoder.dll"

  !insertmacro UnRegisterCOM "$INSTDIR\dsfOGMDecoder.dll"

  SetDetailsPrint textonly
  DetailPrint "Deleting Registry Entries ..."
  SetDetailsPrint listonly
; Get rid of all the registry keys we made for directshow and WMP
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ; Media Type Groups entries - 6

  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\FLAC"  
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGA"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\SPX"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\AXA"

  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\OGV"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\AXV"


  ; MIME Type entries	- 7

  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/ogg"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-flac"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-ogg"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-ogg"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/x-annodex"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-annodex"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-annodex"


  ; File Extension Entries - 8

  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.anx"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv"

  
  ; Extension to filter mapping - 8

  DeleteRegKey HKCR "Media Type\Extensions\.anx"
  DeleteRegKey HKCR "Media Type\Extensions\.axa"
  DeleteRegKey HKCR "Media Type\Extensions\.axv"
  DeleteRegKey HKCR "Media Type\Extensions\.flac"
  DeleteRegKey HKCR "Media Type\Extensions\.oga"
  DeleteRegKey HKCR "Media Type\Extensions\.ogg"
  DeleteRegKey HKCR "Media Type\Extensions\.ogv"
  DeleteRegKey HKCR "Media Type\Extensions\.spx"


  ; Extension to filter mapping for http - 7
  DeleteRegValue HKCR "http\Extensions" ".OGG"
  DeleteRegValue HKCR "http\Extensions" ".OGV"
  DeleteRegValue HKCR "http\Extensions" ".OGA"
  DeleteRegValue HKCR "http\Extensions" ".SPX"
  DeleteRegValue HKCR "http\Extensions" ".ANX"
  DeleteRegValue HKCR "http\Extensions" ".AXA"
  DeleteRegValue HKCR "http\Extensions" ".AXV"
  ; TODO::: FLAC
  

  ; MLS Perceived type - 6
  DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "ogv"
  DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "oga"
  DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "axa"
  DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "axv"
  DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "spx"
  DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "flac"  

    ; Point the extension to the handlers
  DeleteRegKey HKCR "WMP.OggFile"
  DeleteRegKey HKCR "WMP.OgaFile"
  DeleteRegKey HKCR "WMP.OgvFile"
  DeleteRegKey HKCR "WMP.SpxFile"
  DeleteRegKey HKCR "WMP.FlacFile"
  
  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
 
  SetDetailsPrint textonly
  DetailPrint "Deleting Files ..."
  SetDetailsPrint listonly 
  Delete "$INSTDIR\Install.log"

  ; Delete utils - 4
  Delete "$INSTDIR\OOOggCommentDump.exe"
  Delete "$INSTDIR\OOOggValidate.exe"
  Delete "$INSTDIR\OOOggStat.exe"
  Delete "$INSTDIR\OOOggDump.exe"


  ; Delete libraries - 10
  Delete "$INSTDIR\libFLAC++.dll"
  Delete "$INSTDIR\libFLAC.dll"
  Delete "$INSTDIR\libOOTheora.dll"
  Delete "$INSTDIR\vorbis.dll"

  Delete "$INSTDIR\libCMMLParse.dll"
  Delete "$INSTDIR\libCMMLTags.dll"
  Delete "$INSTDIR\libVorbisComment.dll"
  Delete "$INSTDIR\libOOOggSeek.dll"
  Delete "$INSTDIR\libOOOgg.dll"

  Delete "$INSTDIR\libTemporalURI.dll"

  ;Delete Filters - 16
  Delete "$INSTDIR\dsfVorbisEncoder.dll"
  Delete "$INSTDIR\dsfTheoraEncoder.dll"
  Delete "$INSTDIR\dsfSpeexEncoder.dll"
  Delete "$INSTDIR\dsfFLACEncoder.dll"

  Delete "$INSTDIR\dsfVorbisDecoder.dll"
  Delete "$INSTDIR\dsfFLACDecoder.dll"
  Delete "$INSTDIR\dsfTheoraDecoder.dll"
  Delete "$INSTDIR\dsfSpeexDecoder.dll"
  Delete "$INSTDIR\dsfOGMDecoder.dll"

  Delete "$INSTDIR\dsfNativeFLACSource.dll"

  Delete "$INSTDIR\dsfCMMLDecoder.dll"
  Delete "$INSTDIR\dsfCMMLRawSource.dll"

  Delete "$INSTDIR\dsfSubtitleVMR9.dll"
  
  Delete "$INSTDIR\dsfOggDemux2.dll"
  Delete "$INSTDIR\dsfOggMux.dll"

  ; Delete "$INSTDIR\dsfSeeking.dll"
  

  Delete "$INSTDIR\dsfAnxMux.dll"
  ; Delete "$INSTDIR\dsfAnxDemux.dll"


  ; Delete text files - 9
  Delete "$INSTDIR\ABOUT.txt"
  Delete "$INSTDIR\VERSIONS"
  Delete "$INSTDIR\README"
  Delete "$INSTDIR\COPYRIGHTS.rtf"
  Delete "$INSTDIR\COPYRIGHTS"

  Delete "$INSTDIR\AUTHORS"
  Delete "$INSTDIR\HISTORY"
  Delete "$INSTDIR\ChangeLog.txt"
  Delete "$INSTDIR\Ogg Codecs.manifest" 

  ; Delete runtimes - 2
  Delete "$INSTDIR\${VS_RUNTIME_PREFIX}r${VS_RUNTIME_SUFFIX}.dll"
  Delete "$INSTDIR\${VS_RUNTIME_PREFIX}p${VS_RUNTIME_SUFFIX}.dll"
  Delete "$INSTDIR\Microsoft.VC${VS_RUNTIME_SUFFIX}.CRT.manifest"

  ; Delete icons - 3
  Delete "$INSTDIR\xifish.ico"

  ;Delete accesory files, links etc.
  Delete "$SMPROGRAMS\$ICONS_GROUP\Website.url"
  Delete "$INSTDIR\uninst.exe"

  RMDir "$SMPROGRAMS\$ICONS_GROUP"
  ; Remove the start menu group (but only if it's empty)
  RMDir "$SMPROGRAMS\$ICONS_GROUP\.."

  ; Need to change the working directory to something else (anything) besides
  ; the output directory, so we can rmdir it
  SetOutPath "$TEMP"
  RMDir "$INSTDIR"

  ; Remove the parent directory (but only if it's empty)
  RMDir "$INSTDIR\.."

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"

  StrCmp $option_runFromInstaller "1" 0 +2
  SetAutoClose true
  
  SetDetailsPrint both
  
SectionEnd
!endif ; INNER