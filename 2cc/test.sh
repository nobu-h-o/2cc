assert() {
  expected="$1"
  input="$2"

  ./2cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $actual received, but expected $expected ❌"
    exit 1
  fi
}

# Recognition
assert 0 "0"
assert 42 "42"

# Additional tests for more complex expressions
assert 15 "3 * (2 + 3)"
assert 4 "8 / (1 + 1)"
assert 10 "(7 + 3) * (9 - 8) / 1"
assert 10 "10 * (1 + 2) / 3"

echo "All tests succeeded 🎉"
