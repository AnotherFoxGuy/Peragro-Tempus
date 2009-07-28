@ECHO OFF
setlocal enabledelayedexpansion

if %1 == "" goto ERROR
if "%2" == "" goto ERROR
if %3 == "" goto ERROR
if %4 == "" goto ERROR
if %5 == "" goto ERROR

:COMPILE

SET VC_path=%1
SET action=%2
SET buildType=%3
SET solution_file=%4

REM ==Some auto configure stuff================
SET COMPILER=VCExpress.exe
IF EXIST %VC_path%\Common7\IDE\devenv.exe SET COMPILER=devenv.exe
REM ===========================================

IF NOT EXIST %VC_path%\Common7\IDE\%COMPILER% (
ECHO No compiler found!
GOTO END
)

IF %action% == CLEAN (
%VC_path%\Common7\IDE\%COMPILER% %solution_file% /CLEAN %buildType%
) ELSE (
ECHO %DATE% > %5
%VC_path%\Common7\IDE\%COMPILER% %solution_file% /BUILD %buildType% /OUT %5

findstr /M /C:"0 failed" %5 > NUL
if !errorlevel! EQU 0 (
ECHO  - Successfully built %solution_file%
SET errorlevel=0
) ELSE (
ECHO  - WARNING: %solution_file% built with errors check %5 for details.
SET errorlevel=1
)
)

goto END

:ERROR
echo missing argument!
echo usage "build VC_path BUILD/CLEAN buildType solution_file [output_file]"

:END
