#!/bin/sh
echo -----------------------------------------------------------------------------
echo `date`
echo -----------------------------------------------------------------------------
BASEDIR=/home/peragro/
BDIR=$BASEDIR/msvcregenAutoBranch
MSVCDIRS="7
71
8
9
common"
cd $BASEDIR
if [ -d msvcregenAutoBranch ]
then
  cd $BASEDIR/msvcregenAutoBranch
  echo "--> Updating working repository."
  svn up --ignore-externals
  echo "--> Reverting potential conflicts."
  svn revert . -R
else
  echo "--> Making temporary folder."
  mkdir $BASEDIR/msvcregenAutoBranch
  cd $BASEDIR/msvcregenAutoBranch
  echo "Checking out repository."
  svn co --ignore-externals https://cyanox.nl/peragro/branches/ . > /dev/null
fi
for i in `ls`
do
	echo "Doing branch: $i"
	cd $BDIR/$i
	echo "--> Jam maintiner cleaning."
	jam maintainerclean -j5 > /dev/null
	echo "--> Generating configure scripts."
	chmod +x autogen.sh
	./autogen.sh
	echo "--> Configuring source tree."
	./configure --without-cs --without-cel --without-CEGUI --without-pthread --without-boost > /dev/null
	echo "--> Generating project files."
	jam msvcgen -j5 > /dev/null
	echo "--> Deleting old files."
	if [ -d $BDIR/$i/msvc ]
	then
		rm $BDIR/$i/msvc/*/* -f
		echo "--> Copying generated files to source tree."
		cp -rf $BDIR/$i/out/msvc/* $BDIR/$i/msvc/
		echo "Fixing deps."
		if [ -d $BDIR/$i/scripts ]
		then
			cd $BDIR/$i/scripts
			python fixmsvc.py >/dev/null 2>/dev/null
			cd ..
		fi
		echo "Adding all files."
		if [ -d $BDIR/$i/msvc ]
		then
			cd msvc
			for j in $MSVCDIRS
			do
				if [ -d $BDIR/$i/msvc/$j ]
				then
					svn add -N $BDIR/$i/msvc/$j >/dev/null 2>/dev/null
					cd $BDIR/$i/msvc/$j
					svn add * >/dev/null 2>/dev/null
					cd ..
				fi
			done
		fi
	fi
	cd $BDIR/$i
	if [ -d $BDIR/$i/mk ]
	then
		rm $BDIR/$i/mk/msvc7/* -f
		rm $BDIR/$i/mk/msvc71/* -f
		rm $BDIR/$i/mk/msvc8/* -f
		rm $BDIR/$i/mk/msvc9/* -f
		cp $BDIR/$i/out/mk/* $BDIR/$i/mk/ -rf
		cd $BDIR/$i/mk/
		for j in `ls`
		do
			if [ -d $BDIR/$i/mk/$j/ ]
			then
				cd $BDIR/$i/mk/$j/
				svn add * >/dev/null 2>/dev/null
				cd ..
			fi
		done
	fi
	cd $BDIR/$i
	echo "--> Deleting files no longer part of build system."
	svn status | grep "\!" | grep -v "appperagro.rc" | grep -v "appperagro_static.rc" | awk ' { print $2 } ' | xargs svn del  >/dev/null 2>/dev/null
	echo "--> Committing to SVN Repository if changes were found."
	svn ci . -m "Automated MSVC project file regeneration on branch: $i."
	cd ../..
done
echo "--> Done."
