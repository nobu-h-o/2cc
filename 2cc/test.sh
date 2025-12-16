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
assert 0 "main() { return 0; }"
assert 42 "main() { return 42; }"

# Additional tests for more complex expressions
assert 15 "main() { return 3 * (2 + 3); }"
assert 4 "main() { return 8 / (1 + 1); }"
assert 10 "main() { return (7 + 3) * (9 - 8) / 1; }"
assert 10 "main() { return 10 * (1 + 2) / 3; }"

# Variable assignment with return
assert 5 "main() { x=5; return x; }"
assert 3 "main() { x=3; return x; }"
assert 100 "main() { x=100; return x; }"

# Variable with expression assignment and return
assert 8 "main() { x=3+5; return x; }"
assert 15 "main() { x=3*5; return x; }"
assert 2 "main() { x=8/4; return x; }"
assert 7 "main() { x=10-3; return x; }"

# Return keyword tests
assert 42 "main() { return 42; }"
assert 5 "main() { return 5; }"
assert 15 "main() { return 3+4+8; }"
assert 24 "main() { return 2*3*4; }"

# Multiple variables with return
assert 10 "main() { x=10; return x; }"
assert 15 "main() { x=3; y=5; return x*y; }"
assert 8 "main() { x=10; y=2; return x-y; }"
assert 5 "main() { x=2; y=3; return x+y; }"

# Complex variable expressions
assert 17 "main() { x=3; y=4; z=5; return x*y+z; }"
assert 17 "main() { x=10; y=7; return x+y; }"
assert 20 "main() { x=5; y=4; return x*y*1; }"

# Comparison operators
assert 1 "main() { return 3 < 5; }"
assert 0 "main() { return 5 < 3; }"
assert 1 "main() { return 5 > 3; }"
assert 0 "main() { return 3 > 5; }"
assert 1 "main() { return 3 <= 3; }"
assert 1 "main() { return 3 <= 5; }"
assert 0 "main() { return 5 <= 3; }"
assert 1 "main() { return 5 >= 5; }"
assert 1 "main() { return 5 >= 3; }"
assert 0 "main() { return 3 >= 5; }"
assert 1 "main() { return 5 == 5; }"
assert 0 "main() { return 5 == 3; }"
assert 1 "main() { return 5 != 3; }"
assert 0 "main() { return 5 != 5; }"

# While loops
assert 10 "main() { x=0; while (x < 10) { x=x+1; } return x; }"
assert 5 "main() { x=0; while (x < 5) { x=x+1; } return x; }"
assert 0 "main() { x=0; while (x < 0) { x=x+1; } return x; }"
assert 55 "main() { sum=0; i=1; while (i <= 10) { sum=sum+i; i=i+1; } return sum; }"
assert 100 "main() { x=0; while (x < 100) { x=x+1; } return x; }"

# For loops
assert 10 "main() { for (x=0; x < 10; x=x+1) { y=x; } return x; }"
assert 15 "main() { sum=0; for (i=1; i <= 5; i=i+1) { sum=sum+i; } return sum; }"
assert 55 "main() { sum=0; for (i=1; i <= 10; i=i+1) { sum=sum+i; } return sum; }"
assert 20 "main() { x=0; for (i=0; i < 4; i=i+1) { x=x+5; } return x; }"

# Nested loops
assert 100 "main() { sum=0; for (i=0; i < 10; i=i+1) { for (j=0; j < 10; j=j+1) { sum=sum+1; } } return sum; }"
assert 25 "main() { sum=0; i=0; while (i < 5) { j=0; while (j < 5) { sum=sum+1; j=j+1; } i=i+1; } return sum; }"

# Print function tests
assert_output "42" "main() { print(42); }"
assert_output "0" "main() { print(0); }"
assert_output "100" "main() { print(100); }"

# Print with variables
assert_output "10" "main() { x=10; print(x); }"
assert_output "5" "main() { x=5; print(x); }"
assert_output "42" "main() { x=10; y=32; print(x+y); }"
assert_output "30" "main() { x=5; y=6; print(x*y); }"
assert_output "3" "main() { x=10; y=7; print(x-y); }"
assert_output "2" "main() { x=8; y=4; print(x/y); }"

# Print with expressions
assert_output "25" "main() { x=5; print(x*x); }"
assert_output "17" "main() { x=3; y=4; z=5; print(x*y+z); }"
assert_output "23" "main() { x=10; y=7; z=6; print(x+y+z); }"

# Print in loops
assert_output "0
1
2
3
4" "main() { x=0; while(x < 5) { print(x); x=x+1; } }"

assert_output "1
2
3
4
5" "main() { for (x=1; x <= 5; x=x+1) { print(x); } }"

assert_output "55" "main() { sum=0; for (i=1; i <= 10; i=i+1) { sum=sum+i; } print(sum); }"

# Print multiple values
assert_output "10
20
30" "main() { print(10); print(20); print(30); }"

# Print with comparisons
assert_output "1" "main() { print(5 > 3); }"
assert_output "0" "main() { print(3 > 5); }"
assert_output "1" "main() { print(5 == 5); }"
assert_output "0" "main() { print(5 == 3); }"

# Mixed print and return
assert_output "42" "main() { x=42; print(x); return 0; }"
assert_output "1
2
3" "main() { for (i=1; i <= 3; i=i+1) { print(i); } return 0; }"

# Print with values > 8 bits (> 255)
assert_output "256" "main() { print(256); }"
assert_output "1000" "main() { print(1000); }"
assert_output "65535" "main() { print(65535); }"
assert_output "500" "main() { x=250; y=250; print(x+y); }"
assert_output "1024" "main() { x=32; print(x*x); }"

# Global variables
assert_output "105" "g = 100; add(x) { return x + g; } main() { print(add(5)); return 0; }"
assert 14 "g = 10; add(x) { return x + g; } main() { return add(4); }"

# Multiple functions
assert 14 "mul(a, b) { return a * b; } add(a, b) { return a + b; } main() { x = mul(3, 4); y = add(x, 2); return y; }"
assert_output "8
12" "double(x) { return x * 2; } triple(x) { return x * 3; } main() { print(double(4)); print(triple(4)); return 0; }"

echo "All tests succeeded 🎉"
