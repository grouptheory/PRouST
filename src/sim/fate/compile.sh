#!/bin/sh
THIS_DIR="`dirname $0`"
HIDDIR=".tmp"

PROG="`echo $1 | cut -f2 -d/`"
PROG_="$PROG"_
PROG_PATH="`dirname $1`"

SLIBLINE=
if [ -f $HIDDIR/$PROG.lib ]; then
  SLIBLINE="`gawk -f $THIS_DIR/libextract.awk $HIDDIR/$PROG.lib`"
fi

OS="`uname -s`"
MKF=".Makefile.$USER"
CWD="`pwd`"
EXAMPLES="`pwd | grep examples`"
MLOCALPATH="../../Make.local"

if [ "x$PROG_" = "x" ]; then
  echo "usage: $0 fatescript";
  exit;
fi

if [ "x$EXAMPLES" = "x" ]; then
  echo "I have no idea where you are nor what you are doing.";
  exit;
fi

rm -f $MKF
touch $MKF

echo "Dynamically generating $MKF for "$PROG"."
## The user should now say something like 'Oooohhhh' or 'Ahhhhh' with an awe-inspired look on his/her face.
cat <<EOF > $MKF
 
#*** other developers' libraries we need to link with
OTHER_DEVS =
 
#*** other libraries we need to link with
OTHER_MODULES = sim fsm codec FW DS
 
#*** library sources and associated headers
LIBSRCS =
HEADERS = \${LIBSRCS:%.cc=%.h}
 
#*** other targets
EXECS = $PROG_\${OS}\${LIBTYPE}
 
#*** files to be compiled for non-library targets
EXECSRCS = $HIDDIR/tmp_$PROG.cc
 
include $MLOCALPATH

INCLUDES += -I../../.. -I\${TOP}/src

ifneq (\${GREATGRANDPARENT}, src)
    CRON_EXT = \${LIBTYPE}_\${GREATGRANDPARENT}
    LOCAL_DEV_LIB = -l\${GRANDPARENT}_\${OS}\${LIBTYPE}_\${GREATGRANDPARENT}
else
    CRON_EXT = \${LIBTYPE}
endif 
 
all: \${EXECS}
	@echo \${MODULE}/\${THIS_DIR}/\${EXECS} is now ready.
 
$PROG_\${OS}\${LIBTYPE}:	\${EXECSRCS} \${MODULELIB}
	  \${CCC} -D__\${OS}__ \${CCOPTS} \${EXECOPTS} \${INCLUDES} \\
	  \${EXECSRCS} -L../.. -lcronos_\${OS}\${CRON_EXT} \\
	  \${LOCAL_DEV_LIB} \${LINKS} ${SLIBLINE} -o $PROG_\${OS}\${LIBTYPE}
EOF

#done making the source file
echo Making "$PROG"_"$OS" ...
gmake -f $MKF
echo "Done."