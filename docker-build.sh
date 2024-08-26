#!/bin/bash

NAME="mtasa"

docker build . -t $NAME
ID = $(docker images -q $NAME)

echo ""
echo "ID: $ID"