;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; Copyright (C) 2008 - 2010 Cristian Adam
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; Windows Media Player location
Var /GLOBAL WMP_LOCATION_WIN32
Var /GLOBAL WMP_LOCATION_X64

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; Location of Visual Studio runtime libraries on the compiling system
!if "$%COMPILER%" == "VS2008"
    !define VS_RUNTIME_LOCATION_X64 "d:\Program Files\Microsoft Visual Studio 9.0\VC\redist\amd64\Microsoft.VC90.CRT\"
    !define VS_RUNTIME_LOCATION_WIN32 "d:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\"
    !define VS_RUNTIME_SUFFIX 90
!else if "$%COMPILER%" == "VS2005"
    !define VS_RUNTIME_LOCATION_X64 "d:\Program Files\Microsoft Visual Studio 8\VC\redist\amd64\Microsoft.VC80.CRT\"
    !define VS_RUNTIME_LOCATION_WIN32 "d:\Program Files\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT\"	
    !define VS_RUNTIME_SUFFIX 80
!endif

!define VS_RUNTIME_PREFIX msvc

!macro AddVisualStudioRuntime 
    File "${VS_RUNTIME_LOCATION_WIN32}\${VS_RUNTIME_PREFIX}r${VS_RUNTIME_SUFFIX}.dll"
    File "${VS_RUNTIME_LOCATION_WIN32}\${VS_RUNTIME_PREFIX}p${VS_RUNTIME_SUFFIX}.dll"
    File "${VS_RUNTIME_LOCATION_WIN32}\Microsoft.VC${VS_RUNTIME_SUFFIX}.CRT.manifest"

   ${If} ${RunningX64}
        File "/oname=x64\${VS_RUNTIME_PREFIX}r${VS_RUNTIME_SUFFIX}.dll" "${VS_RUNTIME_LOCATION_X64}\${VS_RUNTIME_PREFIX}r${VS_RUNTIME_SUFFIX}.dll" 
        File "/oname=x64\${VS_RUNTIME_PREFIX}p${VS_RUNTIME_SUFFIX}.dll" "${VS_RUNTIME_LOCATION_X64}\${VS_RUNTIME_PREFIX}p${VS_RUNTIME_SUFFIX}.dll"
        File "/oname=x64\Microsoft.VC${VS_RUNTIME_SUFFIX}.CRT.manifest" "${VS_RUNTIME_LOCATION_X64}\Microsoft.VC${VS_RUNTIME_SUFFIX}.CRT.manifest"
   ${EndIf}
!macroend
!define AddVisualStudioRuntime "!insertmacro AddVisualStudioRuntime"

!macro DeleteVisualStudioRuntime
    Delete "$INSTDIR\${VS_RUNTIME_PREFIX}r${VS_RUNTIME_SUFFIX}.dll"
    Delete "$INSTDIR\${VS_RUNTIME_PREFIX}p${VS_RUNTIME_SUFFIX}.dll"
    Delete "$INSTDIR\Microsoft.VC${VS_RUNTIME_SUFFIX}.CRT.manifest"
  
   ${If} ${RunningX64}
        Delete "$INSTDIR\x64\${VS_RUNTIME_PREFIX}r${VS_RUNTIME_SUFFIX}.dll"
        Delete "$INSTDIR\x64\${VS_RUNTIME_PREFIX}p${VS_RUNTIME_SUFFIX}.dll"
        Delete "$INSTDIR\x64\Microsoft.VC${VS_RUNTIME_SUFFIX}.CRT.manifest"  
    ${EndIf}
!macroend
!define DeleteVisualStudioRuntime "!insertmacro DeleteVisualStudioRuntime"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; COM registration macros, with fallbacks on regsvr32
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
!macro RegisterCOM file
    !define LIBRARY_COM
    RegDLL "$INSTDIR\${file}"
    !undef LIBRARY_COM
    IfErrors 0 +2
       ExecWait '$SYSDIR\regsvr32.exe /s "$INSTDIR\${file}"'
            
    ${If} ${RunningX64}
        ${EnableX64FSRedirection}
        ExecWait '$SYSDIR\regsvr32.exe /s "$INSTDIR\x64\${file}"'
        ${DisableX64FSRedirection}
    ${EndIf}
