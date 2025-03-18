
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra


MAIN_TARGET = differentiator
TEST_TARGET = test_expressions


SRCS = main.cpp Expression.cpp
TEST_SRCS = TestExpression.cpp Expression.cpp


OBJS = main.o Expression.o
TEST_OBJS = TestExpression.o Expression.o


all: $(MAIN_TARGET)


$(MAIN_TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(MAIN_TARGET) $(OBJS)


test: $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)


run_tests: test
	./$(TEST_TARGET)


main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

TestExpression.o: TestExpression.cpp
	$(CXX) $(CXXFLAGS) -c TestExpression.cpp -o TestExpression.o

Expression.o: Expression.cpp Expression.hpp
	$(CXX) $(CXXFLAGS) -c Expression.cpp -o Expression.o


clean:
	rm -f $(OBJS) $(TEST_OBJS) $(MAIN_TARGET) $(TEST_TARGET)