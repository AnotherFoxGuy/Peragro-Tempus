# Use this script on your own risc.
# It has been tested on Gentoo amd64.

# Place the script in a directory where you want to download 
# CS, CEL, stackless, boost and newentity (PT).

svn co https://crystal.svn.sourceforge.net/svnroot/crystal/CS/trunk CS
cd CS
./configure --without-xaw7 --without-gtk --enable-debug
jam -j3
export CRYSTAL=$PWD
ln -s cs-config* cs-config
cd ..

svn co https://cel.svn.sourceforge.net/svnroot/cel/cel/trunk CEL
cd CEL
./configure --enable-debug
jam -j3
export CEL=$PWD
cd ..

# install stackless python
wget http://www.stackless.com/binaries/stackless-251-export.tar.bz2
tar jxvf stackless-251-export.tar.bz2
#rm -rf  stackless-251-export.tar.bz2 
unset PYTHON
unset PYTHONHOME
unset PYTHONPATH
unset PYTHON_ROOT
unset PYTHON_VERSION
cd stackless-251-export
./configure --enable-shared --prefix=$PWD/../stackless --enable-stacklessfewerregisters
make -j3 && make install

cd ..
#rm -rf stackless-251-export

wget http://superb-west.dl.sourceforge.net/sourceforge/boost/boost_1_34_1.tar.bz2
tar jxvf boost_1_34_1.tar.bz2
#rm -rf boost_1_34_1.tar.bz2
cd boost_1_34_1
export BOOST_BUILD_PATH=$PWD
export LD_LIBRARY_PATH=$PWD/../stackless/lib
export PYTHON_VERSION=2.5
export PYTHON_ROOT=$PWD/../stackless
export PATH=$PWD/../stackless/bin:$PATH 
export PYTHONHOME=$PWD/../stackless
./configure --with-python=$PWD/../stackless/bin/python2.5
bjam --prefix=../boost --with-python --with-filesystem --with-regex --with-date_time --debug-configuration -j3 install

cd ..

#NOTE: libbost_python.so not being built by boost
svn co https://cyanox.nl/peragro/branches/newentity newentity
cd newentity
./autogen.sh
echo $CRYSTAL
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/../stackless/lib/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/../boost/lib/
export PYTHON=$PWD/../stackless/bin/python2.5
export PYTHONHOME=$PWD/../stackless/
export PATH=$PWD/../stackless/bin:$PATH 
./configure --with-boost=$PWD/../boost/ --with-python=$PWD/../stackless/bin/python2.5

jam -j3
