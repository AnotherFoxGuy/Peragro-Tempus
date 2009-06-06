@ECHO OFF

CLS

REM ============================================
REM Usage:
REM Copy the below to a textfile and adjust to your settings.
REM Run with 'create-static-distro.cmd settingsFile'
REM ==[START PASTE]============================
SET CWD=%~dp0
SET zip_path="C:\Program Files\7-Zip"
SET VC_path="C:\Program Files\Microsoft Visual Studio 9.0"
SET cswinlibs_path=".\CrystalSpaceLibs"
SET CS_path=%CWD%\CS
SET CEL_path=%CWD%\CEL
SET PT_path=%CWD%\PT
SET verbose="True"
SET buildType="Release (with DLLs)|Win32"
REM SET buildType="Release (with static libs)|Win32"

SET FTP_server=peragro.org
SET FTP_user=
SET FTP_pass=
SET FTP_path="HTML/win32"
REM ==[END PASTE]==================================


REM ==Some auto configure stuff================
IF EXIST %1 FOR /F "eol=; tokens=* delims=, " %%i in (%1) do %%i

SET CRYSTAL=%CS_path%
SET CEL=%CEL_path%
SET PERAGRO=%PT_path%
ECHO CWD=%CWD%
ECHO CRYSTAL=%CRYSTAL%
ECHO CEL=%CEL%
ECHO PERAGRO=%PERAGRO%
ECHO.

SET PATH=%PATH%;%PERAGRO%\scripts\build\release\windows\

SET COMPILER=VCExpress.exe
IF EXIST %VC_path%\Common7\IDE\devenv.exe SET COMPILER=devenv.exe
REM ===========================================

:MAIN
CALL :LOGFILE
CALL :UPDATE
REM CALL :REVERT
CALL :COMPILE
CALL :PACKAGE
REM CALL :UPLOAD
GOTO QUIT


:LOGFILE
ECHO Creating log file...
SET LOGFILE="%CWD%\distro-pack.log"
ECHO Logfile is %LOGFILE%
ECHO Build started at %DATE:~4,10% %TIME% > %LOGFILE%
ECHO ======================================= >> %LOGFILE%
ECHO.
GOTO:EOF


:UPDATE
CALL log.cmd %LOGFILE% "Updating CS..."
svn up %CS_path% >> %LOGFILE%

CALL log.cmd %LOGFILE% "Updating CEL..."
svn up %CEL_path% >> %LOGFILE%

CALL log.cmd %LOGFILE% "Updating PT..."
svn up %PT_path% >> %LOGFILE%
GOTO:EOF


:REVERT
CALL log.cmd %LOGFILE% "Reverting CS..."
svn revert -R %CS_path% >> %LOGFILE%

CALL log.cmd %LOGFILE% "Reverting CEL..."
svn revert -R %CEL_path% >> %LOGFILE%

CALL log.cmd %LOGFILE% "Reverting PT..."
svn revert -R %PT_path% >> %LOGFILE%
GOTO:EOF


:COMPILE
CALL log.cmd %LOGFILE% "Compiling CS..."
CALL build.cmd %VC_path% BUILD %buildType% "%CS_path%\mk\msvc9\wksstatic.sln" ".\CS-build.txt"

CALL log.cmd %LOGFILE% "Compiling CEL..."
CALL build.cmd %VC_path% BUILD %buildType% "%CEL_path%\msvc\9\wksstatic.sln" ".\CEL-build.txt"

CALL log.cmd %LOGFILE% "Compiling Peragro..."
CALL build.cmd %VC_path% BUILD %buildType% "%PT_path%\msvc\9\wksstatic.sln" ".\PT-build.txt"

IF errorlevel 1 (
IF NOT DEFINED CLEANCOUNTER (
SET CLEANCOUNTER=1
CALL log.cmd %LOGFILE% "Build errors dectected, trying a clean and recompile."
GOTO CLEAN
GOTO COMPILE
)
)
GOTO:EOF


:PACKAGE
CALL log.cmd %LOGFILE% "Cleaning existing directory"
rmdir /Q /S pt-static >> %LOGFILE%

CALL log.cmd %LOGFILE% "Copy CS shaders"
xcopy /S /Q %CS_path%\data\shader pt-static\shader\ >> %LOGFILE%
xcopy /S /Q %CS_path%\data\shader-old pt-static\shader-old\ >> %LOGFILE%
xcopy /S /Q %CS_path%\data\renderlayers pt-static\data\renderlayers\ >> %LOGFILE%

