@ECHO OFF

if "%1" == "" goto ERROR

IF "%2" == "" (
ECHO Verbose false.
) ELSE (
ECHO Verbose true.
SET Verbose="True"
)

:UPLOAD

SET filename=%1

ECHO Uploading %filename% to %FTP_path%/%filename% ...

echo user %FTP_user% %FTP_pass% > ftpcmd.dat
REM echo %FTP_pass% >> ftpcmd.dat
echo cd %FTP_path% >> ftpcmd.dat

echo hash >> ftpcmd.dat
echo binary >> ftpcmd.dat

echo put %filename% %filename% >> ftpcmd.dat
echo bye >> ftpcmd.dat

IF %verbose% == "True" (
ftp -n -s:ftpcmd.dat %FTP_server%
) ELSE (
ftp -n -s:ftpcmd.dat %FTP_server% >> ftp.log
)

del ftpcmd.dat
goto END

:ERROR
echo missing argument!
echo usage "ftpupload file [verbose]"

:END
ECHO Done.

