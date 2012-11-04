#!/bin/bash

if [ ! -e ./bin/unixtime ];
then
	echo "ERROR: make it";
	exit 1;
fi


A='04.11.2012 15:41:08';
B=$(./bin/unixtime --to '%d.%m.%Y %H:%M:%S' "${A}");
C=$(./bin/unixtime --from '%d.%m.%Y %H:%M:%S' "${B}");

if [ "${A}" != "${C}" ];
then
	echo $A;
	echo $B;
	echo $C;
	echo "ERROR: result different";
	exit 1;
fi


X='2012.11.04-15.41.08';
Y=$(./bin/unixtime --to '%Y.%m.%d %H:%M:%S' "${X}");
Z=$(./bin/unixtime --from '%Y.%m.%d-%H.%M.%S' "${Y}");

if [ "${X}" != "${Z}" ];
then
	echo $X;
	echo $Y;
	echo $Z;
	echo "ERROR: result different";
	exit 1;
fi


echo "ok";


exit 0;
