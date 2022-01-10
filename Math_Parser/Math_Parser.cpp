#include <iostream>
#include <iomanip>
#include <math.h>
#include <stack>
#include <string>
#include <vector>

using namespace std;

class Calculator {
public:
    //We rewrite the numbers and operators to the vector, taking care of the correctness of the entered data.
    vector<string> parse(const string& input)
    {
        vector<string> vec;
        string current;
        int parenthesisOpen = 0, parenthesisClose = 0;
        bool firstIsNegative = false, lastWasDot = false, lastWasOperator = false, lastWasParenthesis = false;

        if (input[0] == '-')
            firstIsNegative = true;

        for (char c : input)
        {
            string s(1, c);

            if (isLetter(s)) {
                throw string("The phrase cannot contain letters!");
                break;
            }
            else if (!isdigit(c) && !isAcceptableChar(s)) {
                throw string("Operator not supported!");
                break;
            }

            else if (isdigit(c) || (c == '.') || lastWasDot || firstIsNegative) {
                current += c;
                if (firstIsNegative)
                    firstIsNegative = false;

                if (c == '.')
                {
                    lastWasDot = true;
                }
                else {
                    lastWasDot = false;
                }
                lastWasOperator = false;
            }

            else if (c)
            {
                if ((lastWasOperator && c == '-') && !lastWasParenthesis)
                    current += c;
                else {
                    if (lastWasOperator && c == '(' && c == ')')
                        throw string("Side by side operators are prohibited!");
                    if (!current.empty())
                    {
                        vec.emplace_back(move(current));
                        current = "";
                    }
                    if (c != ' ')
                        vec.emplace_back(1, c);
                    lastWasOperator = true;
                    if (c == '(')
                        parenthesisOpen++;
                    if (c == ')') {
                        parenthesisClose++;
                        lastWasParenthesis = true;
                    }
                    else
                        lastWasParenthesis = false;
                }
            }
        }
        if (parenthesisOpen != parenthesisClose)
            throw string("Parenthesis mismatch!");
        if (!current.empty())
            vec.push_back(move(current));
        return vec;
    }

    //We check which function to perform depending on the received data.
    double perform(vector<string> notation)
    {
        if (notation.size() < 2)
            throw string("Insufficient data!");

        else if (!isNumber(notation[0]) && notation[0] != "(")
            throw string("The first argument must be a number!");

        else if (notation[1] == "!") {
            if (notation.size() == 2) {
                if (isNatural(notation[0]))
                    return calculateOneArg(notation);
                else
                    throw string("Strong is only computed for natural numbers!");
            }
            else
                throw string("Invalid number or value of arguments for this action!");
        }

        else if (notation[1] == "^" || notation[1] == "V" || notation[1] == "%") {
            if (notation.size() == 3 && isNumber(notation[2]))
                if (notation[1] == "V" && stod(notation[2]) <= 0)
                    throw string("The number under the root must be a positive number!");
                else
                    return calculateTwoArg(notation);
            else
                throw string("Invalid number or value of arguments for this action!");
        }
        else if (notation.size() > 2) {
            vector<string> output = convertToRPN(notation);
            return evalRPN(output);
        }
        else
            throw string("Insufficient data!");
    }

    void help()
    {
        cout <<
            "++=============================================================================++\n"
            "|| The calculator solves complex arithmetic operations, which include:         ||\n"
            "|| addition(+), subtraction(-), multiplication(*), division(/)                 ||\n"
            "|| You can also use parentheses. For example: 2 * (10/2) - 2                   ||\n"
            "||                                                                             ||\n"
            "|| It also calculates other mathematical operations such as:                   ||\n"
            "|| power (^), root (V), factorial (!) and percent (%)                          ||\n"
            "|| However, in this case we must follow a few guidelines:                      ||\n"
            "|| * Math operations cannot be combined!                                       ||\n"
            "|| * Exponentiation: The foundation of power ^ Power exponent. E.g: 3 ^ 2      ||\n"
            "|| * Roots: The degree of the root V Root number. E.g: 2 V 9                   ||\n"
            "|| * Factorial: Number!. E.g: 5!                                               ||\n"
            "|| * Percentage: The percentage % Of a number. E.g. 10 % 100                   ||\n"
            "++=============================================================================++" << endl << endl;
    }

private:
    //A function to handle the operation with one number and one operator.
    unsigned long long calculateOneArg(vector<string> notation)
    {
        string o = notation[1];
        int x = stoi(notation[0]);
        char oper = o[0];

        switch (oper)
        {
        case '!':
            return strong(x);
        default:
            return 0;
        }
    }
    //A function to handle an operation with two numbers and one operator.
    double calculateTwoArg(vector<string> notation)
    {
        string o = notation[1];
        double x = stod(notation[0]);
        char oper = o[0];
        double y = stod(notation[2]);

        switch (oper)
        {
        case '^':
            return pow(x, y);
        case 'V':
            return pow(y, 1 / x);
        case '%':
            return (x / 100 * y);
        default:
            return 0.0;
        }
    }

