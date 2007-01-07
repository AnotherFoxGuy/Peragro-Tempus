#!/bin/sh

ulimit -c unlimited;

server 1>server.log 2>server.err &

echo $! > server.pid                           

wait

if [ -e server.pid ]; then
  crashtime=`date +%Y%m%d-%H.%S.%M`
  cp bin/server crashed/server.$crashtime
  mv core* crashed/core.$crashtime
  $0 &
fi
