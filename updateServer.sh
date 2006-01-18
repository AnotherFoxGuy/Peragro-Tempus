#!/bin/sh

svn up
svn revert . -R
jam distclean
./autogen.sh
./configure --without-cs --without-cel --with-debug
jam server
