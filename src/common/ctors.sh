#! /bin/sh
bindir=`dirname $0`
top=`cd $bindir/../..; pwd`
od="bilal/fsm bilal/FW"
OS=`$bindir/get_OS.sh`
OSLIB="_$OS"
for module in sim fsm codec FW; do
  for d in $od; do
    base=`basename $d`
    if [ $base = $module ]; then
      if [ -f $top/dev/$d/libctors$base$OSLIB.a ]; then
        liba="$liba $top/dev/$d/libctors$base$OSLIB.a"
	basedone="$module"
      fi
    fi
  done
  if [ "x$basedone" != "x$module" ] ; then
    liba="$liba -lctors$module$OSLIB"
  fi
done
echo $liba


