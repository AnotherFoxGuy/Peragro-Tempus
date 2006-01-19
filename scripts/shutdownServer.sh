#!/bin/sh

if [ ! -e server.pid ]; then
  echo "Server is not running."
  exit
fi

echo "Stopping server."

pid=`cat server.pid`
rm server.pid
kill $pid

sleep 1
