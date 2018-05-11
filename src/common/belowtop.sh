#! /bin/sh

# removes $1 from pwd and returns result
TOP=$1;

# use an echo that uderstands -n option (no newline)

if [ "$OS" = "SunOS5" ]; then
  ECHO=/usr/ucb/echo
else
  ECHO=echo
fi

$ECHO -n `pwd|sed "s+$TOP++;s+ ++g"`/

