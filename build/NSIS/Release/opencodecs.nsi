;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; Copyright (C) 2005 - 2006 Zentaro Kavanagh 
; Copyright (C) 2008 - 2010 Cristian Adam
;
; NSIS install script for opencodecs
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!define PRODUCT_NAME "Open Codecs"

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

; Path from .nsi to opencodecs root
!define  OPENCODECS_ROOT_DIR "..\..\.."

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; All the code below needed to create a signed uninstaller
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!ifdef INNER
    !echo "Inner invocation"                  		; just to see what's going on
    OutFile "$%TEMP%\tempinstaller.exe"       	    ; not really important where this is
    SetCompress off                           		; for speed
  
!else
    !echo "Outer invocation"
 
    ; Call makensis again, defining INNER.  This writes an installer for us which, when
    ; it is invoked, will just write the uninstaller to some location, and then exit.
    ; Be sure to substitute the name of this script here.
 
    !system "$\"${NSISDIR}\makensis$\" /DINNER opencodecs.nsi" = 0
 
    ; So now run that installer we just created as %TEMP%\tempinstaller.exe.  Since it
    ; calls quit the return value isn't zero.
 
    !system "$%TEMP%\tempinstaller.exe" = 2
 
    ; That will have written an uninstaller binary for us.  Now we sign it with your
    ; favourite code signing tool.
 
    !system "signtool sign /a /t http://time.certum.pl/ $%TEMP%\uninst.exe" = 0
 
    ; Good.  Now we can carry on writing the real installer.
    OutFile "opencodecs_${PRODUCT_VERSION}.exe"
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
!include "macros.nsi"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${OPENCODECS_ROOT_DIR}\bin\xifish.ico"
!define MUI_UNICON "${OPENCODECS_ROOT_DIR}\bin\xifish.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "extra\header.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "extra\header_uninstall.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "extra\wizard.bmp"

!define MUI_COMPONENTSPAGE_SMALLDESC

VIProductVersion "${PRODUCT_VERSION}.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Directshow Filters for Ogg Vorbis, Speex, Theora, FLAC, and WebM"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "${PRODUCT_WEB_SITE}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (c) 2008 - 2010 ${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "The Xiph Fish Logo and the Vorbis.com many-fish logos are trademarks (tm) of ${PRODUCT_PUBLISHER}"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Welcome page
!define MUI_WELCOMEPAGE_TITLE "Welcome to the ${PRODUCT_NAME} ${PRODUCT_VERSION} Setup Wizard"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of Directshow Filters for Ogg Vorbis, Speex, Theora, FLAC, and WebM ${PRODUCT_VERSION}.$\r$\n$\r$\n${PRODUCT_PUBLISHER} is a collection of open source, multimedia-related projects. The most aggressive effort works to put the foundation standards of Internet audio and video into the public domain, where all Internet standards belong.$\r$\n$\r$\n$_CLICK"
!insertmacro MUI_PAGE_WELCOME
; License page
;!define MUI_LICENSEPAGE_CHECKBOX
!insertmacro MUI_PAGE_LICENSE "${OPENCODECS_ROOT_DIR}\COPYRIGHTS.rtf"
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

Name "${PRODUCT_PUBLISHER} ${PRODUCT_NAME} ${PRODUCT_VERSION}"
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
    
    ; Uninstall the oggcodecs package
    ${If} ${RunningX64}
        SetRegView 64
        Push "Ogg Codecs"
        Call RunUninstaller
        SetRegView 32
    ${EndIf}
    Push "Ogg Codecs"
    Call RunUninstaller
    
    Push "${PRODUCT_NAME}"
    Call RunUninstaller 
