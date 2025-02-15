#!/bin/bash

source ./combination_test/test_assert.sh

# Attempt to overwrite a read-only file
touch protected.txt
chmod 400 protected.txt   # read-only for owner, no write permission
assert "echo 'Should fail' > protected.txt && echo 'should not print'"
# Expected: first echo fails (permission denied), second echo is skipped
# shoud not exit but rightnow we are using d_throw_error. need to change

# Attempt to append to a read-only file
assert "echo 'Append fail' >> protected.txt"
# Expected: echo fails (permission denied)

# create write-only file
touch write_only.txt
chmod 200 write_only.txt  
# Only write permission, no read

# Attempt to read from a write-only file
assert "cat < write_only.txt && echo 'should not print'"
# expected: Permission denied, second echo is skipped
assert "cat < write_only.txt || echo 'fallback after fail'"

# Attempt to write to a directory
mkdir mydir
assert "echo 'Test' > mydir"
# Expected: fails, "Is a directory" not supperted yet

# Input From Nonexistent File
assert "cat < nosuchfile123"
# expected: "No such file or directory"
# not supported yet

# Output to Nonexistent File
assert "echo 'Test' > not_existing_dir/filename"
# expected: "No such file or directory"
# not supported yet

# successful redirection
echo "Initial" > normal_file.txt
assert "echo 'Overwritten' > normal_file.txt && cat normal_file.txt"
Expected: shows "Overwritten"

# echo "Line1" > append_file.txt
# assert "echo 'Line2' >> append_file.txt && cat append_file.txt"
# Expected: "Line1\nLine2"
# commented out on 2025/1/28: diff should be NG because minishell append Line2 after bash

echo "Hello" > in_file.txt
assert "cat < in_file.txt"
# Expected: shows "Hello"

echo "abc" > chain_in.txt
assert "cat < chain_in.txt > chain_out.txt"
assert "cat chain_out.txt"
# Expected: "abc"

# Cleanup if you like:
rm -f normal_file.txt append_file.txt in_file.txt chain_in.txt chain_out.txt
rm -rf mydir protected.txt write_only.txt