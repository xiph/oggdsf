;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; Copyright (C) 2005 - 2006 Zentaro Kavanagh 
; Copyright (C) 2008 - 2010 Cristian Adam
;
; NSIS install script for oggcodecs
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; Location of Visual Studio runtime libraries on the compiling system
!if "$%COMPILER%" == "VS2008"
  !if "$%X64%" == "true"
    !define VS_RUNTIME_LOCATION "d:\Program Files\Microsoft Visual Studio 9.0\VC\redist\amd64\Microsoft.VC90.CRT\"
  !else
    !define VS_RUNTIME_LOCATION "d:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\"
  !endif
  !define VS_RUNTIME_SUFFIX 90
!else if "$%COMPILER%" == "VS2005"
  !if "$%X64%" == "true"
    !define VS_RUNTIME_LOCATION "d:\Program Files\Microsoft Visual Studio 8\VC\redist\amd64\Microsoft.VC80.CRT\"
  !else
    !define VS_RUNTIME_LOCATION "d:\Program Files\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT\"	
  !endif
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
!define PRODUCT_DIR_REGKEY "Software\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"
!define PRODUCT_SUPPORT "http://xiph.org/dshow/bugs/"

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
 
!if "$%X64%" == "true" 
  OutFile "oggcodecs_${PRODUCT_VERSION}-x64.exe"
!else
  OutFile "oggcodecs_${PRODUCT_VERSION}-win32.exe"
!endif
  SetCompressor /SOLID lzma
  
!endif
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!include "extra\DumpLog.nsh"
!include "Library.nsh"
!include "MUI2.nsh"
!include "FileFunc.nsh"
!include "Memento.nsh"
!include "x64.nsh"
!include "LogicLib.nsh"
!include "WinVer.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${OGGCODECS_ROOT_DIR}\bin\xifish.ico"
!define MUI_UNICON "${OGGCODECS_ROOT_DIR}\bin\xifish.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "extra\header.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "extra\header_uninstall.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "extra\wizard.bmp"

!define MUI_COMPONENTSPAGE_SMALLDESC

VIProductVersion "${PRODUCT_VERSION}.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Directshow Filters for Ogg Vorbis, Speex, Theora and FLAC"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "${PRODUCT_WEB_SITE}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (c) 2008 - 2009 ${PRODUCT_PUBLISHER}"
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

!if "$%X64%" == "true" 
  Name "${PRODUCT_PUBLISHER} ${PRODUCT_NAME} ${PRODUCT_VERSION} 64-bit"
  InstallDir "$PROGRAMFILES64\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
!else
  Name "${PRODUCT_PUBLISHER} ${PRODUCT_NAME} ${PRODUCT_VERSION} 32-bit"
  InstallDir "$PROGRAMFILES\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
!endif

;Memento Settings
!define MEMENTO_REGISTRY_ROOT HKLM
!define MEMENTO_REGISTRY_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Function .onInit

!if "$%X64%" == "true"
  SetRegView 64
!endif

  ; Read here the installdir path instead of using InstallDirRegKey because it doesn't work on 64bit registry view
  ReadRegStr $INSTDIR HKLM "${PRODUCT_DIR_REGKEY}" ""
  ${If} $INSTDIR == ""
!if "$%X64%" == "true"
  StrCpy $INSTDIR "$PROGRAMFILES64\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
!else
  StrCpy $INSTDIR "$PROGRAMFILES\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
!endif
  ${EndIf}
  
  ${MementoSectionRestore}

!ifdef INNER
 
  ; If INNER is defined, then we aren't supposed to do anything except write out
  ; the installer.  This is better than processing a command line option as it means
  ; this entire code path is not present in the final (real) installer.
 
  WriteUninstaller "$%TEMP%\uninst.exe"
  Quit  ; just bail out quickly when running the "inner" installer
!endif

!if "$%X64%" == "true"  
  ${IfNot} ${RunningX64}
  IfSilent +2
  MessageBox MB_OK|MB_ICONEXCLAMATION  "This installation package is not supported by this processor type."
  Abort
  ${EndIf}
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
  !if "$%X64%" == "true"
    ExecWait '$SYSDIR\regsvr32.exe "/s" "${file}"'
  !else
    !define LIBRARY_COM
    RegDLL "${file}"
    !undef LIBRARY_COM
    IfErrors 0 +2
    ExecWait '$SYSDIR\regsvr32.exe "/s" "${file}"'
  !endif
