; Unicode version provided by Jim Park http://forums.winamp.com/showthread.php?postid=2469203

!define LVM_GETITEMCOUNT 0x1004
!define LVM_GETITEMTEXT 0x1073

Function DumpLog
  Exch $5
  Push $0
  Push $1
  Push $2
  Push $3
  Push $4
  Push $6
  Push $7

  FindWindow $0 "#32770" "" $HWNDPARENT
  GetDlgItem $0 $0 1016
  StrCmp $0 0 error
  FileOpen $5 $5 "w"
  FileWriteWord $5 0xfeff ; Write the BOM
  StrCmp $5 0 error
    SendMessage $0 ${LVM_GETITEMCOUNT} 0 0 $6
    IntOp $7 ${NSIS_MAX_STRLEN} * 2
    System::Alloc $7
    Pop $3
    StrCpy $2 0
    System::Call "*(i, i, i, i, i, i, i, i, i) i \
      (0, 0, 0, 0, 0, r3, ${NSIS_MAX_STRLEN}) .r1"
    loop: StrCmp $2 $6 done
      System::Call "User32::SendMessageW(i, i, i, i) i \
        ($0, ${LVM_GETITEMTEXT}, $2, r1)"
      System::Call "*$3(&t${NSIS_MAX_STRLEN} .r4)"
      FileWriteUTF16LE $5 "$4$\r$\n"
      IntOp $2 $2 + 1
      Goto loop
    done:
      FileClose $5
      System::Free $1
      System::Free $3
      Goto exit
  error:
    MessageBox MB_OK|MB_ICONSTOP "Error at DumpLog"
  exit:
    Pop $7
    Pop $6
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Pop $0
    Exch $5
FunctionEnd
