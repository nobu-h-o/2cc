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
assert 0 "int main() { return 0; }"
assert 42 "int main() { return 42; }"

# Additional tests for more complex expressions
assert 15 "int main() { return 3 * (2 + 3); }"
assert 4 "int main() { return 8 / (1 + 1); }"
assert 10 "int main() { return (7 + 3) * (9 - 8) / 1; }"
assert 10 "int main() { return 10 * (1 + 2) / 3; }"

# Variable assignment with return
assert 5 "int main() { int x=5; return x; }"
assert 3 "int main() { int x=3; return x; }"
assert 100 "int main() { int x=100; return x; }"

# Variable with expression assignment and return
assert 8 "int main() { int x=3+5; return x; }"
assert 15 "int main() { int x=3*5; return x; }"
assert 2 "int main() { int x=8/4; return x; }"
assert 7 "int main() { int x=10-3; return x; }"

# Return keyword tests
assert 42 "int main() { return 42; }"
assert 5 "int main() { return 5; }"
assert 15 "int main() { return 3+4+8; }"
assert 24 "int main() { return 2*3*4; }"

# Multiple variables with return
assert 10 "int main() { int x=10; return x; }"
assert 15 "int main() { int x=3; int y=5; return x*y; }"
assert 8 "int main() { int x=10; int y=2; return x-y; }"
assert 5 "int main() { int x=2; int y=3; return x+y; }"

# Complex variable expressions
assert 17 "int main() { int x=3; int y=4; int z=5; return x*y+z; }"
assert 17 "int main() { int x=10; int y=7; return x+y; }"
assert 20 "int main() { int x=5; int y=4; return x*y*1; }"

# Comparison operators
assert 1 "int main() { return 3 < 5; }"
assert 0 "int main() { return 5 < 3; }"
assert 1 "int main() { return 5 > 3; }"
assert 0 "int main() { return 3 > 5; }"
assert 1 "int main() { return 3 <= 3; }"
assert 1 "int main() { return 3 <= 5; }"
assert 0 "int main() { return 5 <= 3; }"
assert 1 "int main() { return 5 >= 5; }"
assert 1 "int main() { return 5 >= 3; }"
assert 0 "int main() { return 3 >= 5; }"
assert 1 "int main() { return 5 == 5; }"
assert 0 "int main() { return 5 == 3; }"
assert 1 "int main() { return 5 != 3; }"
assert 0 "int main() { return 5 != 5; }"

# While loops
assert 10 "int main() { int x=0; while (x < 10) { x=x+1; } return x; }"
assert 5 "int main() { int x=0; while (x < 5) { x=x+1; } return x; }"
assert 0 "int main() { int x=0; while (x < 0) { x=x+1; } return x; }"
assert 55 "int main() { int sum=0; int i=1; while (i <= 10) { sum=sum+i; i=i+1; } return sum; }"
assert 100 "int main() { int x=0; while (x < 100) { x=x+1; } return x; }"

# For loops
assert 10 "int main() { int x=0; int y=0; for (x=0; x < 10; x=x+1) { y=x; } return x; }"
assert 15 "int main() { int sum=0; int i=0; for (i=1; i <= 5; i=i+1) { sum=sum+i; } return sum; }"
assert 55 "int main() { int sum=0; int i=0; for (i=1; i <= 10; i=i+1) { sum=sum+i; } return sum; }"
assert 20 "int main() { int x=0; int i=0; for (i=0; i < 4; i=i+1) { x=x+5; } return x; }"

# Nested loops
assert 100 "int main() { int sum=0; int i=0; int j=0; for (i=0; i < 10; i=i+1) { for (j=0; j < 10; j=j+1) { sum=sum+1; } } return sum; }"
assert 25 "int main() { int sum=0; int i=0; int j=0; while (i < 5) { j=0; while (j < 5) { sum=sum+1; j=j+1; } i=i+1; } return sum; }"

# Print function tests
assert_output "42" "int main() { print(42); return 0; }"
assert_output "0" "int main() { print(0); return 0; }"
assert_output "100" "int main() { print(100); return 0; }"