!macroend

!macro UnRegisterCOM file
  !if "$%X64%" == "true"
    ExecWait '$SYSDIR\regsvr32.exe "/u" "/s" "${file}"'
  !else
    !define LIBRARY_COM
    UnRegDLL "${file}"
    !undef LIBRARY_COM
    IfErrors 0 +2
    ExecWait '$SYSDIR\regsvr32.exe "/u" "/s" "${file}"'
  !endif
!macroend


;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; Windows Media Player type registraton macro
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro RegisterWMPType typeName description

  WriteRegStr HKCR "${typeName}" "" "${description}"
  WriteRegStr HKCR "${typeName}\shell" "" "play"

  WriteRegStr HKCR "${typeName}\shell\open" "" "&Open"
  WriteRegStr HKCR "${typeName}\shell\open\command" "" "$WMP_LOCATION /Open $\"%L$\""
  
  WriteRegStr HKCR "${typeName}\shell\play" "" "&Play"
  WriteRegStr HKCR "${typeName}\shell\play\command" "" "$WMP_LOCATION /Play $\"%L$\""    

  ${If} ${AtMostWinVista}
    ; WMP extra integration
    WriteRegStr HKCR "${typeName}\shellex\ContextMenuHandlers\WMPAddToPlaylist" "" "{F1B9284F-E9DC-4e68-9D7E-42362A59F0FD}"
    WriteRegStr HKCR "${typeName}\shellex\ContextMenuHandlers\WMPPlayAsPlaylist" "" "{CE3FB1D1-02AE-4a5f-A6E9-D9F1B4073E6C}"
  ${EndIf}

  WriteRegStr HKCR "${typeName}\DefaultIcon" "" "$INSTDIR\xifish.ico"

!macroend

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro RegisterExtension Ext WMPType MIMEType PerceivedType
    WriteRegStr HKCR "${Ext}" "" "${WMPType}"
    WriteRegStr HKCR "${Ext}" "Content Type" "${MIMEType}"
    WriteRegStr HKCR "${Ext}" "PerceivedType" "${PerceivedType}"
    WriteRegStr HKCR "Applications\wmplayer.exe\supportedtypes" "${Ext}" ""
!macroend

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro RemoveMediaPlayerDesc RegKey
  ReadRegStr $0 HKLM "${RegKey}" "MediaDescNum"
  ${If} $0 != ""
    DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\Descriptions" $0 
    DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\MUIDescriptions" $0 
    DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\Types" $0 
  ${EndIf}  
!macroend

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro AddWMPExtension TypeExt TypeDesc MIMEType PerceivedType ExtensionHandler
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "MediaType.Description" "${TypeDesc}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "MediaType.Icon" "$INSTDIR\xifish.ico"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "Runtime" 0x00000007
  ${If} "${PerceivedType}" != ""  
    WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "PerceivedType" "${PerceivedType}"
  ${EndIf}
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "Extension.MIME" "${MIMEType}"
  ${if} "${ExtensionHandler}" != ""
    WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "Extension.Handler" "${ExtensionHandler}"
  ${EndIf}
  
!macroend 

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro AddWMPMediaGroup TypeExt TypeUppercase TypeDesc MIMEType PerceivedType
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\${PerceivedType}\${TypeUppercase}" "" "${TypeDesc}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\${PerceivedType}\${TypeUppercase}" "Extensions" "${TypeExt}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\${PerceivedType}\${TypeUppercase}" "MIME Types" "${MIMEType}"
!macroend

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro AddWMPMimeTypes MIMEType TypeDesc TypeExt TypeExtComma TypeExtSpace
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\${MIMEType}" "" "${TypeDesc}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\${MIMEType}" "Extension.Key" "${TypeExt}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\${MIMEType}" "Extensions.CommaSep" "${TypeExtComma}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\${MIMEType}" "Extensions.SpaceSep" "${TypeExtSpace}"
!macroend

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Section "Oggcodecs Core Files" SEC_CORE

!if "$%X64%" == "true"
  SetRegView 64
!endif

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


  ; Text files - 7
  File "${OGGCODECS_ROOT_DIR}\README"
  File "${OGGCODECS_ROOT_DIR}\COPYRIGHTS.rtf"
  File "${OGGCODECS_ROOT_DIR}\COPYRIGHTS"

  File "${OGGCODECS_ROOT_DIR}\AUTHORS"
  File "${OGGCODECS_ROOT_DIR}\HISTORY"
  File "${OGGCODECS_ROOT_DIR}\ChangeLog.txt"
