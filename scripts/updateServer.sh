#!/bin/sh

svn up
svn revert . -R
jam distclean
./autogen.sh
./configure --without-cs --without-cel --without-CEGUI --without-boost --enable-debug
jam peragro-server
