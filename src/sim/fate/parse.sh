#!/bin/sh

PROGRAM="`dirname $1`/`basename $1 .fc`"
CWD="`pwd | grep fate`"
EXAMPLES="`pwd | grep examples`"

BASE_OS="`uname -s`"

if [ "$BASE_OS" = "SunOS" ]; then
  OSVRS="`uname -r | awk -F. '{print $1}'`"
  if [ "$OSVRS" != "4" ]; then
    OSV="$OSVRS"
  else
    OSV=""
  fi
else
  OSV=""
fi

OS="$BASE_OS$OSV"

if [ "x$PROGRAM" = "x" ]; then
  echo "usage: $0 fatescript [debug]"
  exit
fi

if [ "x$2" = "xdebug" ]; then
  DEBUG=_debug
else
  DEBUG=
fi

if [ "x$CWD" != "x" ]; then
  if [ "x$PROGRAM" != "x" ]; then
    if [ "x$EXAMPLES" != "x" ]; then
      # we are in the examples subdirectory
      echo Parsing "$PROGRAM".fc ...
      if [ ! -d .tmp ]; then
        mkdir .tmp;
      fi
      cd .tmp ; ../../FC_$OS$DEBUG ../"$PROGRAM".fc > tmp_`basename $PROGRAM`.cc 
      if [ -f tmp_`basename $PROGRAM`.cc ]; then
        cd .. ; ../compile.sh "$PROGRAM" ;
      else
        echo "ERROR\!  tmp_`basename $PROGRAM`.cc wasn't properly generated."
      fi
    fi
  else
    echo "Sorry but I'm unable to find \"$PROGRAM\".fc"
  fi
else
  echo "Sorry but I can't find the fate directory."
fi
