#!/bin/bash

source ./combination_test/test_assert.sh

assert 'a|b|c|'
assert 'a|b|c|>d'
assert 'a|b|c'
assert 'a|b|'
assert 'a|b'
assert 'aa&&'
assert 'aa&&aa'
assert 'aa&&aa&&'
assert 'aa&&aa&&&'
assert 'aa&&&'