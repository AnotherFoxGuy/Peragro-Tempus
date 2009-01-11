#!/bin/bash
DOCS=/var/www/sites/peragro.nl_wiki/HTML/static
DEST=/home/peragro/peragro/docs/

cd $DEST

FILES=`find static/ -type f -name .svn -prune -o -print|grep -v \.svn`
svn add static -N
for i in $FILES
do
  rm $i
done
rsync -r $DOCS $DEST
FILES=`find static/ -name .svn -prune -o -print`
svn add static -N
for i in $FILES
do
  svn add $i -N
done

svn status | grep "\!" | awk ' { print $2 } ' | xargs svn del

svn ci . -m "Automated static wiki export update."
