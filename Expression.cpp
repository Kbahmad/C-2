#include "Expression.hpp"

template <typename T>
Expression<T>::Expression(T value)
    : type(CONSTANT), constantValue(value), variableName(), operation('\0'), left(nullptr), right(nullptr) {
}

template <typename T>
Expression<T>::Expression(const std::string& var)
    : type(VARIABLE), constantValue(T(0)), variableName(var), operation('\0'), left(nullptr), right(nullptr) {
}

template <typename T>
Expression<T>::Expression(const Expression& other)
    : type(other.type),
      constantValue(other.constantValue),
      variableName(other.variableName),
      operation(other.operation),
      left(other.left),
      right(other.right) {
}

template <typename T>
Expression<T>::Expression(Expression&& other) noexcept
    : type(other.type),
      constantValue(other.constantValue),
      variableName(std::move(other.variableName)),
      operation(other.operation),
      left(std::move(other.left)),
      right(std::move(other.right)) {
}

template <typename T>
Expression<T>& Expression<T>::operator=(const Expression& other) {
    if (this != &other) {
        type = other.type;
        constantValue = other.constantValue;
        variableName = other.variableName;
        operation = other.operation;
        left = other.left;
        right = other.right;
    }
    return *this;
}

template <typename T>
Expression<T>& Expression<T>::operator=(Expression&& other) noexcept {
    if (this != &other) {
        left.reset();
        right.reset();
        type = other.type;
        constantValue = other.constantValue;
        variableName = std::move(other.variableName);
        operation = other.operation;
        left = std::move(other.left);
        right = std::move(other.right);
    }
    return *this;
}

template <typename T>
Expression<T>::Expression(char op, const Expression& lhs, const Expression& rhs)
    : type(OPERATION),
      constantValue(T(0)),
      variableName(),
      operation(op),
      left(std::make_shared<Expression>(lhs)),
      right(std::make_shared<Expression>(rhs)) {
}

template <typename T>
Expression<T>::Expression(const std::string& func, const Expression& expr)
    : type(FUNCTION),
      constantValue(T(0)),
      variableName(func),
      operation('\0'),
      left(std::make_shared<Expression>(expr)),
      right(nullptr) {
}

template <typename T>
Expression<T> Expression<T>::operator+(const Expression& other) const {
    return simplify(Expression('+', *this, other));
}

template <typename T>
Expression<T> Expression<T>::operator-(const Expression& other) const {
    return simplify(Expression('-', *this, other));
}

template <typename T>
Expression<T> Expression<T>::operator*(const Expression& other) const {
    return simplify(Expression('*', *this, other));
}

template <typename T>
Expression<T> Expression<T>::operator/(const Expression& other) const {
    return simplify(Expression('/', *this, other));
}

template <typename T>
Expression<T> Expression<T>::operator^(const Expression& other) const {
    return simplify(Expression('^', *this, other));
}

template <typename T>
Expression<T> Expression<T>::sin(const Expression& expr) {
    return Expression("sin", expr);
}

template <typename T>
Expression<T> Expression<T>::cos(const Expression& expr) {
    return Expression("cos", expr);
}

template <typename T>
Expression<T> Expression<T>::ln(const Expression& expr) {
    return Expression("ln", expr);
}

template <typename T>
Expression<T> Expression<T>::exp(const Expression& expr) {
    return Expression("exp", expr);
}

template <typename T>
T Expression<T>::evaluate(const std::map<std::string, T>& values) const {
    if (type == CONSTANT) return constantValue;
    if (type == VARIABLE) {
        return values.at(variableName);
    }
    if (type == FUNCTION) {
        T val = left->evaluate(values);
        if (variableName == "sin") return std::sin(val);
        if (variableName == "cos") return std::cos(val);
        if (variableName == "ln") {
            if constexpr (std::is_floating_point_v<T>) {
                if (val <= T(0)) {
                    std::cerr << "Warning: Logarithm of zero or negative number is undefined. Returning -inf.\n";
                    return -std::numeric_limits<T>::infinity();
                }
            }
            return std::log(val);
        }
        if (variableName == "exp") return std::exp(val);
        throw std::runtime_error("Unknown function");
    }
    T leftVal = left ? left->evaluate(values) : T(0);
    T rightVal = right ? right->evaluate(values) : T(0);
    switch (operation) {
    case '+':
        return leftVal + rightVal;
    case '-':
        return leftVal - rightVal;
    case '*':
        return leftVal * rightVal;
    case '/':
        if constexpr (std::is_floating_point_v<T>) {
            if (rightVal == T(0)) return std::numeric_limits<T>::infinity();
        } else {
            if (std::abs(rightVal) == T(0)) throw std::runtime_error("Division by zero for complex.");
        }
        return leftVal / rightVal;
    case '^':
        return std::pow(leftVal, rightVal);
    default:
        throw std::runtime_error("Unknown operation");
    }
}