!macroend
!define RegisterCOM "!insertmacro RegisterCOM"

!macro UnRegisterCOM file
    !define LIBRARY_COM
    UnRegDLL "$INSTDIR\${file}"
    !undef LIBRARY_COM
    IfErrors 0 +2
        ExecWait '$SYSDIR\regsvr32.exe /u /s "$INSTDIR\${file}"'

    ${If} ${RunningX64}
        ${EnableX64FSRedirection}
        ExecWait '$SYSDIR\regsvr32.exe /u /s "$INSTDIR\x64\${file}"'
        ${DisableX64FSRedirection}
    ${EndIf}
!macroend
!define UnRegisterCOM "!insertmacro UnRegisterCOM"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
; Windows Media Player type registraton macro
;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro RegisterWmpType_Internal typeName description WmpLocation IcoFile

    WriteRegStr HKCR "${typeName}" "" "${description}"
    WriteRegStr HKCR "${typeName}\shell" "" "play"

    WriteRegStr HKCR "${typeName}\shell\open" "" "&Open"
    WriteRegStr HKCR "${typeName}\shell\open\command" "" "$\"${WmpLocation}$\" /Open $\"%L$\""
  
    WriteRegStr HKCR "${typeName}\shell\play" "" "&Play"
    WriteRegStr HKCR "${typeName}\shell\play\command" "" "$\"${WmpLocation}$\" /Play $\"%L$\""    

    ${If} ${AtMostWinVista}
        ; WMP extra integration
        WriteRegStr HKCR "${typeName}\shellex\ContextMenuHandlers\WMPAddToPlaylist" "" "{F1B9284F-E9DC-4e68-9D7E-42362A59F0FD}"
        WriteRegStr HKCR "${typeName}\shellex\ContextMenuHandlers\WMPPlayAsPlaylist" "" "{CE3FB1D1-02AE-4a5f-A6E9-D9F1B4073E6C}"
    ${EndIf}

    WriteRegStr HKCR "${typeName}\DefaultIcon" "" "$INSTDIR\${IcoFile}"
!macroend

!macro RegisterWmpType typeName description IcoFile
    ${If} ${RunningX64}
        SetRegView 64
        !insertmacro RegisterWmpType_Internal "${typeName}" "${description}" "$WMP_LOCATION_X64" "${IcoFile}"
    ${EndIf}
    SetRegView 32
    !insertmacro RegisterWmpType_Internal "${typeName}" "${description}" "$WMP_LOCATION_WIN32" "${IcoFile}"
!macroend
!define RegisterWmpType "!insertmacro RegisterWmpType"

!macro UnRegisterWmpType typeName
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegKey HKCR "${typeName}"
    ${EndIf}
    SetRegView 32
    DeleteRegKey HKCR "${typeName}"
!macroend
!define UnRegisterWmpType "!insertmacro UnRegisterWmpType"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro RegisterExtension_Internal Ext WMPType MimeType PerceivedType
    WriteRegStr HKCR "${Ext}" "" "${WMPType}"
    WriteRegStr HKCR "${Ext}" "Content Type" "${MimeType}"
    WriteRegStr HKCR "${Ext}" "PerceivedType" "${PerceivedType}"
    WriteRegStr HKCR "Applications\wmplayer.exe\supportedtypes" "${Ext}" ""
!macroend

!macro RegisterExtension Ext WMPType MimeType PerceivedType
    ${If} ${RunningX64}
        SetRegView 64
        !insertmacro RegisterExtension_Internal "${Ext}" "${WMPType}" "${MimeType}" "${PerceivedType}"
    ${EndIf}
    SetRegView 32
    !insertmacro RegisterExtension_Internal "${Ext}" "${WMPType}" "${MimeType}" "${PerceivedType}"
!macroend
!define RegisterExtension "!insertmacro RegisterExtension"

!macro UnRegisterExtension TypeExt
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegValue HKCR "Applications\wmplayer.exe\supportedtypes" "${TypeExt}"
    ${EndIf}
    SetRegView 32
    DeleteRegValue HKCR "Applications\wmplayer.exe\supportedtypes" "${TypeExt}"
