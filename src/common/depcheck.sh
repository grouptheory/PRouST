#! /bin/sh 

# derive TOP as ../.. relative to this script
# the following assumes that this script is in $TOP/src/common 
#
mydir=`dirname $0`
TOP=`cd $mydir/../.. ; /bin/pwd`

#echo using TOP=$TOP

echo ""
echo "-------------------------------------------------------"
echo 'Searching for src violations in the dependency graph...'

v1=`grep 'include.*FW/'    $TOP/src/codec/*/*.h $TOP/src/codec/*/*.cc | wc -l`
v2=`grep 'include.*codec/' $TOP/src/FW/*/*.h $TOP/src/FW/*/*.cc | wc -l`

v3=`grep 'include.*fsm/'   $TOP/src/FW/*/*.h $TOP/src/FW/*/*.cc | wc -l`
v4=`grep 'include.*fsm/'   $TOP/src/codec/*/*.h $TOP/src/codec/*/*.cc | wc -l`

v5=`grep 'include.*sim/'   $TOP/src/fsm/*/*.h $TOP/src/fsm/*/*.cc | wc -l`
v6=`grep 'include.*sim/'   $TOP/src/codec/*/*.h $TOP/src/codec/*/*.cc | wc -l`
v7=`grep 'include.*sim/'   $TOP/src/FW/*/*.h $TOP/src/FW/*/*.cc | wc -l`

if [ $v1 -gt 0 ]; then
    echo ""
    echo "codec violates by depending on FW..."
    grep 'include.*FW/'    $TOP/src/codec/*/*.h $TOP/src/codec/*/*.cc
    echo ""; sleep 2;
fi

if [ $v2 -gt 0 ]; then
    echo ""
    echo "FW violates by depending on codec..."
    grep 'include.*codec/' $TOP/src/FW/*/*.h $TOP/src/FW/*/*.cc 
    echo ""; sleep 2;
fi

if [ $v3 -gt 0 ]; then
    echo ""
    echo "FW violates by depending on fsm..."
    grep 'include.*fsm/'   $TOP/src/FW/*/*.h $TOP/src/FW/*/*.cc
    echo ""; sleep 2;
fi

if [ $v4 -gt 0 ]; then
    echo ""
    echo "codec violates by depending on fsm..."
    grep 'include.*fsm/'   $TOP/src/codec/*/*.h $TOP/src/codec/*/*.cc
    echo ""; sleep 2;
fi

if [ $v5 -gt 0 ]; then
    echo ""
    echo "fsm violates by depending on sim..."
    grep 'include.*sim/'   $TOP/src/fsm/*/*.h $TOP/src/fsm/*/*.cc
    echo ""; sleep 2;
fi

if [ $v6 -gt 0 ]; then
    echo ""
    echo "codec violates by depending on sim..."
    grep 'include.*sim/'   $TOP/src/codec/*/*.h $TOP/src/codec/*/*.cc
    echo ""; sleep 2;
fi

if [ $v7 -gt 0 ]; then
    echo ""
    echo "FW violates by depending on sim..."
    grep 'include.*sim/'   $TOP/src/FW/*/*.h $TOP/src/FW/*/*.cc
    echo ""; sleep 2;
fi

echo ""
echo "-------------------------------------------------------"
echo 'Searching for dev violations in the dependency graph...'
v1=`grep 'include.*FW/' $TOP/dev/*/codec/*/*.h $TOP/dev/*/codec/*/*.cc | wc -l`
v2=`grep 'include.*codec/' $TOP/dev/*/FW/*/*.h $TOP/dev/*/FW/*/*.cc | wc -l`

v3=`grep 'include.*fsm/' $TOP/dev/*/FW/*/*.h $TOP/dev/*/FW/*/*.cc | wc -l`
v4=`grep 'include.*fsm/' $TOP/dev/*/codec/*/*.h $TOP/dev/*/codec/*/*.cc | wc -l`

v5=`grep 'include.*sim/' $TOP/dev/*/fsm/*/*.h $TOP/dev/*/fsm/*/*.cc | wc -l`
v6=`grep 'include.*sim/' $TOP/dev/*/codec/*/*.h $TOP/dev/*/codec/*/*.cc | wc -l`
v7=`grep 'include.*sim/' $TOP/dev/*/FW/*/*.h $TOP/dev/*/FW/*/*.cc | wc -l`

if [ $v1 -gt 0 ]; then
    echo ""
    echo "codec violates by depending on FW..."
    grep 'include.*FW/' $TOP/dev/*/codec/*/*.h $TOP/dev/*/codec/*/*.cc
    echo ""; sleep 2;
fi

if [ $v2 -gt 0 ]; then
    echo ""
    echo "FW violates by depending on codec..."
    grep 'include.*codec/' $TOP/dev/*/FW/*/*.h $TOP/dev/*/FW/*/*.cc 
    echo ""; sleep 2;
fi

if [ $v3 -gt 0 ]; then
    echo ""
    echo "FW violates by depending on fsm..."
    grep 'include.*fsm/' $TOP/dev/*/FW/*/*.h $TOP/dev/*/FW/*/*.cc
    echo ""; sleep 2;
fi

if [ $v4 -gt 0 ]; then
    echo ""
    echo "codec violates by depending on fsm..."
    grep 'include.*fsm/' $TOP/dev/*/codec/*/*.h $TOP/dev/*/codec/*/*.cc
    echo ""; sleep 2;
fi

if [ $v5 -gt 0 ]; then
    echo ""
    echo "fsm violates by depending on sim..."
    grep 'include.*sim/' $TOP/dev/*/fsm/*/*.h $TOP/dev/*/fsm/*/*.cc
    echo ""; sleep 2;
fi

if [ $v6 -gt 0 ]; then
    echo ""
    echo "codec violates by depending on sim..."
    grep 'include.*sim/' $TOP/dev/*/codec/*/*.h $TOP/dev/*/codec/*/*.cc
    echo ""; sleep 2;
fi

if [ $v7 -gt 0 ]; then
    echo ""
    echo "FW violates by depending on sim..."
    grep 'include.*sim/' $TOP/dev/*/FW/*/*.h $TOP/dev/*/FW/*/*.cc
    echo ""; sleep 2;
fi

echo "Dependency graph violations checking completed."
