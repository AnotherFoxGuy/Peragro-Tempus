@ECHO OFF
setlocal enabledelayedexpansion

if %1 == "" goto ERROR
if %2 == "" goto ERROR

:LOG
SET STRING=%2
SET STRING=%STRING:~1,-1%
ECHO %STRING%
ECHO %STRING% >> %1
goto END

:ERROR
echo missing argument!
echo usage "log.cmd logfile msg"

:END
