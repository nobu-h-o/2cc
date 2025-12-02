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

# Variable assignment with return
assert 5 "x=5; return x;"
assert 3 "x=3; return x;"
assert 100 "x=100; return x;"

# Variable with expression assignment and return
assert 8 "x=3+5; return x;"
assert 15 "x=3*5; return x;"
assert 2 "x=8/4; return x;"
assert 7 "x=10-3; return x;"

# Return keyword tests
assert 42 "return 42;"
assert 5 "return 5;"
assert 15 "return 3+4+8;"
assert 24 "return 2*3*4;"

# Multiple variables with return
assert 10 "x=10; return x;"
assert 15 "x=3; y=5; return x*y;"
assert 8 "x=10; y=2; return x-y;"
assert 5 "x=2; y=3; return x+y;"

# Complex variable expressions
assert 17 "x=3; y=4; z=5; return x*y+z;"
assert 17 "x=10; y=7; return x+y;"
assert 20 "x=5; y=4; return x*y*1;"

echo "All tests succeeded 🎉"