!macroend
!define UnRegisterExtension "!insertmacro UnRegisterExtension"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro AddWmpExtension_Internal TypeExt TypeDesc MimeType PerceivedType ExtensionHandler IcoFile
    WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "MediaType.Description" "${TypeDesc}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "MediaType.Icon" "$INSTDIR\${IcoFile}"
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "Permissions" 0x0000000f
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "Runtime" 0x00000007
    ${If} "${PerceivedType}" != ""  
        WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "PerceivedType" "${PerceivedType}"
    ${EndIf}
    WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "Extension.MIME" "${MimeType}"
    ${if} "${ExtensionHandler}" != ""
        WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}" "Extension.Handler" "${ExtensionHandler}"
    ${EndIf}
!macroend 

!macro AddWmpExtension TypeExt TypeDesc MimeType PerceivedType ExtensionHandler IcoFile
    ${If} ${RunningX64}
        SetRegView 64
        !insertmacro AddWmpExtension_Internal "${TypeExt}" "${TypeDesc}" "${MimeType}" "${PerceivedType}" "${ExtensionHandler}" "${IcoFile}"
    ${EndIf}
    SetRegView 32
    !insertmacro AddWmpExtension_Internal "${TypeExt}" "${TypeDesc}" "${MimeType}" "${PerceivedType}" "${ExtensionHandler}" "${IcoFile}"
!macroend
!define AddWmpExtension "!insertmacro AddWmpExtension"

!macro DeleteWmpExtension TypeExt
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}"
    ${EndIf}
    SetRegView 32
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\${TypeExt}"
!macroend
!define DeleteWmpExtension "!insertmacro DeleteWmpExtension"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro AddMediaTypeExtensionSource TypeExt SourceFilterClsid
    ${If} ${RunningX64}
        SetRegView 64
        WriteRegStr HKCR "Media Type\Extensions\${TypeExt}" "Source Filter" "${SourceFilterClsid}"
    ${EndIf}
    SetRegView 32
    WriteRegStr HKCR "Media Type\Extensions\${TypeExt}" "Source Filter" "${SourceFilterClsid}"
!macroend
!define AddMediaTypeExtensionSource "!insertmacro AddMediaTypeExtensionSource"

!macro DeleteMediaTypeExtension TypeExt
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegKey HKCR "Media Type\Extensions\${TypeExt}"
    ${EndIf}
    SetRegView 32
    DeleteRegKey HKCR "Media Type\Extensions\${TypeExt}"
!macroend
!define DeleteMediaTypeExtension "!insertmacro DeleteMediaTypeExtension"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro AddHttpExtensionSource TypeExt SourceFilterClsid
    ${If} ${RunningX64}
        SetRegView 64
        WriteRegStr HKCR "http\Extensions" "${TypeExt}" "${SourceFilterClsid}"
    ${EndIf}
    SetRegView 32
    WriteRegStr HKCR "http\Extensions" "${TypeExt}" "${SourceFilterClsid}"
!macroend
!define AddHttpExtensionSource "!insertmacro AddHttpExtensionSource"

!macro DeleteHttpExtensionSource TypeExt 
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegValue HKCR "http\Extensions" "${TypeExt}"
    ${EndIf}
    SetRegView 32
    DeleteRegValue HKCR "http\Extensions" "${TypeExt}"
!macroend
!define DeleteHttpExtensionSource "!insertmacro DeleteHttpExtensionSource"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro AddMimeType MimeType TypeExt 
    ${If} ${RunningX64}
        SetRegView 64
        WriteRegStr HKCR "MIME\DataBase\Content Type\${MimeType}" "Extension" "${TypeExt}"  
    ${EndIf}
    SetRegView 32
    WriteRegStr HKCR "MIME\DataBase\Content Type\${MimeType}" "Extension" "${TypeExt}"  
!macroend
!define AddMimeType "!insertmacro AddMimeType"

!macro DeleteMimeType MimeType
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegKey HKCR "MIME\DataBase\Content Type\${MimeType}" 
    ${EndIf}
    SetRegView 32
    DeleteRegKey HKCR "MIME\DataBase\Content Type\${MimeType}" 
!macroend
!define DeleteMimeType "!insertmacro DeleteMimeType"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
!macro AddMediaPlayerMlsExtension TypeExt MediaType
    ${If} ${RunningX64}
        SetRegView 64
        WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "${TypeExt}" "${MediaType}"
    ${EndIf} 
    SetRegView 32
    WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "${TypeExt}" "${MediaType}"
