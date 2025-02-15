#!/bin/bash

source ./combination_test/test_assert.sh

i=0
assert_error_handle $((++i)) '"'
assert_error_handle $((++i)) "'"
assert_error_handle $((++i)) ')'
assert_error_handle $((++i)) '|'
assert_error_handle $((++i)) '>'
assert_error_handle $((++i)) '<'
assert_error_handle $((++i)) '>>'
assert_error_handle $((++i)) '<<'
assert_error_handle $((++i)) '(()'
assert_error_handle $((++i)) '()'
assert_error_handle $((++i)) 'asdf " asfd " afd "'
assert_error_handle $((++i)) '(cat))'
assert_error_handle $((++i)) '((cat)'
assert_error_handle $((++i)) '(cat) ls'
assert_error_handle $((++i)) '>abc (cat) ' #error should cause
assert_error_handle $((++i)) '<abc (cat) ' #bash doesn't throw error but this is not subshell!
assert_error_handle $((++i)) 'ls && >abc (cat)' #error should cause
assert_error_handle $((++i)) ')()()(' #error should cause
assert_error_handle $((++i)) ')(' #error should cause
assert_error_handle $((++i)) '(cat))(' #error should cause
assert_error_handle $((++i)) 'ls || >abc (cat)' #error should cause
#bash の不思議
# < infile (cat) > outfile は許されるが、> outfile (cat) < infile は許されない
# < infile (cat) > outfile は許されるが、> outfile (cat)  は許されない
# < infile (cat) > outfile は許されるが、> outfile (cat) < infile は許されない

