#!/bin/sh

if [ ! -e server.pid ]; then
  echo "Server is not running."
  exit
fi

pid=`cat server.pid`
rm server.pid
kill $pid
