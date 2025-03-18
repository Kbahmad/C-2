#include "Expression.hpp"
#include <iostream>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: differentiator --eval \"expression\" var=value ...\n";
        std::cerr << "       differentiator --diff \"expression\" --by variable\n";
        return 1;
    }

    std::string command = argv[1];

    if (command == "--eval") {
        std::string exprStr = argv[2];
        Expression<double> expr = Expression<double>::fromString(exprStr);

        std::map<std::string, double> values;
        for (int i = 3; i < argc; i++) {
            std::string arg = argv[i];
            size_t eqPos = arg.find("=");
            if (eqPos != std::string::npos) {
                std::string var = arg.substr(0, eqPos);
                double val = std::stod(arg.substr(eqPos + 1));
                values[var] = val;
            }
        }

        std::cout << expr.evaluate(values) << "\n";
    }
    else if (command == "--diff") {
        std::string exprStr = argv[2];
        std::string var;
        if (argc >= 5 && std::string(argv[3]) == "--by") {
            var = argv[4];
        } else {
            std::cerr << "Error: Missing variable for differentiation.\n";
            return 1;
        }

        Expression<double> expr = Expression<double>::fromString(exprStr);
        Expression<double> diffExpr = expr.differentiate(var);
        std::cout << diffExpr.toString() << "\n";
    }
    else {
        std::cerr << "Unknown command: " << command << "\n";
        return 1;
    }

    return 0;
}