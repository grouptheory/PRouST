#! /bin/sh

for T in *.NSC ; do
   echo "Testing $T, if you see any output, the test failed."
   if ( ! -f golden/$T ) then
     echo "golden/$T doesn't exist!  Nothing to test against."
   else
     diff $T golden/$T
   fi
   echo "---------------------------------------------------"
done