!macroend
!define AddMediaPlayerMlsExtension "!insertmacro AddMediaPlayerMlsExtension"

!macro DeleteMediaPlayerMlsExtension TypeExt
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "${TypeExt}"
    ${EndIf}
    SetRegView 32
    DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "${TypeExt}"
!macroend
!define DeleteMediaPlayerMlsExtension "!insertmacro DeleteMediaPlayerMlsExtension"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro AddWmpMediaGroup_Internal TypeExt TypeUppercase TypeDesc MimeType PerceivedType
    WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\${PerceivedType}\${TypeUppercase}" "" "${TypeDesc}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\${PerceivedType}\${TypeUppercase}" "Extensions" "${TypeExt}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\${PerceivedType}\${TypeUppercase}" "MIME Types" "${MimeType}"
!macroend

!macro AddWmpMediaGroup TypeExt TypeUppercase TypeDesc MimeType PerceivedType
    ${If} ${RunningX64}
        SetRegView 64
        !insertmacro AddWmpMediaGroup_Internal "${TypeExt}" "${TypeUppercase}" "${TypeDesc}" "${MimeType}" "${PerceivedType}"
    ${EndIf}
    SetRegView 32
    !insertmacro AddWmpMediaGroup_Internal "${TypeExt}" "${TypeUppercase}" "${TypeDesc}" "${MimeType}" "${PerceivedType}"
!macroend
!define AddWmpMediaGroup "!insertmacro AddWmpMediaGroup"

!macro DeleteWmpMediaGroup TypeUppercase PerceivedType
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\${PerceivedType}\${TypeUppercase}"  
    ${EndIf}
    SetRegView 32
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\${PerceivedType}\${TypeUppercase}"  
!macroend
!define DeleteWmpMediaGroup "!insertmacro DeleteWmpMediaGroup"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro AddOggRecognitionPattern_Internal
  WriteRegStr HKCR "Media Type\{E436EB83-524F-11CE-9F53-0020AF0BA770}\{DD142C1E-0C1E-4381-A24E-0B2D80B6098A}" "0" "0,4,,4F676753"
  WriteRegStr HKCR "Media Type\{E436EB83-524F-11CE-9F53-0020AF0BA770}\{DD142C1E-0C1E-4381-A24E-0B2D80B6098A}" "Source Filter" "{E436EBB5-524F-11CE-9F53-0020AF0BA770}";
!macroend

!macro AddOggRecognitionPattern
    ${If} ${RunningX64}
        SetRegView 64
        !insertmacro AddOggRecognitionPattern_Internal
    ${EndIf}
    SetRegView 32
    !insertmacro AddOggRecognitionPattern_Internal
!macroend
!define AddOggRecognitionPattern "!insertmacro AddOggRecognitionPattern"

!macro DeleteOggRecognitionPattern
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegKey HKCR "Media Type\{E436EB83-524F-11CE-9F53-0020AF0BA770}\{DD142C1E-0C1E-4381-A24E-0B2D80B6098A}"
    ${EndIf}
    SetRegView 32
    DeleteRegKey HKCR "Media Type\{E436EB83-524F-11CE-9F53-0020AF0BA770}\{DD142C1E-0C1E-4381-A24E-0B2D80B6098A}"
!macroend
!define DeleteOggRecognitionPattern "!insertmacro DeleteOggRecognitionPattern"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro AddWebmRecognitionPattern_Internal
  WriteRegStr HKCR "Media Type\{E436EB83-524F-11CE-9F53-0020AF0BA770}\{ED3110EB-5211-11DF-94AF-0026B977EEAA}" "0" "0,4,,1A45DFA3"
  WriteRegStr HKCR "Media Type\{E436EB83-524F-11CE-9F53-0020AF0BA770}\{ED3110EB-5211-11DF-94AF-0026B977EEAA}" "Source Filter" "{E436EBB5-524F-11CE-9F53-0020AF0BA770}";
!macroend

!macro AddWebmRecognitionPattern
    ${If} ${RunningX64}
        SetRegView 64
        !insertmacro AddWebmRecognitionPattern_Internal
    ${EndIf}
    SetRegView 32
    !insertmacro AddWebmRecognitionPattern_Internal
