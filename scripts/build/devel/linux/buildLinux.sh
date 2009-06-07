#!/bin/bash

VERSION=`grep "#define PACKAGE_VERSION" config.h | egrep -o "[0-9.]+" `
REV=`svn info | grep Revision | egrep -o "[0-9]+"`

if [ "$1" = "dynamic" ] ; then
  buildtype="dynamic"
else
  buildtype="static"
fi                                                                                             

RELEASENAME="peragro-$buildtype-$VERSION-r$REV"
RELEASEDIR="$RELEASENAME"

if [ -e "$RELEASEDIR" ] ; then
  rm -rf "$RELEASEDIR"
fi

mkdir "$RELEASEDIR"

ADDITIONALFILES=""
if [ $buildtype = "dynamic" ] ; then
  jam client
  jam anvil

  cp peragro "$RELEASEDIR/peragro"
  cp anvil "$RELEASEDIR/anvil"

  CSLIBS=`find ../CS | grep \\.so`
  CELLIBS=`find ../cel | grep \\.so`
  CEGUILIBS=`find ../cegui_mk2 | grep \\.libs | grep \\.so`
  CEGUILIBS="$CEGUILIBS `find ../cegui_mk2 | grep \\.libs | grep \\.la`"
  CAL3DLIBS=`find ../cal3d | grep \\.libs | grep \\.so`
  CAL3DLIBS="$CAL3DLIBS `find ../cal3d | grep \\.libs | grep \\.la`"
  ODELIBS=`find ../ode | grep \\.libs | grep \\.so`
  ODELIBS="$ODELIBS `find ../ode | grep \\.libs | grep \\.la`"

  SHAREDLIBS="$CSLIBS $CELLIBS $CEGUILIBS $CAL3DLIBS $ODELIBS"

  SHAREDLIBS=`"$SHAREDLIBS" | sed 's/\n/ /'`
 
  echo "$SHAREDLIBS"
  exit

  #cp "$SHAREDLIBS" "$RELEASEDIR"
else
  jam client_static
  jam anvil_static
  cp peragro_static "$RELEASEDIR/peragro"
  cp anvil_static "$RELEASEDIR/anvil"
fi

CopyIgnore()
{
  files=`find $1 -type f | grep -v .svn | grep -v CVS`
  cp --parents $files $2
}

mkdir "$RELEASEDIR/shader/"
mkdir "$RELEASEDIR/config/"

CopyIgnore ./data "$RELEASEDIR/"

BUILDDIR=`pwd`

cd $CRYSTAL
CopyIgnore "./data/shader/" "$BUILDDIR/$RELEASEDIR/"
CopyIgnore "./data/config/" "$BUILDDIR/$RELEASEDIR/"
cd $BUILDDIR

cp "$CRYSTAL/data/stdtex.zip" "$RELEASEDIR/data/"
cp README vfs.cfg "$RELEASEDIR"

tar jcf "$RELEASENAME.tar.bz2" "$RELEASEDIR"


