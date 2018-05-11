#!/bin/sh

# require gawk (gnu awk) in path

# find the g++ compiler version and strip the minor release number
# 2.7.2.2 becomes 2.7.2

COMMONDIR=`dirname $0`
CXX=${CCC=g++}

# get the version (works only on g++) and change . to /
# use dirname to dump minor number then convert / back to .
#
# some compiler releases have minor viariants for example
#    2.7.2.2 is a minor variant of 2.7.2
#
# Keep only 3 levels in compiler version. 
#   To do this we convert version into a / separated list (like a file name)
#   and use dirname to lop off the tail.
#
# Note that 2.7.2 contains two . not 3 .'s

slash=`$CXX --version | tr "." "/"`
depth_of_version=`echo $slash | sed -e 's/[^\/]//g' | tr -d '/012' |wc -c`

while [ $depth_of_version -gt 2 ]; do
#  echo depth of compiler $slash = $depth_of_version; 
  slash=`dirname $slash`
  depth_of_version=`echo $slash | sed -e 's/[^\/]//g'  | tr -d '/012' | wc -c`
done

#echo depth of compiler $slash = $depth_of_version; 

version=`echo $slash | tr "/" "."`

# look for a match -- use gawk because it supports -v to pass args into awk
# luckily Linux provides gawk under that name (gtar is just tar for example)

answer=`gawk -v version=$version \
    '{ if ( version ~ $1 ) print $2; }' < $COMMONDIR/supported_compilers`

# if answer is empty the use the default entry
if [ "x$answer" = "x" ]; then
    answer=`grep default $COMMONDIR/supported_compilers | gawk '{print $2}'`
fi
echo $answer