!macroend
!define AddWebmRecognitionPattern "!insertmacro AddWebmRecognitionPattern"

!macro DeleteWebmRecognitionPattern
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegKey HKCR "Media Type\{E436EB83-524F-11CE-9F53-0020AF0BA770}\{ED3110F8-5211-11DF-94AF-0026B977EEAA}"
    ${EndIf}
    SetRegView 32
    DeleteRegKey HKCR "Media Type\{E436EB83-524F-11CE-9F53-0020AF0BA770}\{ED3110F8-5211-11DF-94AF-0026B977EEAA}"
!macroend
!define DeleteWebmRecognitionPattern "!insertmacro DeleteWebmRecognitionPattern"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro AddWmpMimeType_Internal MimeType TypeDesc TypeExt TypeExtComma TypeExtSpace
    WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\${MimeType}" "" "${TypeDesc}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\${MimeType}" "Extension.Key" "${TypeExt}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\${MimeType}" "Extensions.CommaSep" "${TypeExtComma}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\${MimeType}" "Extensions.SpaceSep" "${TypeExtSpace}"
!macroend

!macro AddWmpMimeType MimeType TypeDesc TypeExt TypeExtComma TypeExtSpace
    ${If} ${RunningX64}
        SetRegView 64
        !insertmacro AddWmpMimeType_Internal "${MimeType}" "${TypeDesc}" "${TypeExt}" "${TypeExtComma}" "${TypeExtSpace}"
    ${EndIf}
    SetRegView 32
    !insertmacro AddWmpMimeType_Internal "${MimeType}" "${TypeDesc}" "${TypeExt}" "${TypeExtComma}" "${TypeExtSpace}"
!macroend
!define AddWmpMimeType "!insertmacro AddWmpMimeType"

!macro DeleteWmpMimeType MimeType
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\${MimeType}"
    ${EndIf}
    SetRegView 32
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\${MimeType}"
!macroend
!define DeleteWmpMimeType "!insertmacro DeleteWmpMimeType"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
!macro RegisterAxPlayerXmlNamespace
    ${If} ${RunningX64}
        SetRegView 64
        WriteRegStr HKLM "SOFTWARE\Microsoft\Internet Explorer\XMLNamespace" "http://www.w3.org/1999/xhtml/video" "{7CC95AE6-C1FA-40CC-AB17-3E91DA2F77CA}"
    ${EndIf}
    SetRegView 32
    WriteRegStr HKLM "SOFTWARE\Microsoft\Internet Explorer\XMLNamespace" "http://www.w3.org/1999/xhtml/video" "{7CC95AE6-C1FA-40CC-AB17-3E91DA2F77CA}"
!macroend
!define RegisterAxPlayerXmlNamespace "!insertmacro RegisterAxPlayerXmlNamespace"

!macro UnRegisterAxPlayerXmlNamespace
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegValue HKLM "SOFTWARE\Microsoft\Internet Explorer\XMLNamespace" "http://www.w3.org/1999/xhtml/video" 
    ${EndIf}
    SetRegView 32
    DeleteRegValue HKLM "SOFTWARE\Microsoft\Internet Explorer\XMLNamespace" "http://www.w3.org/1999/xhtml/video" 
!macroend
!define UnRegisterAxPlayerXmlNamespace "!insertmacro UnRegisterAxPlayerXmlNamespace"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
!macro RegisterUserAgentString OpenCodecsInfo
    ${If} ${RunningX64}
        SetRegView 64
        WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\5.0\User Agent\Post Platform" "${OpenCodecsInfo}" ""
    ${EndIf}
    SetRegView 32
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\5.0\User Agent\Post Platform" "${OpenCodecsInfo}" ""
!macroend
!define RegisterUserAgentString "!insertmacro RegisterUserAgentString"

!macro UnRegisterUserAgentString OpenCodecsInfo
    ${If} ${RunningX64}
        SetRegView 64
        DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\5.0\User Agent\Post Platform" "${OpenCodecsInfo}"
    ${EndIf}
    SetRegView 32
    DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\5.0\User Agent\Post Platform" "${OpenCodecsInfo}"
