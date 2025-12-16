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

assert_output() {
  expected="$1"
  input="$2"

  ./2cc "$input" > tmp.s
  gcc -o tmp tmp.s
  actual=$(./tmp)

  if [ "$actual" = "$expected" ]; then
    echo "$input => \"$actual\""
  else
    echo "$input => \"$actual\" received, but expected \"$expected\" ❌"
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

# Print function tests
assert_output "42" "print(42);"
assert_output "0" "print(0);"
assert_output "100" "print(100);"

# Print with variables
assert_output "10" "x=10; print(x);"
assert_output "5" "x=5; print(x);"
assert_output "42" "x=10; y=32; print(x+y);"
assert_output "30" "x=5; y=6; print(x*y);"
assert_output "3" "x=10; y=7; print(x-y);"
assert_output "2" "x=8; y=4; print(x/y);"

# Print with expressions
assert_output "25" "x=5; print(x*x);"
assert_output "17" "x=3; y=4; z=5; print(x*y+z);"
assert_output "23" "x=10; y=7; z=6; print(x+y+z);"

# Print in loops
assert_output "0
1
2
3
4" "x=0; while(x < 5) { print(x); x=x+1; }"

assert_output "1
2
3
4
5" "for (x=1; x <= 5; x=x+1) { print(x); }"

assert_output "55" "sum=0; for (i=1; i <= 10; i=i+1) { sum=sum+i; } print(sum);"

# Print multiple values
assert_output "10
20
30" "print(10); print(20); print(30);"

# Print with comparisons
assert_output "1" "print(5 > 3);"
assert_output "0" "print(3 > 5);"
assert_output "1" "print(5 == 5);"
assert_output "0" "print(5 == 3);"

# Mixed print and return
assert_output "42" "x=42; print(x); return 0;"
assert_output "1
2
3" "for (i=1; i <= 3; i=i+1) { print(i); } return 0;"

echo "All tests succeeded 🎉"
