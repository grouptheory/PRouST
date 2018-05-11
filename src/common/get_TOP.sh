#! /bin/sh

here=`pwd`

# insert spaces
this=`echo $here | sed -e 's+/+ +g'`

# reverse the order
set $this
while [ $# -ne 0 ]; do
  rev="$1 $rev"
  shift
done

#search for src or dev
set $rev
while [ $# -ne 0 ]; do
  if [ $1 = "src" -o $1 = "dev" ]; then
    break;
  fi
  shift
done

# pop dev or src off the list
shift

# construct TOP by reversing remaining stack and adding /
while [ $# -ne 0 ]; do
  TOP="/$1$TOP"
  shift
done
echo $TOP
