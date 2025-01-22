#!/bin/bash

echo '1=1の終了ステータス'
[ 1 = 1 ]
echo $?

echo '1=2の終了ステータス'
[ 1 = 2 ]
echo $?