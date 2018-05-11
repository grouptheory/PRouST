#! /bin/sh
#
# Make was launched either from a directory in the src or dev tree
# return the module directory
# Note: /usr/src/pnni/dev/mikey/sim/foo return /usr/src/pnni/dev/mikey/sim
#       /usr/src/pnni/src/sim/foo       return /usr/src/pnni/src/sim
#
common=`dirname $0`
here=`pwd | sed -e 's/\// /g'`

# special case of running from /
if [ "$here" = " " ]; then exit; fi
set $here

# reverse
while [ $# != 0 ]; do
  reverse="$1 $reverse"
  shift
done
set $reverse
tree_type=`$common/tree_type.sh`

if [ x$tree_type = xsrc ]; then
  while [ $# != 0 ]; do
    if [ $1 = src ]; then
	break
    fi
    rest="$1 $rest"
    shift
  done
  while [ $# != 0 ]; do
     srcroot="/$1$srcroot"
     shift
  done
  set $rest
  if [ $# != 0 ]; then
    echo $srcroot/$1
  fi

# DEV TREE
elif [ x$tree_type = xdev ]; then

  while [ $# != 0 ]; do
    if [ $1 = dev ]; then
	break
    fi
    rest="$1 $rest"
    shift
  done
  while [ $# != 0 ]; do
     devroot="/$1$devroot"
     shift
  done
  set $rest
  if [ $# -ge 2 ]; then
    echo $devroot/$1/$2
  fi

fi


