echo -----------------------------------------------------------------------------
echo `date`
echo -----------------------------------------------------------------------------
cd /home/peragro/
if [ -d msvcregenAuto ]
then
  cd msvcregenAuto
  echo "--> Updating working repoitory."
  svn up
  echo "--> Reverting potential conflicts."
  svn revert . -R
  echo "--> Jam maintiner cleaning."
  jam maintainerclean > /dev/null
  echo "--> Generating configure scripts."
  ./autogen.sh
else
  echo "--> Making temporary folder."
  mkdir msvcregenAuto
  cd msvcregenAuto
  echo "Checking out repository."
  svn co https://cyanox.nl/peragro/trunk/ . > /dev/null
  echo "--> Generating configure scripts."
  ./autogen.sh
fi
echo "--> Configuring source tree."
./configure --without-cs --without-cel --without-CEGUI --without-pthread --without-boost > /dev/null
echo "--> Generating project files."
jam msvcgen > /dev/null
echo "--> Copying generated files to source tree."
cp -rf out/msvc/* msvc/
cd msvc
echo "Adding all files."
cd 7
svn add *
cd ../71
svn add *
cd ../8
svn add *
cd ../9
svn add *
cd ../..
echo "--> Commiting to SVN Repository if changes where found."
svn ci . -m "Automated MSVC project file regeneration."
cd ../..
echo "--> Done."
