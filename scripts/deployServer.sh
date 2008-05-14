#!/bin/sh
if [ ! -d bin ]; then
	mkdir bin
fi

scripts/shutdownServer.sh &&
echo "Deploying Server."
cp peragro-server bin &&
scripts/startServer.sh