!macroend
!define UnRegisterUserAgentString "!insertmacro UnRegisterUserAgentString"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
!macro AddMediaPlayerDesc_Internal Descriptions Types MuiDescription
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
    WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\Descriptions" $2 "${Descriptions}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\MUIDescriptions" $2 "@${MuiDescription},-101"
    WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\Types" $2 "${Types}"
   
    WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "MediaDescNum" $2
!macroend

!macro AddMediaPlayerDesc Descriptions Types
    ${If} ${RunningX64}
        SetRegView 64
        !insertmacro AddMediaPlayerDesc_Internal "${Descriptions}" "${Types}" "$INSTDIR\x64\wmpinfo.dll"
    ${EndIf}
    SetRegView 32
    !insertmacro AddMediaPlayerDesc_Internal "${Descriptions}" "${Types}" "$INSTDIR\wmpinfo.dll"
!macroend
!define AddMediaPlayerDesc "!insertmacro AddMediaPlayerDesc"

!macro RemoveMediaPlayerDesc_Internal RegKey
    ReadRegStr $0 HKLM "${RegKey}" "MediaDescNum"
    ${If} $0 != ""
        DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\Descriptions" $0 
        DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\MUIDescriptions" $0 
        DeleteRegValue HKLM "SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\Types" $0 
    ${EndIf}  
!macroend

!macro RemoveMediaPlayerDesc RegKey
    ${If} ${RunningX64}
        SetRegView 64
        !insertmacro RemoveMediaPlayerDesc_Internal "${RegKey}"
    ${EndIf}
    SetRegView 32
    !insertmacro RemoveMediaPlayerDesc_Internal "${RegKey}"
!macroend
!define RemoveMediaPlayerDesc "!insertmacro RemoveMediaPlayerDesc"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
!macro CheckWindowsMediaPlayer
;   ReadRegStr $WMP_LOCATION HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer" "Player.Path"
    StrCpy $WMP_LOCATION_WIN32 "$PROGRAMFILES\Windows Media Player\wmplayer.exe"
    ${If} ${RunningX64}
        StrCpy $WMP_LOCATION_X64 "$PROGRAMFILES64\Windows Media Player\wmplayer.exe"
    ${EndIf}
  
    IfFileExists  $WMP_LOCATION_WIN32 +3 0
    IfSilent +2
    MessageBox MB_OK|MB_ICONEXCLAMATION "A recognised version of Windows Media Player was not found. $\n File extenstion association must be done manually." IDOK 0

    ${If} ${RunningX64}
        IfFileExists  $WMP_LOCATION_X64 +3 0
        IfSilent +2
        MessageBox MB_OK|MB_ICONEXCLAMATION "A recognised version of Windows Media Player (x64) was not found. $\n File extenstion association must be done manually." IDOK 0
    ${EndIf}
!macroend
!define CheckWindowsMediaPlayer "!insertmacro CheckWindowsMediaPlayer"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

!macro AddFile FileName
    File "bin\win32\${FileName}"
    ${If} ${RunningX64}
        File "/oname=x64\${FileName}" "bin\x64\${FileName}"
    ${EndIf}
!macroend
!define AddFile "!insertmacro AddFile"

!macro DeleteFile FileName
    Delete "$INSTDIR\${FileName}"
    ${If} ${RunningX64}
        Delete "$INSTDIR\x64\${FileName}"
    ${EndIf}
!macroend
!define DeleteFile "!insertmacro DeleteFile"

;--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Function RunUninstaller
    Exch $0
    ReadRegStr $R1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\$0" "UninstallString"
    StrCmp $R1 "" job_done
 
    IfSilent +3
    MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "$0 is already installed. $\n$\nClick `OK` to remove the existing version or `Cancel` to cancel this installation." IDOK uninst
    Abort

;Run the uninstaller
uninst:
    ClearErrors
    ; Copy the uninstaller to a temp location
    GetTempFileName $1
    CopyFiles $R1 $1

    ; Copy the path of the uninstaller
    StrCpy $2 $R1 -11
 
    ;Start the uninstaller using the option to not copy itself
    IfSilent 0 +3
        ExecWait '$1 /S /FromInstaller _?=$2'
        Goto AfterSilent 
    ExecWait '$1 /FromInstaller _?=$2'
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
        "Unable to remove previous version of $0"
        Abort
  
job_done:
    ; remove the copied uninstaller
    Delete '$1'
    Pop $0

FunctionEnd
