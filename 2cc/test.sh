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

# Comparison operators
assert 1 "return 3 < 5;"
assert 0 "return 5 < 3;"
assert 1 "return 5 > 3;"
assert 0 "return 3 > 5;"
assert 1 "return 3 <= 3;"
assert 1 "return 3 <= 5;"
assert 0 "return 5 <= 3;"
assert 1 "return 5 >= 5;"
assert 1 "return 5 >= 3;"
assert 0 "return 3 >= 5;"
assert 1 "return 5 == 5;"
assert 0 "return 5 == 3;"
assert 1 "return 5 != 3;"
assert 0 "return 5 != 5;"

# While loops
assert 10 "x=0; while (x < 10) { x=x+1; } return x;"
assert 5 "x=0; while (x < 5) { x=x+1; } return x;"
assert 0 "x=0; while (x < 0) { x=x+1; } return x;"
assert 55 "sum=0; i=1; while (i <= 10) { sum=sum+i; i=i+1; } return sum;"
assert 100 "x=0; while (x < 100) { x=x+1; } return x;"

# For loops
assert 10 "for (x=0; x < 10; x=x+1) { y=x; } return x;"
assert 15 "sum=0; for (i=1; i <= 5; i=i+1) { sum=sum+i; } return sum;"
assert 55 "sum=0; for (i=1; i <= 10; i=i+1) { sum=sum+i; } return sum;"
assert 20 "x=0; for (i=0; i < 4; i=i+1) { x=x+5; } return x;"

# Nested loops
assert 100 "sum=0; for (i=0; i < 10; i=i+1) { for (j=0; j < 10; j=j+1) { sum=sum+1; } } return sum;"
assert 25 "sum=0; i=0; while (i < 5) { j=0; while (j < 5) { sum=sum+1; j=j+1; } i=i+1; } return sum;"

echo "All tests succeeded 🎉"
