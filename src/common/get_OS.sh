#! /bin/sh
#
# generate unique OS to be used by Makes and shells
#
BASE_OS=`uname -s`
OSV=

if [ $BASE_OS = "SunOS" ]; then
  OSVRS=`uname -r | awk -F. '{print $1}'`
  if [ $OSVRS != 4 ]; then
    OSV=$OSVRS
  fi
fi
OS=$BASE_OS$OSV

#
# OS will end up as a -D__OS__ on compile line
# this must be "letters, numbers, and _"
# the following reduces CYGWIN-4.0 to CYGWIN (the -4.0 doesn't belong on uname -s anyway)

echo $OS | sed -e 's/-.*//'
