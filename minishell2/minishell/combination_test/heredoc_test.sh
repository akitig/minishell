#!/bin/bash

source ./combination_test/test_assert.sh

assert 'cat << abc 
abc
'
assert 'cat << abc 
abcd
abc
'
assert 'cat << abc 
ab c
abc
'
assert 'cat << abc 
 abc
abc
'
assert 'cat << abc 
 abc
gabca
abc
'
assert 'cat << eof <<eof<<eof
eof
abc
eof
abcdefg
eof
'
assert 'cat Makefile << eof includes/utils.h<<eof<<eof
eof
abc
eof
abcdefg
eof
'
assert 'cat << eof
$USER
$aUSER
$USER"abcd"
eof
'
assert 'cat << eof
"$USER"
$aUSER
$USERabcd
"$USER
eof
'
assert 'cat << eof
"a
eof
'
assert "cat << eof
'a
eof
"
assert "cat << eof
'a
eof
"
#below cases are failed and need to be fixed.
# assert "cat << eof
# $'
# eof
# "
# assert "cat << eof
# $+
# eof
# "