FunctionEnd

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Section "Open Codecs Core Files" SEC_CORE
    SectionIn 1 RO
  
    SetShellVarContext all
  
    SetOutPath "$INSTDIR"
    SetOverwrite on

    SetDetailsPrint textonly
    DetailPrint "Copying Files ..."
    SetDetailsPrint listonly
    
    ${If} ${RunningX64}
        CreateDirectory "$INSTDIR\x64"
    ${EndIf}
  
    ; Runtime libraries from visual studio
    ${AddVisualStudioRuntime}
  
    ; Icon files - 2
    File "${OPENCODECS_ROOT_DIR}\bin\xifish.ico"
    File "${OPENCODECS_ROOT_DIR}\bin\webm.ico"

    ; Text files - 7
    File "${OPENCODECS_ROOT_DIR}\README"
    File "${OPENCODECS_ROOT_DIR}\COPYRIGHTS.rtf"
    File "${OPENCODECS_ROOT_DIR}\COPYRIGHTS"

    File "${OPENCODECS_ROOT_DIR}\AUTHORS"
    File "${OPENCODECS_ROOT_DIR}\HISTORY"
    File "${OPENCODECS_ROOT_DIR}\ChangeLog.txt"
  
    ; Install Filters - 15  
    ${AddFile} "dsfFLACEncoder.dll"
    ${AddFile} "dsfSpeexEncoder.dll"
    ${AddFile} "dsfTheoraEncoder.dll"
    ${AddFile} "dsfVorbisEncoder.dll"
    ${AddFile} "vp8encoder.dll"
  
    ${AddFile} "dsfNativeFLACSource.dll"
    ${AddFile} "dsfSpeexDecoder.dll"
    ${AddFile} "dsfTheoraDecoder.dll"
    ${AddFile} "dsfFLACDecoder.dll"
    ${AddFile} "dsfVorbisDecoder.dll"
    ${AddFile} "vp8decoder.dll"

    ${AddFile} "dsfOggDemux2.dll"
    ${AddFile} "dsfOggMux.dll"
    ${AddFile} "webmsplit.dll"
    ${AddFile} "webmmux.dll"

    ${AddFile} "wmpinfo.dll"

    ; HTML <video> Tag Implementation
    ${AddFile} "AxPlayer.dll"

    SetDetailsPrint textonly
    DetailPrint "Registering DirectShow Filters ..."
    SetDetailsPrint listonly

    SetOutPath "$INSTDIR"

    ; Register libraries 
    ${RegisterCOM} "dsfFLACEncoder.dll"
    ${RegisterCOM} "dsfSpeexEncoder.dll" 
    ${RegisterCOM} "dsfTheoraEncoder.dll"
    ${RegisterCOM} "dsfVorbisEncoder.dll" 
    ${RegisterCOM} "vp8encoder.dll"
  
    ${RegisterCOM} "dsfNativeFLACSource.dll" 
    ${RegisterCOM} "dsfSpeexDecoder.dll" 
    ${RegisterCOM} "dsfTheoraDecoder.dll" 
    ${RegisterCOM} "dsfFLACDecoder.dll" 
    ${RegisterCOM} "dsfVorbisDecoder.dll" 
    ${RegisterCOM} "vp8decoder.dll"
    
    ${RegisterCOM} "dsfOggDemux2.dll" 
    ${RegisterCOM} "dsfOggMux.dll" 
    ${RegisterCOM} "webmsplit.dll"
    ${RegisterCOM} "webmmux.dll"
   
    SetDetailsPrint textonly
    DetailPrint "Writing Registry Entries ..."
    SetDetailsPrint listonly

    ; Media Group Entries - 7
    ${AddWmpMediaGroup} ".flac" "FLAC" "FLAC File (flac)" "audio/flac" "Audio"
    ${AddWmpMediaGroup} ".oga" "OGA" "Ogg Audio File (oga)" "audio/ogg" "Audio"
    ${AddWmpMediaGroup} ".ogg" "OGG" "Ogg Audio File (ogg)" "audio/ogg" "Audio"
    ${AddWmpMediaGroup} ".ogv" "OGV" "Ogg Video File (ogv)" "video/ogg" "Video"
    ${AddWmpMediaGroup} ".spx" "SPX" "Speex File (spx)" "audio/ogg" "Audio"
    ${AddWmpMediaGroup} ".weba" "WEBA" "WebM Audio File (weba)" "audio/webm" "Audio"
    ${AddWmpMediaGroup} ".webm" "WEBM" "WebM Video File (webm)" "video/webm" "Video"

    ; WMP Mime type entries - 6
    ${AddWmpMimeType} "application/ogg" "Ogg File" ".ogg" "ogg,oga,ogv,spx" ".ogg .oga .ogv .spx"
    ${AddWmpMimeType} "audio/flac" "FLAC Audio File" ".flac" "flac" ".flac"
    ${AddWmpMimeType} "audio/ogg" "Ogg Audio File" ".oga" "ogg,oga,spx" ".ogg .oga .spx"
    ${AddWmpMimeType} "video/ogg" "Ogg Video File" ".ogv" "ogv" ".ogv"
    ${AddWmpMimeType} "audio/webm" "WebM Audio File" ".weba" "weba" ".weba"
    ${AddWmpMimeType} "video/webm" "WebM Video File" ".webm" "webm" ".webm"

    ; WMP extension entries - 7
    ${AddWmpExtension} ".flac" "FLAC Audio" "audio/flac" "audio" "WMP.FlacFile" "xifish.ico"
    ${AddWmpExtension} ".oga" "Ogg Audio" "audio/ogg" "audio" "WMP.OgaFile" "xifish.ico"
    ${AddWmpExtension} ".ogg" "Ogg Audio" "audio/ogg" "audio" "WMP.OggFile" "xifish.ico"
    ${AddWmpExtension} ".ogv" "Ogg Video" "video/ogg" "video" "WMP.OgvFile" "xifish.ico"
    ${AddWmpExtension} ".spx" "Speex Audio" "audio/ogg" "audio" "WMP.SpxFile" "xifish.ico"
    ${AddWmpExtension} ".weba" "WebM Audio" "audio/webm" "audio" "WMP.WebaFile" "webm.ico"
    ${AddWmpExtension} ".webm" "WebM Video" "video/webm" "video" "WMP.WebmFile" "webm.ico"

    ; MIME type entries  - 6
    ${AddMimeType} "application/ogg" ".ogx"
    ${AddMimeType} "audio/flac" ".flac"
    ${AddMimeType} "audio/ogg" ".ogg"
    ${AddMimeType} "video/ogg" ".ogv"
    ${AddMimeType} "audio/webm" ".weba"
    ${AddMimeType} "video/webm" ".webm"

    ; Directshow extension to filter mapping - 7
    ; Mapped to File Source (Async.), except for FLAC
    ${AddMediaTypeExtensionSource} ".flac" "{6DDA37BA-0553-499a-AE0D-BEBA67204548}"
    ${AddMediaTypeExtensionSource} ".oga" "{E436EBB5-524F-11CE-9F53-0020AF0BA770}"
    ${AddMediaTypeExtensionSource} ".ogg" "{E436EBB5-524F-11CE-9F53-0020AF0BA770}"
    ${AddMediaTypeExtensionSource} ".ogv" "{E436EBB5-524F-11CE-9F53-0020AF0BA770}"
    ${AddMediaTypeExtensionSource} ".spx" "{E436EBB5-524F-11CE-9F53-0020AF0BA770}"
    ${AddMediaTypeExtensionSource} ".weba" "{E436EBB5-524F-11CE-9F53-0020AF0BA770}"
    ${AddMediaTypeExtensionSource} ".webm" "{E436EBB5-524F-11CE-9F53-0020AF0BA770}"

    ; Remove a faulty http extension added by mistake
    ${DeleteHttpExtensionSource} "${TypeExt}"
    ; Directshow extension to filter mapping for HTTP  - 6
    ; Mapped to File Source (URL)
    ${AddHttpExtensionSource} ".OGA" "{E436EBB6-524F-11CE-9F53-0020AF0BA770}"
    ${AddHttpExtensionSource} ".OGG" "{E436EBB6-524F-11CE-9F53-0020AF0BA770}"
    ${AddHttpExtensionSource} ".OGV" "{E436EBB6-524F-11CE-9F53-0020AF0BA770}"
    ${AddHttpExtensionSource} ".SPX" "{E436EBB6-524F-11CE-9F53-0020AF0BA770}"
    ${AddHttpExtensionSource} ".WEBA" "{E436EBB6-524F-11CE-9F53-0020AF0BA770}"
    ${AddHttpExtensionSource} ".WEBM" "{E436EBB6-524F-11CE-9F53-0020AF0BA770}"
      
    ; Add the "OggS" recognition pattern
    ${AddOggRecognitionPattern}
    
    ; Add "EBML" recognition pattern
    ${AddWebmRecognitionPattern}

    ; MLS Perceived type - 7
    ${AddMediaPlayerMlsExtension} "flac" "audio"  
    ${AddMediaPlayerMlsExtension} "oga" "audio"
    ${AddMediaPlayerMlsExtension} "ogg" "audio"  
    ${AddMediaPlayerMlsExtension} "ogv" "video"
    ${AddMediaPlayerMlsExtension} "spx" "audio"
    ${AddMediaPlayerMlsExtension} "weba" "audio"
    ${AddMediaPlayerMlsExtension} "webm" "video"  
      
    ; Remove the previously set Media Player description
    ${RemoveMediaPlayerDesc} "SOFTWARE\illiminable\oggcodecs"
    DeleteRegKey HKLM "SOFTWARE\illiminable\oggcodecs"
    DeleteRegKey /ifempty HKLM "SOFTWARE\illiminable"
       
    ${AddMediaPlayerDesc} "Open Codecs Files (*.ogg;*.flac;*.ogv;*.oga;*.spx;*.webm;*.weba)" "*.ogg;*.flac;*.ogv;*.oga;*.spx;*.webm;*.weba"
      
    ${CheckWindowsMediaPlayer}
      
    ; Shortcuts
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    !insertmacro MUI_STARTMENU_WRITE_END
  
