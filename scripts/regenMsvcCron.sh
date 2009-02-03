cd /home/sueastside/dev
if [ -d msvcregenAuto ]
then
  cd msvcregenAuto
  echo "--> Updating working repository."
  svn up --ignore-externals 
  echo "--> Reverting potential conflicts."
  svn revert . -R >/dev/null 2>&1
  echo "--> Jam maintainer cleaning."
  jam maintainerclean > /dev/null
else
  echo "--> Making temporary directory."
  mkdir msvcregenAuto
  cd msvcregenAuto
  echo "Checking out repository."
  svn co --ignore-externals https://cyanox.nl/peragro/trunk/ . >/dev/null 2>&1
fi

echo "--> Updating buildsystem files from CS."
cd mk/autoconf
wget -erobots=off -r -l1 --no-parent -A'.m4,config.*,install-sh' -nd -m 'https://crystal.svn.sourceforge.net/svnroot/crystal/CS/trunk/mk/autoconf/'  >/dev/null 2>&1
svn add * >/dev/null 2>&1
cd ../jam
wget -erobots=off -r -l1 --no-parent -A.jam -nd -m 'https://crystal.svn.sourceforge.net/svnroot/crystal/CS/trunk/mk/jam/' >/dev/null 2>&1
svn add * >/dev/null 2>&1
cd ../msvcgen
wget -erobots=off -r -l1 --no-parent -A'*.tlib,custom.cslib' -nd -m 'https://crystal.svn.sourceforge.net/svnroot/crystal/CS/trunk/mk/msvcgen/' >/dev/null 2>&1
svn add * >/dev/null 2>&1
cd ../..

echo "--> Generating configure scripts."
./autogen.sh
echo "--> Configuring source tree."
./configure --without-cs --without-cel --without-CEGUI --without-pthread --without-boost > /dev/null
echo "--> Generating project files."
jam msvcgen > /dev/null
echo "--> Deleting old files."
rm msvc/*/*
echo "--> Copying generated files to source tree."
cp -rf out/msvc/* msvc/
echo "Fixing deps."
cd scripts
python fixmsvc.py
cd ..
echo "Adding all files."
cd msvc
cd 7
svn add *
cd ../71
svn add *
cd ../8
svn add *
cd ../9
svn add *
cd ../common
svn add *
cd ../..
echo "--> Deleting files no longer part of build system."
svn status | grep "\!" | grep -v "appperagro.rc" | grep -v "appperagro_static.rc" | awk ' { print $2 } ' | xargs svn del
echo "--> Committing to SVN Repository if changes were found."
svn ci . -m "Automated MSVC project file regeneration."
cd ../..
echo "--> Done."
