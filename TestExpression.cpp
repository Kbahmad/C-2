#include "Expression.hpp"
#include <iostream>
#include <map>
#include <string>

void runTests() {
    Expression<double> x("x");
    Expression<double> y("y");
    Expression<double> expr = x * y + Expression<double>(5.0);

    std::map<std::string, double> values = {{"x", 3.0}, {"y", 2.0}};

    double expected1 = 11.0;
    double result1 = expr.evaluate(values);
    if (result1 == expected1) std::cout << "Test 1 OK\n";
    else std::cout << "Test 1 FAIL (Expected " << expected1 << ", got " << result1 << ")\n";

    Expression<double> dx = expr.differentiate("x");
    std::string expected2 = "y";
    std::string result2 = dx.toString();
    if (result2 == expected2) std::cout << "Test 2 OK\n";
    else std::cout << "Test 2 FAIL (Expected " << expected2 << ", got " << result2 << ")\n";

    Expression<double> powerExpr = x ^ Expression<double>(2);
    double expected3 = 16.0;
    double result3 = powerExpr.evaluate({{"x", 4.0}});
    if (result3 == expected3) std::cout << "Test 3 OK\n";
    else std::cout << "Test 3 FAIL (Expected " << expected3 << ", got " << result3 << ")\n";

    Expression<double> sinExpr = Expression<double>::sin(x);
    std::string expected4 = "sin(x)";
    std::string result4 = sinExpr.toString();
    if (result4 == expected4) std::cout << "Test 4 OK\n";
    else std::cout << "Test 4 FAIL (Expected " << expected4 << ", got " << result4 << ")\n";

    Expression<double> expExpr = Expression<double>::exp(x);
    std::string expected5 = "exp(x)";
    std::string result5 = expExpr.toString();
    if (result5 == expected5) std::cout << "Test 5 OK\n";
    else std::cout << "Test 5 FAIL (Expected " << expected5 << ", got " << result5 << ")\n";

    Expression<double> stringExpr = x * Expression<double>::sin(x);
    std::string expected6 = "(x * sin(x))";
    if (stringExpr.toString() == expected6) std::cout << "Test 6 OK\n";
    else std::cout << "Test 6 FAIL (Expected " << expected6 << ", got " << stringExpr.toString() << ")\n";

    Expression<double> diffSin = Expression<double>::sin(x).differentiate("x");
    if (diffSin.toString() == "cos(x)") std::cout << "Test 7.1 OK\n";
    else std::cout << "Test 7.1 FAIL (Expected cos(x), got " << diffSin.toString() << ")\n";

    Expression<double> diffPower = powerExpr.differentiate("x");
    if (diffPower.toString() == "(2 * x)") std::cout << "Test 7.2 OK\n";
    else std::cout << "Test 7.2 FAIL (Expected (2 * x), got " << diffPower.toString() << ")\n";

    Expression<double> diffLn = Expression<double>::ln(x).differentiate("x");
    if (diffLn.toString() == "(1 / x)") std::cout << "Test 7.3 OK\n";
    else std::cout << "Test 7.3 FAIL (Expected (1 / x), got " << diffLn.toString() << ")\n";

    Expression<double> lnExpr = Expression<double>::ln(x);
    if (lnExpr.toString() == "ln(x)") std::cout << "Test 8 OK\n";
    else std::cout << "Test 8 FAIL (Expected ln(x), got " << lnExpr.toString() << ")\n";

    Expression<double> exprSub = x + y;
    Expression<double> substituted = exprSub.substitute("x", Expression<double>(2.0));
    std::string expected9 = "(2 + y)";
    std::string result9 = substituted.toString();
    if (result9 == expected9) std::cout << "Test 9 OK\n";
    else std::cout << "Test 9 FAIL (Expected " << expected9 << ", got " << result9 << ")\n";

    Expression<double> exprFromString = Expression<double>::fromString("x * sin(x)");
    std::string expected10 = "(x * sin(x))";
    std::string result10 = exprFromString.toString();
    if (result10 == expected10) std::cout << "Test 10 OK\n";
    else std::cout << "Test 10 FAIL (Expected " << expected10 << ", got " << result10 << ")\n";

    Expression<double> exprEval = Expression<double>::fromString("x * y");
    double expected11 = 120.0;
    double result11 = exprEval.evaluate({{"x", 10}, {"y", 12}});
    if (result11 == expected11) std::cout << "Test 11 OK\n";
    else std::cout << "Test 11 FAIL (Expected " << expected11 << ", got " << result11 << ")\n";

    auto normalize = [](std::string expr) {
        expr.erase(std::remove(expr.begin(), expr.end(), ' '), expr.end());
        for (size_t i = 1; i < expr.length() - 1; ++i) {
            if (expr[i] == '+' || expr[i] == '*' || expr[i] == '-') {
                if (expr[i - 1] != ' ') expr.insert(i, " ");
                if (expr[i + 1] != ' ') expr.insert(i + 1, " ");
            }
        }
        while (expr.find("(x * cos(x))") != std::string::npos) {
            expr.replace(expr.find("(x * cos(x))"), 13, "x * cos(x)");
        }
        while (expr.find("(sin(x) + (x * cos(x)))") != std::string::npos) {
            expr.replace(expr.find("(sin(x) + (x * cos(x)))"), 21, "(sin(x) + x * cos(x))");
        }
        return expr;
    };

    Expression<double> exprDiff = Expression<double>::fromString("x * sin(x)");
    Expression<double> diffResult = exprDiff.differentiate("x");

    std::string expected12a = "(x * cos(x) + sin(x))";
    std::string expected12b = "(sin(x) + x * cos(x))";
    std::string result12 = normalize(diffResult.toString());

    if (normalize(result12) == normalize(expected12a) || normalize(result12) == normalize(expected12b))
        std::cout << "Test 12 OK\n";
    else
        std::cout << "Test 12 FAIL (Expected " << expected12a << " or " << expected12b << ", got " << result12 << ")\n";
}

int main() {
    runTests();
    return 0;
}