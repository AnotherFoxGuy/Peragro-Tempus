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

SET verbose="True"

SET buildType="Release (with DLLs)|Win32"
REM SET buildType="Release (with static libs)|Win32"


REM ===========================================

REM ==Some auto configure stuff================
SET CRYSTAL=%~dp0%CS_path%
SET CEL=%~dp0%CEL_path%

SET COMPILER=VCExpress.exe
IF EXIST %VC_path%\Common7\IDE\devenv.exe SET COMPILER=devenv.exe
REM ===========================================
ECHO %CRYSTAL%
ECHO %CEL%

goto compile

ECHO updating CS... 
svn up %CRYSTAL%
REM svn revert -R %CRYSTAL%

ECHO updating CEL... 
svn up %CEL%
REM svn revert -R %CEL%

ECHO updating PT... 
svn up %PT_path%
svn revert -R %PT_path%

REM ===========================================

:clean
ECHO Cleaning CS...
%VC_path%\Common7\IDE\%COMPILER% "%CS_path%\mk\msvc8\wksstatic.sln" /CLEAN %buildType%

ECHO Cleaning CEL...
%VC_path%\Common7\IDE\%COMPILER% "%CEL_path%\msvc\8\wksstatic.sln" /CLEAN %buildType%

ECHO Cleaning Peragro...
%VC_path%\Common7\IDE\%COMPILER% "%PT_path%\msvc\8\wksstatic.sln" /CLEAN %buildType%


:compile
ECHO Compiling CS...
ECHO %DATE% > CS-build.txt
%VC_path%\Common7\IDE\%COMPILER% "%CS_path%\mk\msvc8\wksstatic.sln" /BUILD %buildType% /OUT .\CS-build.txt

ECHO Compiling CEL...
ECHO %DATE% > CEL-build.txt
%VC_path%\Common7\IDE\%COMPILER% "%CEL_path%\msvc\8\wksstatic.sln" /BUILD %buildType% /OUT .\CEL-build.txt

ECHO Compiling Peragro...
ECHO %DATE% > PT-build.txt
%VC_path%\Common7\IDE\%COMPILER% "%PT_path%\msvc\8\wksstatic.sln" /BUILD %buildType% /OUT .\PT-build.txt

:package
ECHO Cleaning existing directory
rmdir /Q /S pt-static > distro-pack.log

ECHO Copy CS shaders
xcopy /S /Q %CS_path%\data\shader pt-static\shader\ >> distro-pack.log
xcopy /S /Q %CS_path%\data\shader-old pt-static\shader-old\ >> distro-pack.log
xcopy /S /Q %CS_path%\data\renderlayers pt-static\data\renderlayers\ >> distro-pack.log

ECHO Copy PT shaders
xcopy /S /Q %PT_path%\data\art\shaders pt-static\shader\ >> distro-pack.log

ECHO Copy Peragro data
xcopy /S /Q %PT_path%\data pt-static\data\ >> distro-pack.log

ECHO Copy static client
copy %PT_path%\peragro_static.exe pt-static\peragro.exe >> distro-pack.log

ECHO Copy configs
copy %CS_path%\data\config-plugins pt-static\data\config >> distro-pack.log
copy %PT_path%\vfs-static.cfg pt-static\vfs.cfg >> distro-pack.log

ECHO Copy CS and CsWinLibs DLLs
xcopy %cswinlibs_path%\dlls\vc\*csvc8.dll pt-static >> distro-pack.log
xcopy %cswinlibs_path%\dlls\*.dll pt-static >> distro-pack.log

xcopy /Y dlls\*.* pt-static >> distro-pack.log

ECHO Copy CS, CEL and PT plugins
xcopy /Y %CS_path%\crystalspace*-*-vc8.dll pt-static >> distro-pack.log
xcopy /Y %CEL_path%\celtool-*-vc8.dll pt-static >> distro-pack.log
xcopy /Y %PT_path%\*-*-vc8.dll pt-static >> distro-pack.log

cd pt-static\

ECHO Creating data.zip
cd data
%zip_path%\7z a -bd -tzip ..\data.zip * >> ..\..\distro-pack.log
cd ..

ECHO Creating config.zip
cd data\config
%zip_path%\7z a -bd -tzip ..\..\config.zip * >> ..\..\..\distro-pack.log
cd ..\..

ECHO Creating shader.zip
cd shader
%zip_path%\7z a -bd -tzip ..\shader.zip * >> ..\..\distro-pack.log
cd ..

ECHO Creating shader-old.zip
cd shader-old
%zip_path%\7z a -bd -tzip ..\shader-old.zip * >> ..\..\distro-pack.log
cd ..

ECHO Removing temporary files
rmdir /s /Q data\ >> ..\distro-pack.log
rmdir /s /Q shader\ >> ..\distro-pack.log
rmdir /s /Q shader-old\ >> ..\distro-pack.log

ECHO Creating final zip
%zip_path%\7z a -bd -tzip peragro.zip * >> ..\distro-pack.log
move peragro.zip ..\ >> ..\distro-pack.log

cd ..

:upload
ECHO.
ECHO 1. Upload ZIP
ECHO 2. Upload EXE
ECHO 3. Rename ZIP
ECHO 4. Quit
SET choice=
SET /p choice=Choice: 
if not '%choice%'=='' set choice=%choice:~0,1%
if '%choice%'=='1' goto ZIP
if '%choice%'=='2' goto EXE
if '%choice%'=='3' goto RENAME
if '%choice%'=='4' goto QUIT
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

:RENAME
ECHO Renaming zip

SET filename=peragro-%DATE:~3,10%.zip
SET filename=%filename:/=-%

move peragro.zip %filename%

goto QUIT

:QUIT
ECHO.
ECHO Read 'distro-pack.log' for details.

PAUSE

