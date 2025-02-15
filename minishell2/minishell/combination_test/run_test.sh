#!/bin/bash

test_result() {
	echo "=== Test Summary ==="
	passed=$(grep -c "diff OK.*status OK" result)
	total=$(wc -l < result)
	failed=$((total - passed))
	echo "Tests completed: $total"
	echo "Tests passed: $passed"
	echo "Tests failed: $failed"
	true > result
}

echo_start_test() {

	echo -e "\e[36m
 ██████   ██████   ██████   ██████      ██   ██   ████    ██  ██    ████    ██████   ██  ██   ██████   ██       ██
   ██     ██       ██  ██     ██        ███ ███    ██     ███ ██     ██     ██  ██   ██  ██   ██       ██       ██
   ██     ██       ██         ██        ███████    ██     ██████     ██     ██       ██  ██   ██       ██       ██
   ██     ██████   ██████     ██        ██ █ ██    ██     ██████     ██     ██████   ██████   ██████   ██       ██
   ██     ██           ██     ██        ██   ██    ██     ██ ███     ██         ██   ██  ██   ██       ██       ██
   ██     ██       ██  ██     ██        ██   ██    ██     ██  ██     ██     ██  ██   ██  ██   ██       ██       ██
   ██     ██████   ██████     ██        ██   ██   ████    ██  ██    ████    ██████   ██  ██   ██████   ██████   ██████

\e[0m"
}

make
if [ $? -ne 0 ]; then
	echo "make is failed"
	exit 1
else 
	source ./combination_test/test_assert.sh || { echo "Failed to source test_assert.sh"; exit 1; }
	echo_start_test
	echo "expand test"
	./combination_test/expand_test.sh
	test_result
	echo "builtin test"
	./combination_test/builtin_test.sh
	test_result
	echo "heredoc test"
	./combination_test/heredoc_test.sh
	test_result
	echo "redirect test"
	./combination_test/redirect_test.sh
	test_result
	echo "exec path test"
	./combination_test/exec_path_test.sh
	test_result
	echo "syntax error test"
	./combination_test/syntax_error_test.sh
	test_result
	echo "test is completed"

fi

trap 'cleanup || { echo "Error: cleanup failed"; exit 1; }' EXIT