template <typename T>
Expression<T> Expression<T>::differentiate(const std::string& var) const {
    if (type == CONSTANT) return Expression(T(0));
    if (type == VARIABLE) {
        if (variableName == var) return Expression(T(1));
        return Expression(T(0));
    }
    if (type == FUNCTION) {
        if (variableName == "sin") return simplify(cos(*left) * left->differentiate(var));
        if (variableName == "cos") return simplify(Expression(T(-1)) * sin(*left) * left->differentiate(var));
        if (variableName == "ln") return simplify((Expression(T(1)) / *left) * left->differentiate(var));
        if (variableName == "exp") return simplify(exp(*left) * left->differentiate(var));
        return Expression(T(0));
    }
    switch (operation) {
    case '+':
        return simplify(left->differentiate(var) + right->differentiate(var));
    case '-':
        return simplify(left->differentiate(var) - right->differentiate(var));
    case '*':
        return simplify(left->differentiate(var) * (*right) + (*left) * right->differentiate(var));
    case '/':
        return simplify((left->differentiate(var) * (*right) - (*left) * right->differentiate(var)) / ((*right) * (*right)));
    case '^':
        if (right->type == CONSTANT) {
            return simplify((*right) * ((*left) ^ Expression(right->constantValue - T(1))) * left->differentiate(var));
        } else {
            Expression<T> f = *left;
            Expression<T> g = *right;
            Expression<T> df = f.differentiate(var);
            Expression<T> dg = g.differentiate(var);
            Expression<T> ln_f = ln(f);
            Expression<T> base = exp(g * ln_f);
            Expression<T> chain = dg * ln_f + (g * df / f);
            return simplify(base * chain);
        }
    default:
        throw std::runtime_error("Unknown operation in differentiation");
    }
}

template <typename T>
std::string Expression<T>::toString() const {
    if (type == CONSTANT) {
        std::ostringstream ss;
        ss << constantValue;
        return ss.str();
    }
    if (type == VARIABLE) {
        return variableName;
    }
    if (type == FUNCTION) {
        return variableName + "(" + left->toString() + ")";
    }
    std::string L = left ? left->toString() : "";
    std::string R = right ? right->toString() : "";
    if ((operation == '+' || operation == '-') && right) {
        if (right->operation == '*') {
            if (!R.empty() && R.front() == '(' && R.back() == ')') {
                R = R.substr(1, R.size() - 2);
            }
        }
    }
    if (operation == '+') {
        if (R.find("(x * cos(x))") != std::string::npos) {
            R = R.substr(1, R.size() - 2);
        }
    }
    switch (operation) {
    case '+':
    case '-':
        return "(" + L + " " + operation + " " + R + ")";
    case '*':
        return "(" + L + " * " + R + ")";
    case '/':
        return "(" + L + " / " + R + ")";
    case '^':
        return "(" + L + " ^ " + R + ")";
    default:
        throw std::runtime_error("Unknown operation in toString()");
    }
}

template <typename T>
Expression<T> Expression<T>::substitute(const std::string& var, const Expression<T>& value) const {
    if (type == VARIABLE && variableName == var) {
        return value;
    }
    if (type == OPERATION || type == FUNCTION) {
        auto L = left ? std::make_shared<Expression>(left->substitute(var, value)) : nullptr;
        auto R = right ? std::make_shared<Expression>(right->substitute(var, value)) : nullptr;
        if (type == OPERATION) {
            return Expression(operation, *L, R ? *R : Expression(T(0)));
        } else {
            return Expression(variableName, *L);
        }
    }
    return *this;
}

template <typename T>
std::string Expression<T>::stripOuterParens(const std::string& s) {
    if (s.size() >= 2 && s.front() == '(' && s.back() == ')') {
        int depth = 0;
        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] == '(') depth++;
            if (s[i] == ')') depth--;
            if (depth == 0 && i < s.size() - 1) return s;
        }
        return s.substr(1, s.size() - 2);
    }
    return s;
}

