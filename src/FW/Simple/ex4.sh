#!/bin/csh

rm -f core

foreach T ( 12 14 16 18 20 22 24 26 28 30 32 64 128 256 512 )
	echo "Running example4_SunOS5 $T ..."
	example4_SunOS5 $T >& /dev/null
	if ( -f core ) then
	  echo "Core file found on iteration $T ... start debugging."
	  exit
	endif
end

if ( ! -f core ) then
echo "Everything appears to be working fine ... better find some new tests."
endif
