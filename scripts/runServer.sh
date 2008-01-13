#!/bin/sh

ulimit -c unlimited;

./server 1>server.log 2>server.err &

echo $! > server.pid                           

wait

if [ -e server.pid ]; then
  crashtime=`date +%Y%m%d-%H.%S.%M`
  if [ -e "core.` cat server.pid `" ]; then
    cp bin/server crashed/server.$crashtime
    mv core* crashed/core.$crashtime
  fi
  $0 &
fi

