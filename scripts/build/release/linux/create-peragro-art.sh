#!/bin/sh
export CRYSTAL=/home/peragro/peragro/CS
export LD_LIBRARY_PATH=$CRYSTAL
export PATH=$CRYSTAL:$PATH

rm -rf ptartexport

svn export https://cyanox.nl/ptartexport

rm -rf peragro-art
mkdir peragro-art
mv ptartexport/world/*.zip peragro-art

cd peragro-art
for e in *.zip
do
	echo convert $e
	unzip $e -d $e.dir > /dev/null
	cd $e.dir/
	echo - convert world
	docconv -inds=xmlread -outds=binary world > /dev/null
	rm world.bak
	docconv -inds=xmlread -outds=binary lightmaps.cslib > /dev/null
	rm lightmaps.cslib.bak
	for f in factories/*
	do
		echo - convert "$f"
		docconv -inds=xmlread -outds=binary "$f" > /dev/null
		rm "$f.bak"
	done
        for f in *.params
        do
		echo - convert "$f"
                docconv -inds=xmlread -outds=binary "$f" > /dev/null
                rm "$f.bak"
        done
	zip -9 -r ../$e . > /dev/null
        cd ..
        rm -r $e.dir
done
cd ..

rm -rf ptartexport/shaders
zip -9 -r peragro-art/art.zip ptartexport/*

zip -9 -r peragro-art-`date +"%d-%m-%Y"`.zip peragro-art/*
