#starting

make clean

make

make test

./test_expressions 


#  basic arithmetic
./differentiator --eval "x + y" x=3 y=2
./differentiator --eval "x - y" x=3 y=2
./differentiator --eval "x * y" x=3 y=2
./differentiator --eval "x / y" x=10 y=2
./differentiator --eval "x ^ y" x=2 y=3

#  functions
./differentiator --eval "sin(x)" x=3.14159
./differentiator --eval "cos(x)" x=0
./differentiator --eval "ln(x)" x=2.718
./differentiator --eval "exp(x)" x=1

#  mixed functions
./differentiator --eval "x * sin(x) + y * cos(y)" x=3.14 y=1.57

# Differentiate arithmetic
./differentiator --diff "x * y + 5" --by x
./differentiator --diff "x ^ 2 + y" --by x
./differentiator --diff "sin(x) + cos(y)" --by x
./differentiator --diff "exp(x) * ln(y)" --by x
./differentiator --diff "x / y" --by x
./differentiator --diff "x * sin(x)" --by x
./differentiator --diff "x ^ 3 + 5 * x ^ 2 - 3 * x + 7" --by x


# Handle division by zero
./differentiator --eval "x / 0" x=5

# Handle log of zero
./differentiator --eval "ln(0)" x=0
./differentiator --eval "ln(-1)" x=-1
./differentiator --eval "ln(0.0001)" x=0.0001

# Power rule differentiation for variables
./differentiator --diff "x ^ y" --by x

# Differentiation inside trigonometric functions
./differentiator --diff "sin(x * y)" --by x