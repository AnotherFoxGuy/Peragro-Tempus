#!/bin/sh
echo -----------------------------------------------------------------------------
echo `date`
echo -----------------------------------------------------------------------------
cd /home/peragro/peragro
svn up
# echo "--> Autogen"
# ./autogen.sh
# echo "--> Configuring source tree."
# ./configure --without-cs --without-cel --without-CEGUI --without-pthread --without-boost
export PTREV="`svn info|grep Revision`"
echo "--> Generating project files."
jam apidocs
echo "--> Copying generated files to website."
cp -rf out/docs/html/devapi /var/www/sites/peragro.nl/HTML/
cp -rf out/docs/html/devapi.log /var/www/sites/peragro.nl/HTML/devapi/
echo "--> Done."
