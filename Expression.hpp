#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <iostream>
#include <memory>
#include <string>
#include <cmath>
#include <map>
#include <complex>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <type_traits>

template <typename T>
class Expression {
public:
    enum Type { CONSTANT, VARIABLE, OPERATION, FUNCTION };

    Expression(T value);
    Expression(const std::string& var);
    ~Expression() = default;
    Expression(const Expression& other);
    Expression(Expression&& other) noexcept;
    Expression& operator=(const Expression& other);
    Expression& operator=(Expression&& other) noexcept;

    Expression operator+(const Expression& other) const;
    Expression operator-(const Expression& other) const;
    Expression operator*(const Expression& other) const;
    Expression operator/(const Expression& other) const;
    Expression operator^(const Expression& other) const;

    static Expression sin(const Expression& expr);
    static Expression cos(const Expression& expr);
    static Expression ln(const Expression& expr);
    static Expression exp(const Expression& expr);

    T evaluate(const std::map<std::string, T>& values) const;
    Expression differentiate(const std::string& var) const;
    std::string toString() const;
    Expression substitute(const std::string& var, const Expression& value) const;
    static Expression fromString(std::string str);

private:
    Expression(char op, const Expression& lhs, const Expression& rhs);
    Expression(const std::string& func, const Expression& expr);
    static Expression simplify(const Expression& expr);
    static std::string trim(const std::string& str);
    static std::string stripOuterParens(const std::string& s);

    Type type;
    T constantValue;
    std::string variableName;
    char operation;
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;
};

#endif