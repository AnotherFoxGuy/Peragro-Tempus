;Peragro Tempus Installer
;by Ross Brackstone
;Based on NSIS examples written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;General

  ;Name and file
  Name "Peragro Tempus"
  OutFile "PTinstall.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\Peragro Tempus"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Peragro Tempus" ""
;--------------------------------
;Variables

  Var MUI_TEMP
  Var STARTMENU_FOLDER

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "..\gpl.txt"
  !insertmacro MUI_PAGE_LICENSE "..\CCL.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Peragro Tempus" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER

  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

; Just the Client
Section "Peragro Game Client" SecClient

  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...

  file /r PT
 
  ;Store installation folder
  WriteRegStr HKCU "Software\Peragro Tempus" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

;Start Menu
Section "Start Menu Shortcuts" SecStartMenu

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER\Peragro Tempus.lnk" "$INSTDIR\PT\client.exe" -fs -mode=800x600
    CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER\Peragro Tempus (windowed).lnk" "$INSTDIR\PT\client.exe" -mode=800x600
    CreateShortcut "$SMPROGRAMS\$STARTMENU_FOLDER\Peragro Tempus (safe mode).lnk" "$INSTDIR\PT\client.exe" -verbose=-scf -console 

  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd


;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecClient ${LANG_ENGLISH} "The Peragro Tempus Client."
  LangString DESC_SecStartMenu ${LANG_ENGLISH} "Start Menu Shortcuts."

  ;Assign language strings to sections

  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecClient} $(DESC_SecClient)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenu} $(DESC_SecStartMenu)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  Delete "$INSTDIR\Uninstall.exe"

  RMDir /r "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
    
  Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\Peragro Tempus.lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\Peragro Tempus (windowed).lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\Peragro Tempus (safe mode).lnk"

  
  ;Delete empty start menu parent diretories
  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
 
  startMenuDeleteLoop:
	ClearErrors
    RMDir $MUI_TEMP
    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."
    
    IfErrors startMenuDeleteLoopDone
  
    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
  startMenuDeleteLoopDone:

  DeleteRegKey /ifempty HKCU "Software\Peragro Tempus"

SectionEnd