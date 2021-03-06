#!/bin/sh

ulimit -c unlimited;

#Prevent script from spaming core files
if [ `find . -name "core.*" | wc -l` -ge 100 ]; then
   exit 
fi

./peragro-server 1>server.log 2>server.err &

echo $! > peragro-server.pid                           

wait

if [ -e peragro-server.pid ]; then
  crashtime=`date +%Y%m%d-%H.%S.%M`
  if [ -e "core.` cat peragro-server.pid `" ]; then
    cp peragro-server crashed/peragro-server.$crashtime
    mv core* crashed/core.$crashtime
  fi

  $0 &
fi