SectionEnd

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SectionGroup "File type associations" SEC_USE_WMP_FOR_OGG

${MementoSection} ".ogg"  SecOgg
    SectionIn 1
    ${RegisterExtension} ".ogg" "WMP.OggFile" "audio/ogg" "audio"
    ${RegisterWmpType} "WMP.OggFile" "Ogg File" "xifish.ico"
${MementoSectionEnd}

${MementoSection} ".oga" SecOga
    SectionIn 1
    ${RegisterExtension} ".oga" "WMP.OgaFile" "audio/ogg" "audio"
    ${RegisterWmpType} "WMP.OgaFile" "Ogg Audio File" "xifish.ico"
${MementoSectionEnd}

${MementoSection} ".ogv"  SecOgv
    SectionIn 1
    ${RegisterExtension} ".ogv" "WMP.OgvFile" "video/ogg" "video"
    ${RegisterWmpType} "WMP.OgvFile" "Ogg Video File" "xifish.ico"
${MementoSectionEnd}

${MementoSection} ".spx"  SecSpx
    SectionIn 1
    ${RegisterExtension} ".spx" "WMP.SpxFile" "audio/ogg" "audio"
    ${RegisterWmpType} "WMP.SpxFile" "Speex File" "xifish.ico"
${MementoSectionEnd}

