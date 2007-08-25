@echo off

REM Requires to be run from the folder that contains CS, cel and pergaro as subfolders.
REM sevenzip ( 7z.exe ) needs to be in %PATH%

REM Deletes the folder pt-static!

exit

REM Remove the exit line above when everything is set up!

echo Cleaning existing directory
rmdir /Q /S pt-static > distro-pack.log

echo Copy CS shaders
xcopy /S /Q CS\data\shader pt-static\data\shader\ >> distro-pack.log
xcopy /S /Q peragro\data\art\shaders pt-static\data\shader\ >> distro-pack.log

echo Copy Peragro data
xcopy /S /Q peragro\data pt-static\data >> distro-pack.log

echo Copy static client
copy peragro\client_static.exe pt-static\peragro.exe >> distro-pack.log

echo Copy configs
copy CS\data\config\gldrivers.xml pt-static\data\config >> distro-pack.log
copy peragro\vfs-static.cfg pt-static\vfs.cfg >> distro-pack.log

echo Copy CS and CsWinLibs DLLs
xcopy CSLibsStatic\dlls\vc\*csvc8.dll pt-static >> distro-pack.log
xcopy CSLibsStatic\dlls\*.dll pt-static >> distro-pack.log

echo creating zip files
move pt-static\data\art\world\*.zip pt-static >> distro-pack.log

cd pt-static\data

7z a -bd -tzip ..\data.zip * >> ..\..\distro-pack.log

cd config

7z a -bd -tzip ..\..\config.zip * >> ..\..\..\distro-pack.log

cd ..\shader

7z a -bd -tzip ..\..\shader.zip * >> ..\..\..\distro-pack.log

cd ..\..

echo Removing temporary files
rmdir /s /Q data >> ..\distro-pack.log

echo Creating final zip
7z a -bd -tzip peragro.zip * >> ..\distro-pack.log

cd ..

pause

