#!/bin/bash

source ./combination_test/test_assert.sh

echo "=== Test: '.' and '..' as commands ==="

# assert "."
# the '.' builtin is not supported" so we expect "command not found" => exit 127

assert ".."
assert "..."
# Expect 127 (command not found) for "..."

echo "=== Test: PATH is empty ==="

# Temporarily override PATH for the test
assert "unset PATH && ls"
# Expected: 'ls' not found => 'command not found' => exit 127

echo "=== Test: Non-ENOENT error on access() ==="
mkdir -p no_read_dir/sub
chmod 000 no_read_dir
assert "./no_read_dir"
assert "./no_read_dir/sub/echo"
# expected: no such file or directory, exit 126

echo "=== Put current directory first in PATH and execute the leftmost executable file==="
echo "手動で実行可能ファイル(ls)を作成して確認"
export PATH="./:$PATH"
assert "ls"
assert "./ls///"
# not sure if we should handle trailing slashes

echo "=== Test: command not found => exit 127 ==="
assert "zzzNonExistentCommand123"
# expected: command not found, exit 127

# Cleanup
chmod 755 no_read_dir
rm -rf no_read_dir