    // A function that solves an expression in reverse Polish notation.
    double evalRPN(vector<string>& tokens) {
        if (tokens.empty()) {
            return 0;
        }
        stack<string> s;
        for (const auto& tok : tokens) {
            if (!isOperator(tok)) {
                s.emplace(tok);
            }
            else {
                double y = stod(s.top());
                s.pop();
                double x = stod(s.top());
                s.pop();
                if (tok[0] == '+') {
                    x += y;
                }
                else if (tok[0] == '-') {
                    x -= y;
                }
                else if (tok[0] == '*') {
                    x *= y;
                }
                else if (tok[0] == '/') {
                    if (y == 0)
                        throw string("Division by 0 is prohibited!");
                    else x /= y;
                }
                else throw string("For this expression only the following operators are supported: '+', '-', '*', '/'");
                s.emplace(to_string(x));
            }
        }
        return stod(s.top());
    }

    // Converting an infix expression into postfix.
    vector<string> convertToRPN(vector<string>& expression) {
        vector<string> output;
        infixToPostfix(expression, output);
        return output;
    }

    void infixToPostfix(const vector<string>& infix, vector<string>& postfix) {
        stack<string> s;
        for (string tok : infix) {
            if (atoi(tok.c_str())) {
                postfix.emplace_back(tok);
            }
            else if (tok == "(") {
                s.emplace(tok);
            }
            else if (tok == ")") {
                while (!s.empty()) {
                    tok = s.top();
                    s.pop();
                    if (tok == "(") {
                        break;
                    }
                    postfix.emplace_back(tok);
                }
            }
            else {
                while (!s.empty() && precedence(tok) <= precedence(s.top())) {
                    postfix.emplace_back(s.top());
                    s.pop();
                }
                s.emplace(tok);
            }
        }
        while (!s.empty()) {
            postfix.emplace_back(s.top());
            s.pop();
        }
    }

    //Sequence of actions.
    int precedence(string x) {
        if (x == "(") {
            return 0;
        }
        else if (x == "+" || x == "-") {
            return 1;
        }
        else if (x == "*" || x == "/") {
            return 2;
        }
        return 3;
    }

    //Recursive factorial function.
    unsigned long long strong(unsigned long long n)
    {
        if (n < 2)
            return 1;

        return n * strong(n - 1);
    }

    bool isNumber(const string& number) {
        bool condition = false;
        for (const auto& tok : number) {
            if (isdigit(tok) || tok == '.' || tok == '-') {
                condition = true;
            }
            else {
                condition = false;
                break;
            }
        }
        return condition;
    }

    bool isNatural(const string& number) {
        double x = stod(number);
        if ((x - int(x) == 0) && (x >= 0)) return true;
        else return false;
    }

    bool isLetter(const string& letter) {
        return letter.length() == 1 && string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUWXYZ").find(letter) != string::npos;
    }

    bool isOperator(const string& op) {
        return op.length() == 1 && string("+-*/^V!%").find(op) != string::npos;
    }

    bool isAcceptableChar(const string& op) {
        return op.length() == 1 && string("+-*/^V!%(). ").find(op) != string::npos;
    }
};

int main() {
    string input;
    double result = 0.0;
    Calculator c;
    cout <<
        "++=============================================================================++\n"
        "||                       CALCULATOR CONSOLE APPLICATION                        ||\n"
        "||                                                                             ||" << endl;
    c.help();

    while (true) {
        cout << "Please enter the operation to perform." << endl;
        getline(cin, input);

        if (input == "help")
            c.help();
        if (input == "exit")
            break;
        else if (input != "") {
            try {
                vector<string> notation = c.parse(input);
                result = c.perform(notation);
                cout << "Result: " << fixed << result << endl;
                cout << "++=============================================================================++" << endl << endl;
            }
            catch (string& exc) {
                cout << exc << endl;
                cout << "++=============================================================================++" << endl << endl;
            }
        }
        else {
            cout << "If you want to quit, type 'exit'.If you need help, type: 'help'" << endl;
            cout << "++=============================================================================++" << endl << endl;
        }
    }
}