CALL log.cmd %LOGFILE% "Copy PT shaders"
xcopy /S /Q %PT_path%\data\art\shaders pt-static\shader\ >> %LOGFILE%

CALL log.cmd %LOGFILE% "Copy Peragro data"
xcopy /S /Q %PT_path%\data pt-static\data\ >> %LOGFILE%

CALL log.cmd %LOGFILE% "Copy static client"
copy %PT_path%\peragro_static.exe pt-static\peragro.exe >> %LOGFILE%

CALL log.cmd %LOGFILE% "Copy configs"
copy %CS_path%\data\config-plugins pt-static\data\config >> %LOGFILE%
copy %PT_path%\vfs-static.cfg pt-static\vfs.cfg >> %LOGFILE%

CALL log.cmd %LOGFILE% "Copy CS and CsWinLibs DLLs"
xcopy %cswinlibs_path%\dlls\vc\*csvc9.dll pt-static >> %LOGFILE%
xcopy %cswinlibs_path%\dlls\*.dll pt-static >> %LOGFILE%

CALL log.cmd %LOGFILE% "Copy redist dlls"
xcopy /Y %VC_path%\VC\redist\x86\Microsoft.VC90.CRT\*.* pt-static >> %LOGFILE%

CALL log.cmd %LOGFILE% "Copy CS, CEL and PT plugins"
xcopy /Y %CS_path%\crystalspace*-*-vc9.dll pt-static >> %LOGFILE%
xcopy /Y %CEL_path%\celtool-*-vc9.dll pt-static >> %LOGFILE%
xcopy /Y %PT_path%\*-*-vc9.dll pt-static >> %LOGFILE%

cd pt-static\

CALL log.cmd %LOGFILE% "Creating data.zip"
cd data
%zip_path%\7z a -bd -tzip ..\data.zip * >> %LOGFILE%
cd ..

CALL log.cmd %LOGFILE% "Creating config.zip"
cd data\config
%zip_path%\7z a -bd -tzip ..\..\config.zip * >> %LOGFILE%
cd ..\..

CALL log.cmd %LOGFILE% "Creating shader.zip"
cd shader
%zip_path%\7z a -bd -tzip ..\shader.zip * >> %LOGFILE%
cd ..

CALL log.cmd %LOGFILE% "Creating shader-old.zip"
cd shader-old
%zip_path%\7z a -bd -tzip ..\shader-old.zip * >> %LOGFILE%
cd ..

CALL log.cmd %LOGFILE% "Removing temporary files"
rmdir /s /Q data\ >> %LOGFILE%
rmdir /s /Q shader\ >> %LOGFILE%
rmdir /s /Q shader-old\ >> %LOGFILE%

CALL log.cmd %LOGFILE% "Creating final zip"
%zip_path%\7z a -bd -tzip peragro.zip * >> %LOGFILE%
move peragro.zip ..\ >> %LOGFILE%

cd ..
GOTO:EOF


:UPLOAD
CALL log.cmd %LOGFILE% "Uploading ZIP"

CALL :RENAME

IF %verbose% == "True" (
CALL ftpupload.cmd %filename% True
) ELSE (
CALL ftpupload.cmd %filename%
)
GOTO:EOF


:RENAME
CALL log.cmd %LOGFILE% "Renaming zip"

SET filename=peragro-%DATE:~4,10%.zip
SET filename=%filename:/=-%

move peragro.zip "%filename%"
GOTO:EOF

:CLEAN
CALL log.cmd %LOGFILE% "Cleaning CS..."
CALL build.cmd %VC_path% CLEAN %buildType% "%CS_path%\mk\msvc9\wksstatic.sln" ".\CS-build.txt"

CALL log.cmd %LOGFILE% "Cleaning CEL..."
CALL build.cmd %VC_path% CLEAN %buildType% "%CEL_path%\msvc\9\wksstatic.sln" ".\CEL-build.txt"

CALL log.cmd %LOGFILE% "Cleaning Peragro..."
CALL build.cmd %VC_path% CLEAN %buildType% "%PT_path%\msvc\9\wksstatic.sln" ".\PT-build.txt"

GOTO:EOF

:ERROR
ECHO.
ECHO Usage: 'create-static-distro.cmd settingsFile'
PAUSE

:QUIT
ECHO.
ECHO ======================================= >> %LOGFILE%
ECHO Build ended at %DATE:~4,10% %TIME% >> %LOGFILE%
ECHO Read '%LOGFILE%' for details.