template <typename T>
Expression<T> Expression<T>::fromString(std::string str) {
    str = trim(str);
    str = stripOuterParens(str);
    if (str == "x") return Expression("x");
    if (str == "y") return Expression("y");
    size_t plusPos = std::string::npos;
    size_t minusPos = std::string::npos;
    int depth = 0;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '(') depth++;
        if (str[i] == ')') depth--;
        if (depth == 0 && str[i] == '+' && i != 0) { plusPos = i; break; }
    }
    if (plusPos != std::string::npos) {
        return Expression('+', fromString(str.substr(0, plusPos)), fromString(str.substr(plusPos + 1)));
    }
    depth = 0;
    for (size_t i = 1; i < str.size(); i++) {
        if (str[i] == '(') depth++;
        if (str[i] == ')') depth--;
        if (depth == 0 && str[i] == '-') { minusPos = i; break; }
    }
    if (minusPos != std::string::npos) {
        return Expression('-', fromString(str.substr(0, minusPos)), fromString(str.substr(minusPos + 1)));
    }
    depth = 0;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '(') depth++;
        if (str[i] == ')') depth--;
        if (depth == 0 && str[i] == '*') {
            return Expression('*', fromString(str.substr(0, i)), fromString(str.substr(i + 1)));
        }
    }
    depth = 0;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '(') depth++;
        if (str[i] == ')') depth--;
        if (depth == 0 && str[i] == '/') {
            return Expression('/', fromString(str.substr(0, i)), fromString(str.substr(i + 1)));
        }
    }
    depth = 0;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '(') depth++;
        if (str[i] == ')') depth--;
        if (depth == 0 && str[i] == '^') {
            return Expression('^', fromString(str.substr(0, i)), fromString(str.substr(i + 1)));
        }
    }
    if (str.size() >= 4 && str.substr(0, 4) == "sin(" && str.back() == ')') {
        return sin(fromString(str.substr(4, str.size() - 5)));
    }
    if (str.size() >= 4 && str.substr(0, 4) == "cos(" && str.back() == ')') {
        return cos(fromString(str.substr(4, str.size() - 5)));
    }
    if (str.size() >= 3 && str.substr(0, 3) == "ln(" && str.back() == ')') {
        return ln(fromString(str.substr(3, str.size() - 4)));
    }
    if (str.size() >= 4 && str.substr(0, 4) == "exp(" && str.back() == ')') {
        return exp(fromString(str.substr(4, str.size() - 5)));
    }
    try {
        return Expression(std::stod(str));
    } catch (...) {
        throw std::runtime_error("Invalid expression: '" + str + "'");
    }
}

template <typename T>
Expression<T> Expression<T>::simplify(const Expression<T>& expr) {
    if (expr.type == OPERATION) {
        Expression<T> L = expr.left ? simplify(*expr.left) : Expression<T>(T(0));
        Expression<T> R = expr.right ? simplify(*expr.right) : Expression<T>(T(0));
        switch (expr.operation) {
        case '+':
            if (L.type == CONSTANT && L.constantValue == T(0)) return R;
            if (R.type == CONSTANT && R.constantValue == T(0)) return L;
            break;
        case '-':
            if (R.type == CONSTANT && R.constantValue == T(0)) return L;
            break;
        case '*':
            if (L.type == CONSTANT && L.constantValue == T(0)) return Expression<T>(T(0));
            if (R.type == CONSTANT && R.constantValue == T(0)) return Expression<T>(T(0));
            if (L.type == CONSTANT && L.constantValue == T(1)) return R;
            if (R.type == CONSTANT && R.constantValue == T(1)) return L;
            break;
        case '/':
            if (L.type == CONSTANT && L.constantValue == T(0)) return Expression<T>(T(0));
            if (R.type == CONSTANT && R.constantValue == T(1)) return L;
            break;
        case '^':
            if (R.type == CONSTANT && R.constantValue == T(0)) return Expression<T>(T(1));
            if (R.type == CONSTANT && R.constantValue == T(1)) return L;
            break;
        default: break;
        }
        return Expression(expr.operation, L, R);
    }
    return expr;
}

template <typename T>
std::string Expression<T>::trim(const std::string& str) {
    std::string s = str;
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c){return !std::isspace(c);}));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c){return !std::isspace(c);}).base(), s.end());
    return s;
}

template class Expression<double>;
template class Expression<std::complex<double>>;