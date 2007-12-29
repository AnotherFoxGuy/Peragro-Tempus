#!/bin/sh
echo -----------------------------------------------------------------------------
echo `date`
echo -----------------------------------------------------------------------------
cd /home/peragro/
if [ -d msvcregenAutoNewEntity ]
then
  cd msvcregenAutoNewEntity
  echo "--> Updating working repository."
  svn up
  echo "--> Reverting potential conflicts."
  svn revert . -R
  echo "--> Jam maintiner cleaning."
  jam maintainerclean > /dev/null
  jam clean > /dev/null
  echo "--> Generating configure scripts."
  ./autogen.sh
else
  echo "--> Making temporary folder."
  mkdir msvcregenAutoNewEntity
  cd msvcregenAutoNewEntity
  echo "Checking out repository."
  svn co https://cyanox.nl/peragro/branches/newentity/ . > /dev/null
  echo "--> Generating configure scripts."
  ./autogen.sh
fi
echo "--> Configuring source tree."
./configure --without-cs --without-cel --without-CEGUI --without-pthread --without-boost > /dev/null
echo "--> Generating project files."
jam msvcgen > /dev/null
echo "--> Copying generated files to source tree."
cp -rf out/mk/* mk/
cd mk
echo "Adding all files."
cd msvc
svn add *
cd ../msvc8
svn add *
cd ../..
echo "--> Committing to SVN Repository if changes were found."
svn ci . -m "Automated MSVC project file regeneration."
cd ../..
echo "--> Done."
