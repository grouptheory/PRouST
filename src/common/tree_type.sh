#! /bin/sh
#
# Make was launched either from a directory in the src or dev tree
# return which it was dev or src, null if no directory of either name is
# found. Return the deepest match
# Note: /usr/src/pnni/dev/mikey/sim should return dev
#
here=`pwd | sed -e 's/\// /g'`

# special case of running from /
if [ "$here" = " " ]; then exit; fi

set $here
tree_type=
while [ $# != 0 ]; do
  if [ $1 = src  -o  $1 = dev ]; then tree_type=$1;  fi
  shift
done

echo $tree_type
