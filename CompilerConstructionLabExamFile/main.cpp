#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
using namespace std;

struct ExpressionNode {
    string content;
    shared_ptr<ExpressionNode> left, right;

    explicit ExpressionNode(string value) : content(value), left(nullptr), right(nullptr) {}
};

class TokenReader {
    string expressionText;
    int position;
    char currentCharacter;

public:
    explicit TokenReader(string expression) : expressionText(expression), position(0) {
        currentCharacter = expressionText[position];
    }

    void moveToNextChar() {
        position++;
        currentCharacter = (position < expressionText.size()) ? expressionText[position] : '\0';
    }

    void skipSpaces() {
        while (currentCharacter != '\0' && isspace(currentCharacter))
            moveToNextChar();
    }

    string readNextToken() {
        skipSpaces();

        if (isdigit(currentCharacter)) {
            string number;
            while (currentCharacter != '\0' && isdigit(currentCharacter)) {
                number += currentCharacter;
                moveToNextChar();
            }
            return number;
        }

        if (currentCharacter == '+' || currentCharacter == '*' ||
            currentCharacter == '(' || currentCharacter == ')' ||
            currentCharacter == '\0') {
            string token(1, currentCharacter);
            moveToNextChar();
            return token;
        }

        return "";
    }
};

class ExpressionParser {
    TokenReader tokenReader;
    string currentToken;

    shared_ptr<ExpressionNode> parseExpression();
    shared_ptr<ExpressionNode> parseExpressionPrime(shared_ptr<ExpressionNode> leftNode);
    shared_ptr<ExpressionNode> parseTerm();
    shared_ptr<ExpressionNode> parseTermPrime(shared_ptr<ExpressionNode> leftNode);
    shared_ptr<ExpressionNode> parseFactor();

public:
    explicit ExpressionParser(string expression) : tokenReader(expression) {
        currentToken = tokenReader.readNextToken();
    }

    void expectToken(string expectedToken) {
        if (currentToken == expectedToken) {
            currentToken = tokenReader.readNextToken();
        } else {
            throw runtime_error("Syntax Error: Expected '" + expectedToken +
                              "' but found '" + currentToken + "'");
        }
    }

    shared_ptr<ExpressionNode> buildParseTree() {
        return parseExpression();
    }
};

shared_ptr<ExpressionNode> ExpressionParser::parseExpression() {
    auto node = make_shared<ExpressionNode>("Expression");
    node->left = parseTerm();
    node->right = parseExpressionPrime(node->left);
    return node;
}

shared_ptr<ExpressionNode> ExpressionParser::parseExpressionPrime(shared_ptr<ExpressionNode> leftNode) {
    auto node = make_shared<ExpressionNode>("Expression'");

    if (currentToken == "+") {
        expectToken("+");
        auto additionNode = make_shared<ExpressionNode>("+");
        additionNode->left = leftNode;
        additionNode->right = parseTerm();
        node->left = additionNode;
        node->right = parseExpressionPrime(additionNode);
    } else {
        node->left = make_shared<ExpressionNode>("epsilon"); // epsilon for empty production
    }
    return node;
}

shared_ptr<ExpressionNode> ExpressionParser::parseTerm() {
    auto node = make_shared<ExpressionNode>("Term");
    node->left = parseFactor();
    node->right = parseTermPrime(node->left);
    return node;
}

shared_ptr<ExpressionNode> ExpressionParser::parseTermPrime(shared_ptr<ExpressionNode> leftNode) {
    auto node = make_shared<ExpressionNode>("Term'");

    if (currentToken == "*") {
        expectToken("*");
        auto multiplicationNode = make_shared<ExpressionNode>("*");
        multiplicationNode->left = leftNode;
        multiplicationNode->right = parseFactor();
        node->left = multiplicationNode;
        node->right = parseTermPrime(multiplicationNode);
    } else {
        node->left = make_shared<ExpressionNode>("epsilon");
    }
    return node;
}

shared_ptr<ExpressionNode> ExpressionParser::parseFactor() {
    auto node = make_shared<ExpressionNode>("Factor");

    if (currentToken == "(") {
        expectToken("(");
        node->left = parseExpression();
        expectToken(")");
    } else if (!currentToken.empty() && isdigit(currentToken[0])) {
        node->left = make_shared<ExpressionNode>(currentToken);
        expectToken(currentToken);
    } else {
        throw runtime_error("Syntax Error: Expected number or '(' but found '" + currentToken + "'");
    }
    return node;
}

void visualizeParseTree(shared_ptr<ExpressionNode> node, int indentationLevel = 0) {
    if (node == nullptr) return;

    string indent(indentationLevel * 3, ' ');
    cout << indent << node->content << endl;

    visualizeParseTree(node->left, indentationLevel + 1);
    visualizeParseTree(node->right, indentationLevel + 1);
}

int main() {
    string mathExpression;
    cout << "Please enter a mathematical expression (e.g., '2+3*4') : ";
    cin >> mathExpression;

    try {
        ExpressionParser parser(mathExpression);
        auto parseTree = parser.buildParseTree();
        cout << "\n\nParse Tree for "<<mathExpression<<" :\n" << endl;
        visualizeParseTree(parseTree);
    } catch (const exception& e) {
        cerr << "Error while parsing: " << e.what() << endl;
    }

    return 0;
}