${MementoSection} ".flac" SecFlac
    SectionIn 1
    ${RegisterExtension} ".flac" "WMP.FlacFile" "audio/flac" "audio"
    ${RegisterWmpType} "WMP.FlacFile" "FLAC File" "xifish.ico"
${MementoSectionEnd}

${MementoSection} ".weba" SecWeba
    SectionIn 1
    ${RegisterExtension} ".weba" "WMP.WebaFile" "audio/webm" "audio"
    ${RegisterWmpType} "WMP.WebaFile" "WebM Audio File" "webm.ico"
${MementoSectionEnd}

${MementoSection} ".webm" SecWebm
    SectionIn 1
    ${RegisterExtension} ".webm" "WMP.WebmFile" "video/webm" "video"
    ${RegisterWmpType} "WMP.WebmFile" "WebM Video File" "webm.ico"
${MementoSectionEnd}

SectionGroupEnd

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

${MementoUnselectedSection} "HTML5 <video> tag for Internet Explorer" SEC_VIDEO_TAG
    SectionIn 1 
    ${RegisterCOM} "AxPlayer.dll"
    ; Add AxPlayer XMLNamespace registry value
    ${RegisterAxPlayerXmlNamespace}
    ${RegisterUserAgentString} "${PRODUCT_NAME} ${PRODUCT_VERSION}"
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
  
    ; Unregister libraries - 15
    ${UnRegisterCOM} "dsfOggDemux2.dll"
    ${UnRegisterCOM} "dsfOggMux.dll"
    ${UnRegisterCOM} "webmsplit.dll"
    ${UnRegisterCOM} "webmmux.dll"

    ${UnRegisterCOM} "dsfFLACEncoder.dll"
    ${UnRegisterCOM} "dsfSpeexEncoder.dll"
    ${UnRegisterCOM} "dsfTheoraEncoder.dll"
    ${UnRegisterCOM} "dsfVorbisEncoder.dll"
    ${UnRegisterCOM} "vp8encoder.dll"

    ${UnRegisterCOM} "dsfNativeFLACSource.dll"
    ${UnRegisterCOM} "dsfSpeexDecoder.dll"
    ${UnRegisterCOM} "dsfTheoraDecoder.dll"
    ${UnRegisterCOM} "dsfFLACDecoder.dll"
    ${UnRegisterCOM} "dsfVorbisDecoder.dll"
    ${UnRegisterCOM} "vp8decoder.dll"

    ${UnRegisterCOM} "AxPlayer.dll"

    SetDetailsPrint textonly
    DetailPrint "Deleting Registry Entries ..."
    SetDetailsPrint listonly
    
    ; Get rid of all the registry keys we made for directshow and WMP

    ; Media Type Groups entries - 7
    ${DeleteWmpMediaGroup} "FLAC" "Audio"
    ${DeleteWmpMediaGroup} "OGA" "Audio"
    ${DeleteWmpMediaGroup} "OGG" "Audio"
    ${DeleteWmpMediaGroup} "OGV" "Video"
    ${DeleteWmpMediaGroup} "SPX" "Audio"
    ${DeleteWmpMediaGroup} "WEBA" "Audio"
    ${DeleteWmpMediaGroup} "WEBM" "Video"

    ; WMP MIME Type entries  - 6
    ${DeleteWmpMimeType} "application/ogg"
    ${DeleteWmpMimeType} "audio/flac"
    ${DeleteWmpMimeType} "audio/ogg"
    ${DeleteWmpMimeType} "video/ogg"
    ${DeleteWmpMimeType} "audio/webm"
    ${DeleteWmpMimeType} "video/webm"

    ; MIME Type entries - 6
    ${DeleteMimeType} "application/ogg"
    ${DeleteMimeType} "audio/flac"
    ${DeleteMimeType} "audio/ogg"
    ${DeleteMimeType} "video/ogg"
    ${DeleteMimeType} "audio/webm"
    ${DeleteMimeType} "video/webm"

    ; File Extension Entries - 7
    ${DeleteWmpExtension} ".flac"
    ${DeleteWmpExtension} ".oga"
    ${DeleteWmpExtension} ".ogg"
    ${DeleteWmpExtension} ".ogv"
    ${DeleteWmpExtension} ".spx"
    ${DeleteWmpExtension} ".weba"
    ${DeleteWmpExtension} ".webm"

    ; Extension to filter mapping - 7
    ${DeleteMediaTypeExtension} ".flac"
    ${DeleteMediaTypeExtension} ".oga"
    ${DeleteMediaTypeExtension} ".ogg"
    ${DeleteMediaTypeExtension} ".ogv"
    ${DeleteMediaTypeExtension} ".spx"
    ${DeleteMediaTypeExtension} ".weba"
    ${DeleteMediaTypeExtension} ".webm"

    ; Extension to filter mapping for http - 6
    ${DeleteHttpExtensionSource} ".OGA"
    ${DeleteHttpExtensionSource} ".OGG"
    ${DeleteHttpExtensionSource} ".OGV"
    ${DeleteHttpExtensionSource} ".SPX"
    ${DeleteHttpExtensionSource} ".WEBA"
    ${DeleteHttpExtensionSource} ".WEBM"
    ; TODO: FLAC

    ; MLS Perceived type - 7
    ${DeleteMediaPlayerMlsExtension} "flac"  
    ${DeleteMediaPlayerMlsExtension} "oga"
    ${DeleteMediaPlayerMlsExtension} "ogg"
    ${DeleteMediaPlayerMlsExtension} "ogv"
    ${DeleteMediaPlayerMlsExtension} "spx"
    ${DeleteMediaPlayerMlsExtension} "weba"
    ${DeleteMediaPlayerMlsExtension} "webm"

    ; Point the extension to the handlers - 7
    ${UnRegisterWmpType} "WMP.OggFile"
    ${UnRegisterWmpType} "WMP.OgaFile"
    ${UnRegisterWmpType} "WMP.OgvFile"
    ${UnRegisterWmpType} "WMP.SpxFile"
    ${UnRegisterWmpType} "WMP.FlacFile"
    ${UnRegisterWmpType} "WMP.WebaFile"
    ${UnRegisterWmpType} "WMP.WebmFile"

    ; Delete all the registered supported types from wmplayer.exe's list - 7
    ${UnRegisterExtension} ".ogg"
    ${UnRegisterExtension} ".oga"
    ${UnRegisterExtension} ".ogv"
    ${UnRegisterExtension} ".spx"
    ${UnRegisterExtension} ".flac"
    ${UnRegisterExtension} ".weba"
    ${UnRegisterExtension} ".webm"

    ; Delete the "OggS" regonition pattern
    ${DeleteOggRecognitionPattern}

    ; Delete "EBML" recognition pattern
    ${DeleteWebmRecognitionPattern}

    ; Delete the AxPlayer XMLNamespace registry value
    ${UnRegisterAxPlayerXmlNamespace}
    ${UnRegisterUserAgentString} "${PRODUCT_NAME} ${PRODUCT_VERSION}"

    !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP

    SetDetailsPrint textonly
    DetailPrint "Deleting Files ..."
    SetDetailsPrint listonly 
    Delete "$INSTDIR\Install.log"

    ;Delete Filters - 15
    ${DeleteFile} "dsfVorbisEncoder.dll"
    ${DeleteFile} "dsfTheoraEncoder.dll"
    ${DeleteFile} "dsfSpeexEncoder.dll"
    ${DeleteFile} "dsfFLACEncoder.dll"
    ${DeleteFile} "vp8encoder.dll"

    ${DeleteFile} "dsfVorbisDecoder.dll"
    ${DeleteFile} "dsfFLACDecoder.dll"
    ${DeleteFile} "dsfNativeFLACSource.dll"
    ${DeleteFile} "dsfTheoraDecoder.dll"
    ${DeleteFile} "dsfSpeexDecoder.dll"
    ${DeleteFile} "vp8decoder.dll"

    ${DeleteFile} "dsfOggDemux2.dll"
    ${DeleteFile} "dsfOggMux.dll"
    ${DeleteFile} "webmsplit.dll"
    ${DeleteFile} "webmmux.dll"

    ${DeleteFile} "wmpinfo.dll"
    
    ${DeleteFile} "AxPlayer.dll"

    ; Delete text files - 6
    Delete "$INSTDIR\README"
    Delete "$INSTDIR\COPYRIGHTS.rtf"
    Delete "$INSTDIR\COPYRIGHTS"

    Delete "$INSTDIR\AUTHORS"
    Delete "$INSTDIR\HISTORY"
    Delete "$INSTDIR\ChangeLog.txt"

    ; Delete runtimes
    ${DeleteVisualStudioRuntime}

    ; Delete icons - 2
    Delete "$INSTDIR\xifish.ico"
    Delete "$INSTDIR\webm.ico"

    ;Delete accesory files, links etc.
    Delete "$SMPROGRAMS\$ICONS_GROUP\Website.url"
    Delete "$INSTDIR\uninst.exe"

    RMDir "$SMPROGRAMS\$ICONS_GROUP"
    ; Remove the start menu group (but only if it's empty)
    RMDir "$SMPROGRAMS\$ICONS_GROUP\.."

    ; Need to change the working directory to something else (anything) besides
    ; the output directory, so we can rmdir it
    SetOutPath "$TEMP"
    ${If} ${RunningX64}
        RMDir "$INSTDIR\x64"
    ${EndIf}
    RMDir "$INSTDIR"

    ; Remove the parent directory (but only if it's empty)
    RMDir "$INSTDIR\.."

    ; Remove the previously set Media Player description
    ${RemoveMediaPlayerDesc} "${PRODUCT_DIR_REGKEY}"

    DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
    DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
    DeleteRegKey /ifempty HKLM "Software\${PRODUCT_PUBLISHER}"

    StrCmp $option_runFromInstaller "1" 0 +2
    SetAutoClose true

    SetDetailsPrint both
  
SectionEnd
!endif ; INNER
