#!/bin/sh

SCRIPT=$(readlink -f "$0")
SCRIPT_PATH=$(dirname "$SCRIPT")

cd $SCRIPT_PATH/..
chmod 777 ./vendor/premake/bin/premake5
./vendor/premake/bin/premake5 gmake2
