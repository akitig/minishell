#!/bin/sh
echo $SHELL

cat answer.sh
cat ~/Desktop/42/honda/playground/answer.sh

echo "test
# > 
# bash: unexpected EOF while looking for matching `"'
# bash: syntax error: unexpected end of file

echo 'hello'
# hello

echo "'hello'"
# 'hello'

echo '"hello"'
# "hello"

echo "'hello"
# 'hello

echo "'hello"'
# > 
# bash: unexpected EOF while looking for matching `''
# bash: syntax error: unexpected end of file

echo \
# >

echo "\"
#>

echo "\\"
# \

echo ;
#

echo ";"
# ;

echo *
# playground README.md sample

echo "*"
# *

echo "$SHELL"
# /bin/bash

echo '$SHELL'
# $SHELL

echo "cat *"
# cat *

echo "cat $SHELL"
# cat /bin/bash

# メタ文字
# * + ? $ ^ . () | \ {} [


# ------- 
# redirect
# --------

cat test.txt > test2.txt
cat test2.txt 
# this is test!

cat < test.txt
# this is test!

echo "this is add test" >> test.txt 
# cat test.txt
# this is test!
# this is add test

cat << a
# > now is test
# > test
# > a
# now is test
# test

cat answer.sh | wc -l
# 86

$?
# 0: command not found

echo $?
# 0