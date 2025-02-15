#!/bin/bash

cleanup() {
	rm -f cmp out result
}

error_echo() 
{
	echo -e -n "\e[1;31m$1\e[0m"
}

error_echo_w_linebreak() 
{
	echo -e "\e[1;31m$1\e[0m"
}

assert() {
	printf '%-30s:' "\"$1\""
	# exit status
	echo -n -e "$1" | bash --norc --noprofile >cmp 2>&-
	expected=$?
	echo -n -e "$1" | ./minishell >out 2>&-
	actual=$?

	diff cmp out >/dev/null && echo -n '  diff OK' | tee -a result || error_echo '  diff NG' | tee -a result

	if [ "$actual" = "$expected" ]; then
		echo '  status OK' | tee -a result
	else
		error_echo_w_linebreak "  status NG, expected $expected but got $actual" | tee -a result
	fi
	echo
}

assert_error_handle() {
	EXPECTED_ERRORS_FILE="./combination_test/resources/expected_syntax_error_msg.txt"
	printf '%-30s:' "\"$2\""
	# exit status
	echo -n -e "$2" | bash --norc --noprofile 2>cmp 1>&-
	expected=$?
	echo -n -e "$2" | ./minishell 2>out 1>&-
	actual=$?
	local error_number=$1
	expected_error=$(awk -v RS="---" -v num="$error_number" '
		$1 == num { $1=""; print substr($0,2) }
	' "$EXPECTED_ERRORS_FILE")

	if [ -z "$expected_error" ]; then
		echo "Error: Expected error message is blank for error_number=$error_number" >&2
		return 1

	fi
	# diff cmp out >/dev/null && echo -n '  diff OK' | tee -a result || error_echo '  diff NG' | tee -a result
	if echo "$expected_error" | grep -qF -f - out; then
		echo -n '  diff OK'
	else
		error_echo '  diff NG'
	fi

	if [ "$actual" = "$expected" ]; then
		echo '  status OK' | tee -a result
	else
		error_echo_w_linebreak "  status NG, expected $expected but got $actual" | tee -a result
	fi
	echo
}
