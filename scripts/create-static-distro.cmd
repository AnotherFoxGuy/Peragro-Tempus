@ECHO OFF

CLS

ECHO.
ECHO Make sure to set your values in the script file!
ECHO.

REM ============================================
REM Set your values here.
REM ============================================

SET zip_path="C:\Program Files\7-Zip"
SET VC_path="C:\Program Files\Microsoft Visual Studio 8"

SET cswinlibs_path=".\CrystalSpaceLibs"
SET CS_path=.\CS
SET CEL_path=.\cel
SET PT_path=.\peragro

SET FTP_server=MYSERVER
SET FTP_user=MYUSER
SET FTP_pass=MYPASS
SET FTP_path="MY/PATH"

SET verbose="False"

REM ============================================

:compile
ECHO Compiling CS... 
%VC_path%\Common7\IDE\VCExpress.exe "%CS_path%\msvc\8\wksstatic.sln" /BUILD Release Win32

ECHO Compiling CEL...
%VC_path%\Common7\IDE\VCExpress.exe "%CEL_path%\msvc\8\wksstatic.sln" /BUILD Release Win32

ECHO Compiling Peragro...
%VC_path%\Common7\IDE\VCExpress.exe "%PT_path%\msvc\8\wksstatic.sln" /BUILD Release Win32

:package
ECHO Cleaning existing directory
rmdir /Q /S pt-static > distro-pack.log

ECHO Copy CS shaders
xcopy /S /Q %CS_path%\data\shader pt-static\data\shader\ >> distro-pack.log
xcopy /S /Q %PT_path%\data\art\shaders pt-static\data\shader\ >> distro-pack.log

ECHO Copy Peragro data
xcopy /S /Q %PT_path%\data pt-static\data >> distro-pack.log

ECHO Copy static client
copy %PT_path%\client_static.exe pt-static\peragro.exe >> distro-pack.log

ECHO Copy configs
copy %CS_path%\data\config\gldrivers.xml pt-static\data\config >> distro-pack.log
copy %PT_path%\vfs-static.cfg pt-static\vfs.cfg >> distro-pack.log

ECHO Copy CS and CsWinLibs DLLs
xcopy %cswinlibs_path%\dlls\vc\*csvc8.dll pt-static >> distro-pack.log
xcopy %cswinlibs_path%\dlls\*.dll pt-static >> distro-pack.log

REM Copies msvcp80.dll and msvcr80.dll
REM xcopy dlls\*.dll pt-static >> distro-pack.log


ECHO creating zip files
move pt-static\data\art\world\*.zip pt-static >> distro-pack.log

cd pt-static\data

%zip_path%\7z a -bd -tzip ..\data.zip * >> ..\..\distro-pack.log

cd config

%zip_path%\7z a -bd -tzip ..\..\config.zip * >> ..\..\..\distro-pack.log

cd ..\shader

%zip_path%\7z a -bd -tzip ..\..\shader.zip * >> ..\..\..\distro-pack.log

cd ..\..

ECHO Removing temporary files
rmdir /s /Q data >> ..\distro-pack.log

ECHO Creating final zip
%zip_path%\7z a -bd -tzip peragro.zip * >> ..\distro-pack.log
move peragro.zip ..\ >> ..\distro-pack.log

cd ..

:upload
ECHO.
ECHO 1. Upload ZIP
ECHO 2. Upload EXE
ECHO 3. Quit
SET choice=
SET /p choice=Choice: 
if not '%choice%'=='' set choice=%choice:~0,1%
if '%choice%'=='1' goto ZIP
if '%choice%'=='2' goto EXE
if '%choice%'=='3' goto QUIT
ECHO "%choice%" is not valid please try again.
goto upload

:ZIP
ECHO Uploading ZIP

echo user %FTP_user%> ftpcmd.dat
echo %FTP_pass%>> ftpcmd.dat
echo cd %FTP_path%>> ftpcmd.dat

echo hash>> ftpcmd.dat
echo binary>> ftpcmd.dat

SET filename=peragro-%DATE:~3,10%.zip
SET filename=%filename:/=-%

echo put peragro.zip %filename%>> ftpcmd.dat
echo bye>> ftpcmd.dat

IF %verbose% == "True" (
ftp -n -s:ftpcmd.dat %FTP_server%
) ELSE (
ftp -n -s:ftpcmd.dat %FTP_server% >> distro-pack.log
)

del ftpcmd.dat
goto QUIT


:EXE
ECHO Uploading EXE

echo user %FTP_user%> ftpcmd.dat
echo %FTP_pass%>> ftpcmd.dat
echo cd %FTP_path%>> ftpcmd.dat

echo hash>> ftpcmd.dat
echo binary>> ftpcmd.dat

echo lcd pt-static>> ftpcmd.dat

echo put peragro.exe peragro.exe>> ftpcmd.dat
echo bye>> ftpcmd.dat

IF %verbose% == "True" (
ftp -n -s:ftpcmd.dat %FTP_server%
) ELSE (
ftp -n -s:ftpcmd.dat %FTP_server% >> distro-pack.log
)

del ftpcmd.dat
goto QUIT

:QUIT
ECHO.
ECHO Read 'distro-pack.log' for details.

PAUSE

