#!/bin/csh -f

TYPE=$1
GRANDPARENT=$2
TOP=$3

QSAAL_DEV="false"

if [ "$GRANDPARENT" != "dev" ]; then
  echo Sorry, you may only do this at the module level.
  exit 1;
fi

if [ -d qsaal ]; then
	QSAAL_DEV="true"
	rm -f ./lib--\>aal.so
	ln -s qsaal/lib${TYPE}_aal.so ./lib--\>aal.so
	occ=`ls * | grep 'lib-->aal.so' | wc -l | sed -e 's/ *//'`
	if [ $occ -gt 0 ]; then
		echo "WARNING: you are shadowing"
		ls */lib--\>aal.so
		echo "WARNING! Having symlinks in submodules is not recommended."
	fi
else
	QSAAL_DEV="false'
	rm -f ./lib--\>aal.so
	ln -s $TOP/src/sean/qsaal/lib${TYPE}_aal.so ./lib--\>aal.so
	occ=`ls * | grep 'lib-->aal.so' | wc -l | sed -e 's/ *//'`
	if [ $occ -gt 0 ]; then
		echo "WARNING! you are shadowing"
		ls */lib--\>aal.so
		echo "WARNING! Having symlinks in submodules is not recommended."
	fi
fi

echo Setup of symbolic link
ls -l lib--\>aal.so
echo Done.
