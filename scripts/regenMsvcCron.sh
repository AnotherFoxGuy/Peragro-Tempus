cd /home/peragro/
if [! -d msvcregenAuto]
then
  echo "--> Making temporary folder."
  mkdir msvcregenAuto
else
  echo "--> Cleaning temporary folder."
  rm -rf msvcregenAuto
  echo "--> Making temporary folder."
  mkdir msvcregenAuto
fi
cd msvcregenAuto
echo "Checking out repository."
svn co https://cyanox.nl/peragro/trunk/ .
echo "--> Generating configure scripts."
./autogen.sh
echo "--> Configuring source tree."
./configure --without-cs --without-cel --without-CEGUI --without-pthread --without-crystalspace
echo "--> Generating project files."
jam msvcgen
echo "--> Copying generated files to source tree."
cp -rf out/msvc/* msvc/
cd msvc
echo "--> Commiting to SVN Repository if changes where found."
svn ci . -m "Automated MSVC project file regeneration."
cd ../..
echo "--> Cleaning up temporary folder."
rm -rf msvcregenAuto
echo "--> Done."
