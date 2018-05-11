#! /bin/sh
if [ -d /opt/FOREatm/lib ]; then
  if [ -d /opt/FOREatm/include ]; then
    echo yes
  else
    echo no
  fi
else
  echo no
fi

