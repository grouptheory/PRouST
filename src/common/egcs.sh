#!/bin/sh

vers=`g++ --version`

egcs=`echo $vers | grep egcs | wc -l | sed -e 's/ *//g'`

old272=`echo $vers | grep '2.7.2' | wc -l | sed -e 's/ *//g'`

if [ "x$egcs" = "x1" ]; then
    echo "g++ EGCS detected."
else
    if [ "x$old272" = "x1" ]; then
	echo "g++ 2.7.2 detected"
    else
	echo "Unknown compiler detected."
	echo "PRouST compiles on g++ versions:"
	echo "   EGCS"
    	echo "   2.7.2.2"
    fi
fi
