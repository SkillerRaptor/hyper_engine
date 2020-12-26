#!/bin/sh

SCRIPT=$(readlink -f "$0")
SCRIPT_PATH=$(dirname "$SCRIPT")

cd $SCRIPT_PATH
sudo ./vendor/premake/bin/premake5 gmake2
cd ..
