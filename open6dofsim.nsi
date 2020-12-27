!define VERSION 0.0.1
!define APPNAME "Open6DOFSim"
!define MUI_PRODUCT "Open6DOFSim ${VERSION}"
!define MUI_VERSION "${VERSION}"
!define MUI_FILE "open6dofsim"
!define BUILDPATH "x64\Release"
CRCCheck ON

!include "MUI2.nsh"

InstallDir "$PROGRAMFILES64\${MUI_PRODUCT}"

!define MUI_WELCOMEPAGE
!define MUI_LICENSEPAGE
!define MUI_DIRECTORYPAGE
!define MUI_ABORTWARNING
!define MUI_UNINSTALLER
!define MUI_UNCONFIRMPAGE
!define MUI_FINISHPAGE

!define MUI_PAGE_HEADER_TEXT "License Information"
!define MUI_PAGE_HEADER_SUBTEXT "Please review the license terms before installing Open6DOFSim."
!define MUI_LICENSEPAGE_TEXT_TOP "Press Page Down or scroll to see the rest of the license."
!define MUI_LICENSEPAGE_TEXT_BOTTOM " "
!define MUI_LICENSEPAGE_BUTTON "&Next >"

!insertmacro MUI_LANGUAGE "English"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; LicenseData "LICENSE"

Name "Open6DOFSim"
OutFile "Open6DOFSim-${VERSION}-Installer-x64.exe"

Section "install"

; Add files

SetOutPath "$INSTDIR"
File "${BUILDPATH}\${MUI_FILE}.exe"
File "${BUILDPATH}\D3Dcompiler_47.dll"
File "${BUILDPATH}\libEGL.dll"
File "${BUILDPATH}\libGLESV2.dll"
File "${BUILDPATH}\opengl32sw.dll"
File "${BUILDPATH}\Qt5Core.dll"
File "${BUILDPATH}\Qt5Gui.dll"
File "${BUILDPATH}\Qt5Network.dll"
File "${BUILDPATH}\Qt5SerialPort.dll"
File "${BUILDPATH}\Qt5Svg.dll"
File "${BUILDPATH}\Qt5Widgets.dll"
SetOutPath "$INSTDIR\platforms"
File "${BUILDPATH}\platforms\qwindows.dll"
SetOutPath "$INSTDIR\bearer"
File "${BUILDPATH}\bearer\qgenericbearer.dll"
SetOutPath "$INSTDIR\iconengines"
File "${BUILDPATH}\iconengines\qsvgicon.dll"
SetOutPath "$INSTDIR\imageformats"
File "${BUILDPATH}\imageformats\qgif.dll"
File "${BUILDPATH}\imageformats\qicns.dll"
File "${BUILDPATH}\imageformats\qico.dll"
File "${BUILDPATH}\imageformats\qjpeg.dll"
File "${BUILDPATH}\imageformats\qsvg.dll"
File "${BUILDPATH}\imageformats\qtga.dll"
File "${BUILDPATH}\imageformats\qtiff.dll"
File "${BUILDPATH}\imageformats\qwbmp.dll"
File "${BUILDPATH}\imageformats\qwebp.dll"
SetOutPath "$INSTDIR\styles"
File "${BUILDPATH}\styles\qwindowsvistastyle.dll"
SetOutPath "$INSTDIR\translations"
File "${BUILDPATH}\translations\*.qm"

; create desktop shortcut
CreateShortCut "$DESKTOP\${MUI_PRODUCT}.lnk" "$INSTDIR\${MUI_FILE}.exe" ""

; create start menu items
CreateDirectory "$SMPROGRAMS\${MUI_PRODUCT}"
CreateShortCut "$SMPROGRAMS\${MUI_PRODUCT}\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
CreateShortCut "$SMPROGRAMS\${MUI_PRODUCT}\${MUI_PRODUCT}.lnk" "$INSTDIR\${MUI_FILE}.exe" "" "$INSTDIR\${MUI_FILE}.exe" 0

; write uninstall info to registry
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayName" "${MUI_PRODUCT} (remove only)"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "UninstallString" "$INSTDIR\Uninstall.exe"
 
WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

Section "Uninstall"
 
;Delete Files 
  RMDir /r "$INSTDIR\*.*"    
 
;Remove the installation directory
  RMDir "$INSTDIR"
 
;Delete Start Menu Shortcuts
  Delete "$DESKTOP\${MUI_PRODUCT}.lnk"
  Delete "$SMPROGRAMS\${MUI_PRODUCT}\*.*"
  RmDir  "$SMPROGRAMS\${MUI_PRODUCT}"
 
;Delete Uninstaller And Unistall Registry Entries
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\${MUI_PRODUCT}"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}"  
 
SectionEnd

;Function that calls a messagebox when installation finished correctly
Function .onInstSuccess
  MessageBox MB_OK "You have successfully installed ${MUI_PRODUCT}. Use the desktop icon to start the program."
FunctionEnd
 
Function un.onUninstSuccess
  MessageBox MB_OK "You have successfully uninstalled ${MUI_PRODUCT}."
FunctionEnd