!if "$%X64%" == "true"
  File "${OGGCODECS_ROOT_DIR}\bin\x64\Ogg Codecs.manifest" 
!else
  File "${OGGCODECS_ROOT_DIR}\bin\Ogg Codecs.manifest" 
!endif
  
  ; Install Filters - 15  

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

  ; File "bin\dsfAnxDemux.dll"
  File "bin\dsfAnxMux.dll"                                           

  File "bin\wmpinfo.dll"

  ; HTML <video> Tag Implementation
  File "bin\AxPlayer.dll"

  SetDetailsPrint textonly
  DetailPrint "Registering DirectShow Filters ..."
  SetDetailsPrint listonly

  SetOutPath "$INSTDIR"
  ; Register libraries - 15

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
  
  ;!insertmacro RegisterCOM "$INSTDIR\dsfAnxDemux.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfAnxMux.dll"
  
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

!insertmacro AddWMPMediaGroup ".flac" "FLAC" "FLAC File (flac)" "audio/flac" "Audio"
!insertmacro AddWMPMediaGroup ".oga" "OGA" "Ogg Audio File (oga)" "audio/ogg" "Audio"
!insertmacro AddWMPMediaGroup ".ogg" "OGG" "Ogg Audio File (ogg)" "audio/ogg" "Audio"
!insertmacro AddWMPMediaGroup ".ogv" "OGV" "Ogg Video File (ogv)" "video/ogg" "Video"
!insertmacro AddWMPMediaGroup ".axa" "AXA" "Annodex Audio File (axa)" "audio/annodex" "Audio"
!insertmacro AddWMPMediaGroup ".axv" "AXV" "Annodex Video File (axv)" "video/annodex" "Video"
!insertmacro AddWMPMediaGroup ".spx" "SPX" "Speex File (spx)" "audio/ogg" "Audio"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	WMP Mime type entries - 7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

!insertmacro AddWMPMimeTypes "application/annodex" "Annodex File" ".anx" "anx,axa,axv" ".anx .axa .axv"
!insertmacro AddWMPMimeTypes "application/ogg" "Ogg File" ".ogg" "ogg,oga,ogv,spx" ".ogg .oga .ogv .spx"
!insertmacro AddWMPMimeTypes "audio/flac" "FLAC Audio File" ".flac" "flac" ".flac"
!insertmacro AddWMPMimeTypes "audio/ogg" "Ogg Audio File" ".oga" "oga,spx" ".oga .spx"
!insertmacro AddWMPMimeTypes "video/ogg" "Ogg Video File" ".ogv" "ogv" ".ogv"
!insertmacro AddWMPMimeTypes "audio/annodex" "Annodex Audio File" ".axa" "axa" ".axa"
!insertmacro AddWMPMimeTypes "video/annodex" "Annodex Video File" ".axv" "axv" ".axv"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	WMP extension entries - 8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

!insertmacro AddWMPExtension ".anx" "Annodex" "application/annodex" "" ""
!insertmacro AddWMPExtension ".axa" "Annodex Audio" "audio/annodex" "audio" ""
!insertmacro AddWMPExtension ".axv" "Annodex Video" "video/annodex" "video" ""
!insertmacro AddWMPExtension ".flac" "FLAC Audio" "audio/flac" "audio" "WMP.FlacFile"
!insertmacro AddWMPExtension ".oga" "Ogg Audio" "audio/ogg" "audio" "WMP.OgaFile"
!insertmacro AddWMPExtension ".ogg" "Ogg Audio" "audio/ogg" "audio" "WMP.OggFile"
!insertmacro AddWMPExtension ".ogv" "Ogg Video" "video/ogg" "video" "WMP.OgvFile"
!insertmacro AddWMPExtension ".spx" "Speex Audio" "audio/ogg" "audio" "WMP.SpxFile"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	Directshow extension to filter mapping - 8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  WriteRegStr HKCR "Media Type\Extensions\.anx" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "Media Type\Extensions\.axa" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "Media Type\Extensions\.axv" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "Media Type\Extensions\.flac" "Source Filter" "{6DDA37BA-0553-499a-AE0D-BEBA67204548}"
  WriteRegStr HKCR "Media Type\Extensions\.oga" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "Media Type\Extensions\.ogg" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "Media Type\Extensions\.ogv" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
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
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	MLS Perceived type - 7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "ogv" "video"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "oga" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "axv" "video"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "axa" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "spx" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "flac" "audio"  
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "ogg" "audio"  
  
  ; Remove the previously set Media Player description
  !insertmacro RemoveMediaPlayerDesc "SOFTWARE\illiminable\oggcodecs"
  DeleteRegKey HKLM "SOFTWARE\illiminable\oggcodecs"
  DeleteRegKey /ifempty HKLM "SOFTWARE\illiminable"
   
  ; Get the next file description entry
  StrCpy $0 0
  StrCpy $2 0
  ${Do}
    EnumRegValue $1 HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\Descriptions" $0
    ${If} $1 != ""
      ${If} $1 > $2
        StrCpy $2 $1
      ${EndIf}
    ${EndIf}
    IntOp $0 $0 + 1
  ${LoopWhile} $1 != ""
  IntOp $2 $2 + 1

  ; Write new Media Player file description
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\Descriptions" $2 "Xiph.org Files (*.ogg;*.flac;*.ogv;*.oga;*.spx)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\MUIDescriptions" $2 "@$INSTDIR\wmpinfo.dll,-101"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\Types" $2 "*.ogg;*.flac;*.ogv;*.oga;*.spx"
   
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "MediaDescNum" $2
  
; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  !insertmacro MUI_STARTMENU_WRITE_END

; Check for Windows Media player
  Var /GLOBAL WMP_LOCATION  
  
;  ReadRegStr $WMP_LOCATION HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer" "Player.Path"
!if "$%X64%" == "true"
  StrCpy $WMP_LOCATION "$PROGRAMFILES64\Windows Media Player\wmplayer.exe"
!else
  StrCpy $WMP_LOCATION "$PROGRAMFILES\Windows Media Player\wmplayer.exe"
!endif
  
  IfFileExists  $WMP_LOCATION +3 0
  IfSilent +2
  MessageBox MB_OK|MB_ICONEXCLAMATION "A recognised version of Windows Media Player was not found. $\n File extenstion association must be done manually." IDOK 0

SectionEnd

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SectionGroup "File type associations" SEC_USE_WMP_FOR_OGG

${MementoSection} ".ogg"  SecOgg
  SectionIn 1

!if "$%X64%" == "true"
  SetRegView 64
!endif
  !insertmacro RegisterExtension ".ogg" "WMP.OggFile" "audio/ogg" "audio"
  !insertmacro RegisterWMPType "WMP.OggFile" "Ogg File"
${MementoSectionEnd}

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

${MementoSection} ".oga" SecOga
  SectionIn 1

!if "$%X64%" == "true"
  SetRegView 64
!endif

  !insertmacro RegisterExtension ".oga" "WMP.OgaFile" "audio/ogg" "audio"
  !insertmacro RegisterWMPType "WMP.OgaFile" "Ogg Audio File"
${MementoSectionEnd}

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

${MementoSection} ".ogv"  SecOgv
  SectionIn 1

!if "$%X64%" == "true"
  SetRegView 64
!endif

  !insertmacro RegisterExtension ".ogv" "WMP.OgvFile" "video/ogg" "video"
  !insertmacro RegisterWMPType "WMP.OgvFile" "Ogg Video File"
${MementoSectionEnd}

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

${MementoSection} ".spx"  SecSpx
  SectionIn 1

!if "$%X64%" == "true"
  SetRegView 64
!endif

  !insertmacro RegisterExtension ".spx" "WMP.SpxFile" "audio/ogg" "audio"
  !insertmacro RegisterWMPType "WMP.SpxFile" "Speex File"
${MementoSectionEnd}

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

${MementoSection} ".flac" SecFlac
  SectionIn 1

!if "$%X64%" == "true"
  SetRegView 64
!endif
 
  !insertmacro RegisterExtension ".flac" "WMP.FlacFile" "audio/flac" "audio"
  !insertmacro RegisterWMPType "WMP.FlacFile" "FLAC File"
${MementoSectionEnd}

SectionGroupEnd

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

${MementoUnselectedSection} "HTML5 <video> tag for Internet Explorer" SEC_VIDEO_TAG
  SectionIn 1 

!if "$%X64%" == "true"
  SetRegView 64
!endif

  !insertmacro RegisterCOM "$INSTDIR\AxPlayer.dll"
  
  ; Add AxPlayer XMLNamespace registry value
  WriteRegStr HKLM "SOFTWARE\Microsoft\Internet Explorer\XMLNamespace" "http://www.w3.org/1999/xhtml/video" "{7CC95AE6-C1FA-40CC-AB17-3E91DA2F77CA}"

