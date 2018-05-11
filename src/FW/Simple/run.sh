#!/afs/cmf/@sys/bin/tcsh -f

setenv USER     "`whoami`"
setenv PROGRAM  $1
setenv ARG1     $2
# usu. the layout file
setenv ARG2     $3

setenv GTEXEC   /afs/cmf.nrl.navy.mil/atm/signal/pnni/dev/bilal/FW/NV
setenv EXECDIR  "`pwd`"

#echo "$USER running $PROGRAM in $EXECDIR"

if ( "x$PROGRAM" != "x" ) then
  if ( -f $PROGRAM ) then
    rm -rf /tmp/$USER.pipe
    mknod /tmp/$USER.pipe p
    if ( "x$ARG2" != "x" ) then
      ( cd $GTEXEC ; $GTEXEC/NV_SunOS5 /tmp/$USER.pipe $EXECDIR/$ARG2 ) &
    else
      ( cd $GTEXEC ; $GTEXEC/NV_SunOS5 /tmp/$USER.pipe ) &
    endif
    ( cd $EXECDIR ; $EXECDIR/$PROGRAM $ARG1 /tmp/$USER.pipe)
    wait
    rm /tmp/$USER.pipe
    echo "$PROGRAM done."
  else
    echo "There is no file by the name of $PROGRAM here."
  endif
else
  echo "usage: $0 program arg layout"
endif
