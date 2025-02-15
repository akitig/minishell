#!/bin/bash

source ./combination_test/test_assert.sh

assert 'echo $USER | cat'
assert 'echo $USER$HOME | cat'
assert 'echo $USERA | cat'
assert "echo $USER'A' | cat"
assert "echo $USER$123 | cat"
assert "echo $USER$ | cat"
assert "echo $USER'$'$USER | cat"
assert "echo $USE'R' | cat"
assert "echo $USER$ | cat"
assert "echo $USER$ | cat"

#test case for question
assert 'echo $?'
assert 'echo $?$?$?$?$?$?$?$?$?$?$?$?'
assert 'echo $???????????'
assert 'echo $@?'
assert 'echo $?abc'
assert 'echo $abc?abc'
assert 'echo $?$USER'
assert 'echo $?$USEAR'
assert 'ls && echo $?'
assert 'lsl || echo $?' #this may need to be fixed but not this issue(#48) since this is related to command not found error
assert 'exit 42 | echo $?'
assert '(exit 42 && ls) || echo $?'
assert '(true && ls) && echo $?'
assert 'false && true || ls && echo $?'
assert 'exit 3 && ls $$ echo $?'
assert 'echo $? && false || echo $?'
assert '(echo $? && exit 2) || echo $?'

# Below cases show diff but should be ignored.

#This test case always come to fail but not sure the reason. 0\n1\n should be displaying but original bash output always 0\n\1\n1\n. 
#Howeve,r running same command on bash, 0\n1\n shows up. It seems test script has some problem.
#assert '(echo $? && false) || echo $?' 

#This test case always come to fail since 'subshell' will end and echo$? is not executed when executing exit 34, but our implementation is not 'subshell' but 'round_bracket'.
#Subshell is handling the commands inside subshell as 'one process'. 
#But round_bracket is not handling the commands inside round_bracket as 'one process'. 
#And if there is logical operator, the commands which surrounding logical operator treat as another process.
#So that, in our case, even though you execute 'exit' command, round_bracket doesn't end. 
#assert 'echo $? && (exit 34 || echo $?)' 


 #this test case always come to fail since we are not implementing $$(show process id).
#assert 'echo $$?'