${MementoSectionEnd}

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LangString DESC_OggCoreSection ${LANG_ENGLISH} "Core files for ${PRODUCT_NAME}"
LangString DESC_OggOpensInWMP ${LANG_ENGLISH} "Associates Ogg Files with Windows Media Player, so you can double click them in explorer."
LangString DESC_OggVideoTag ${LANG_ENGLISH} "Technical Preview! Add support for HTML5 <video> tag in Internet Explorer."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_CORE} $(DESC_OggCoreSection)
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_USE_WMP_FOR_OGG} $(DESC_OggOpensInWMP)
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_VIDEO_TAG} $(DESC_OggVideoTag)
!insertmacro MUI_FUNCTION_DESCRIPTION_END


;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Section -AdditionalIcons

!if "$%X64%" == "true"
  SetRegView 64
!endif

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  WriteIniStr "$SMPROGRAMS\$ICONS_GROUP\Website.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  !insertmacro MUI_STARTMENU_WRITE_END
  
  IfSilent +3
  Push $INSTDIR\Install.log
  Call DumpLog

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

!if "$%X64%" == "true"
  SetRegView 64
!endif

!ifndef INNER

  SetOutPath "$INSTDIR"
 
  ; this packages the signed uninstaller
 
  File "$%TEMP%\uninst.exe"
!endif  

  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR"
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

!if "$%X64%" == "true"
  SetRegView 64
!endif

  SetShellVarContext all

  SetDetailsPrint textonly
  DetailPrint "Unregistering DirectShow Filters ..."
  SetDetailsPrint listonly
  
  ; Unregister libraries - 15

  ; Unregister core annodex libraries

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
  
  !insertmacro UnRegisterCOM "$INSTDIR\AxPlayer.dll"

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
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/flac"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/ogg"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/ogg"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/annodex"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/annodex"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/annodex"


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
  
  ; Delete all the registered supported types from wmplayer.exe's list
  DeleteRegValue HKCR "Applications\wmplayer.exe\supportedtypes" ".ogg"
  DeleteRegValue HKCR "Applications\wmplayer.exe\supportedtypes" ".oga"
  DeleteRegValue HKCR "Applications\wmplayer.exe\supportedtypes" ".ogv"
  DeleteRegValue HKCR "Applications\wmplayer.exe\supportedtypes" ".spx"
  DeleteRegValue HKCR "Applications\wmplayer.exe\supportedtypes" ".flac"
  
  ; Delete the AxPlayer XMLNamespace registry value
  DeleteRegValue HKLM "SOFTWARE\Microsoft\Internet Explorer\XMLNamespace" "http://www.w3.org/1999/xhtml/video" 

  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
 
  SetDetailsPrint textonly
  DetailPrint "Deleting Files ..."
  SetDetailsPrint listonly 
  Delete "$INSTDIR\Install.log"


  ;Delete Filters - 15
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
  
  Delete "$INSTDIR\dsfOggDemux2.dll"
  Delete "$INSTDIR\dsfOggMux.dll"

  ; Delete "$INSTDIR\dsfSeeking.dll"
  

  Delete "$INSTDIR\dsfAnxMux.dll"
  ; Delete "$INSTDIR\dsfAnxDemux.dll"
  Delete "$INSTDIR\wmpinfo.dll"
  
  Delete "$INSTDIR\AxPlayer.dll"

  ; Delete text files - 7
  Delete "$INSTDIR\README"
  Delete "$INSTDIR\COPYRIGHTS.rtf"
  Delete "$INSTDIR\COPYRIGHTS"

  Delete "$INSTDIR\AUTHORS"
  Delete "$INSTDIR\HISTORY"
  Delete "$INSTDIR\ChangeLog.txt"
  Delete "$INSTDIR\Ogg Codecs.manifest" 

  ; Delete runtimes - 3
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

  ; Remove the previously set Media Player description
  !insertmacro RemoveMediaPlayerDesc "${PRODUCT_DIR_REGKEY}"
  
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  DeleteRegKey /ifempty HKLM "Software\${PRODUCT_PUBLISHER}"

  StrCmp $option_runFromInstaller "1" 0 +2
  SetAutoClose true
  
  SetDetailsPrint both
  
SectionEnd
!endif ; INNER