# Print with variables
assert_output "10" "int main() { int x=10; print(x); return 0; }"
assert_output "5" "int main() { int x=5; print(x); return 0; }"
assert_output "42" "int main() { int x=10; int y=32; print(x+y); return 0; }"
assert_output "30" "int main() { int x=5; int y=6; print(x*y); return 0; }"
assert_output "3" "int main() { int x=10; int y=7; print(x-y); return 0; }"
assert_output "2" "int main() { int x=8; int y=4; print(x/y); return 0; }"

# Print with expressions
assert_output "25" "int main() { int x=5; print(x*x); return 0; }"
assert_output "17" "int main() { int x=3; int y=4; int z=5; print(x*y+z); return 0; }"
assert_output "23" "int main() { int x=10; int y=7; int z=6; print(x+y+z); return 0; }"

# Print in loops
assert_output "0
1
2
3
4" "int main() { int x=0; while(x < 5) { print(x); x=x+1; } return 0; }"

assert_output "1
2
3
4
5" "int main() { int x=0; for (x=1; x <= 5; x=x+1) { print(x); } return 0; }"

assert_output "55" "int main() { int sum=0; int i=0; for (i=1; i <= 10; i=i+1) { sum=sum+i; } print(sum); return 0; }"

# Print multiple values
assert_output "10
20
30" "int main() { print(10); print(20); print(30); return 0; }"

# Print with comparisons
assert_output "1" "int main() { print(5 > 3); return 0; }"
assert_output "0" "int main() { print(3 > 5); return 0; }"
assert_output "1" "int main() { print(5 == 5); return 0; }"
assert_output "0" "int main() { print(5 == 3); return 0; }"

# Mixed print and return
assert_output "42" "int main() { int x=42; print(x); return 0; }"
assert_output "1
2
3" "int main() { int i=0; for (i=1; i <= 3; i=i+1) { print(i); } return 0; }"

# Print with values > 8 bits (> 255)
assert_output "256" "int main() { print(256); return 0; }"
assert_output "1000" "int main() { print(1000); return 0; }"
assert_output "65535" "int main() { print(65535); return 0; }"
assert_output "500" "int main() { int x=250; int y=250; print(x+y); return 0; }"
assert_output "1024" "int main() { int x=32; print(x*x); return 0; }"

# Global variables
assert_output "105" "int g = 100; int add(int x) { return x + g; } int main() { print(add(5)); return 0; }"
assert 14 "int g = 10; int add(int x) { return x + g; } int main() { return add(4); }"

# Multiple functions
assert 14 "int mul(int a, int b) { return a * b; } int add(int a, int b) { return a + b; } int main() { int x = mul(3, 4); int y = add(x, 2); return y; }"
assert_output "8
12" "int double_it(int x) { return x * 2; } int triple(int x) { return x * 3; } int main() { print(double_it(4)); print(triple(4)); return 0; }"

# Semantic error tests
assert_error() {
  input="$1"
  expected_error="$2"

  output=$(./2cc "$input" 2>&1)
  exit_code=$?

  if [ "$exit_code" != "1" ]; then
    echo "Expected exit code 1 but got $exit_code for: $input ❌"
    exit 1
  fi

  if echo "$output" | grep -q "$expected_error"; then
    echo "$input => error detected"
  else
    echo "Expected error containing '$expected_error' but got '$output' for: $input ❌"
    exit 1
  fi
}

# Test undefined variable
assert_error "int main() { return x; }" "undefined variable 'x'"

# Test undefined function
assert_error "int main() { return foo(); }" "undefined function 'foo'"

# Test duplicate variable
assert_error "int main() { int x = 1; int x = 2; return x; }" "variable 'x' already declared"

# Test duplicate function
assert_error "int foo() { return 1; } int foo() { return 2; } int main() { return foo(); }" "function 'foo' already defined"

# Test argument count mismatch
assert_error "int add(int a, int b) { return a + b; } int main() { return add(1); }" "function 'add' expects 2 arguments, got 1"
assert_error "int add(int a, int b) { return a + b; } int main() { return add(1, 2, 3); }" "function 'add' expects 2 arguments, got 3"

echo "All tests succeeded 🎉"
