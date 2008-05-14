#!/bin/sh

if [ ! -e peragro-server.pid ]; then
  echo "Server is not running."
  exit
fi

echo "Stopping server."

pid=`cat peragro-server.pid`
rm peragro-server.pid
kill $pid

sleep 1
