/******************************************************************

Introduction:
mathparser is a simple c++ program to parse math expressions.

The program is a modified version of math expression parser
presented in the book : "C++ The Complete Reference" by H.Schildt.

It supports operators: + - * / ^ ( )

It supports math functions : SIN, COS, TAN, COT, ASIN, ACOS, ATAN, ACOT, SINH,
COSH, TANH, COTH, ASINH, ACOSH, ATANH, ACOTH, PDF, ERF, ERFC, GAMMA, LN, LOG,
EXP, SQRT, CBRT, ROUND, FLOOR, CEIL.

mathparser version 1.0 by Hamid Soltani. (gmail: hsoltanim)
Last modified: Aug. 2016.
mathparser version 1.1.1 by Faether.
Last modified: June 2023.

*******************************************************************/


#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <math.h>
#include <unordered_map>

#define PI 3.14159265358979323846
#define SQRT2PI 2.50662827463100050242

using namespace std;

unordered_map<string, int> table = {
	{"SIN", 2},
	{"COS", 3},
	{"TAN", 4},
	{"COT", 5},
	{"ASIN", 6},
	{"ACOS", 7},
	{"ATAN", 8},
	{"ACOT", 9},
	{"SINH", 10},
	{"COSH", 11},
	{"TANH", 12},
	{"COTH", 13},
	{"ASINH", 14},
	{"ACOSH", 15},
	{"ATANH", 16},
	{"ACOTH", 17},
	{"PDF", 18},
	{"ERF", 19},
	{"ERFC", 20},
	{"GAMMA", 21},
	{"LN", 22},
	{"LG", 23},
	{"EXP", 24},
	{"SQRT", 25},
	{"CBRT", 26},
	{"ROUND", 27},
	{"FLOOR", 28},
	{"CEIL", 29},
};

// Node of a parse tree
struct TreeNode{
	int data;
	TreeNode* left = NULL;
	TreeNode* right = NULL;

	TreeNode(int new_data){
		data = new_data;
	}

	TreeNode(int new_data, TreeNode* new_right){
		data = new_data;
		right = new_right;
	}

	TreeNode(int new_data, TreeNode* new_right, TreeNode* new_left){
		data = new_data;
		right = new_right;
		left = new_left;
	}

	~TreeNode(){
		delete left;
		delete right;
	}
};

// Tree Node that stores constant values
struct DataNode : TreeNode{
	double number;

	DataNode(int data, double new_number)
	: TreeNode(data){
		number = new_number;
	}
};

// Stack node
struct StackNode{
	TreeNode* node;
	StackNode* next = NULL;

	StackNode(int data, StackNode* previous){
		next = previous;
		node = new TreeNode(data);
	}

	StackNode(int data, double number, StackNode* previous){
		next = previous;
		node = new DataNode(data, number);
	}

	StackNode(int data, StackNode* right, StackNode* previous){
		next = previous;
		node = new TreeNode(data, right -> node);
	}

	StackNode(int data, StackNode* right, StackNode* left, StackNode* previous){
		next = previous;
		node = new TreeNode(data, right -> node, left -> node);
	}

	~StackNode(){
		delete node;
		delete next;
	}
};

// Stack for storing and organizing values
class Stack{
	StackNode* top = NULL;

	public:
		void push(int data);
		void push(double data);
		StackNode* pop();
		TreeNode* getNode();
		void Clear();
};

void Stack::Clear(){
    delete top;
	top = NULL;
}

TreeNode* Stack::getNode(){
	return top -> node;
}

// Organizes values into a parse tree
void Stack::push(int data){
	if(data < 128){
		if(data < 0){
			top = new StackNode(data, pop(), pop(), top);
		}else{
			top = new StackNode(data, pop(), top);
		}
	}else{
		top = new StackNode(data, top);
	}
}

void Stack::push(double data){
	top = new StackNode(129, data, top);
}

StackNode* Stack::pop(){
	StackNode* temp = top;
	top = top -> next;
	return temp;
}

enum types { DELIMITER = 1, VARIABLE, NUMBER, FUNCTION };
class parser {
	char *exp_ptr; // points to the expression
	char token[256]; // holds current token
	char tok_type; // holds token's type
	double x; // holds variable value
	double rev(TreeNode* node);
	void parse2();
	void parse3();
	void parse4();
	void parse5();
	void parse6();
	void get_token();

	public:
		Stack stack;
		parser();
		TreeNode* parse(char*);
		double evaluate(double);
		double evaluate(char*);
};

// Parser constructor.
parser::parser()
{
	exp_ptr = NULL;
	x = 0.0;
}

// Parser entry point.
TreeNode* parser::parse(char *exp)
{
	stack.Clear();
	exp_ptr = exp;
	get_token();
	if (!*token)
	{
		return 0; // no expression present
	}
	parse2();
	return stack.getNode();
}

// Add or subtract two terms.
void parser::parse2()
{
	char op;
	parse3();
	while ((op = *token) == '+' || op == '-')
	{
		get_token();
		parse3();
		if(op == '+') stack.push(-1);
		else stack.push(-2);
	}
}

// Multiply or divide two factors.
void parser::parse3()
{
	char op;
	parse4();
	while ((op = *token) == '*' || op == '/')
	{
		get_token();
		parse4();
		if(op == '*') stack.push(-3);
		else stack.push(-4);
	}
}

// Process an exponent.
void parser::parse4()
{
	parse5();
	while (*token == '^')
	{
		get_token();
		parse5();
		stack.push(-5);
	}
}

// Evaluate a unary + or -.
void parser::parse5()
{
	char op = 0;
	if ((tok_type == DELIMITER) && (*token == '+' || *token == '-'))
	{
		op = *token;
		get_token();
	}
	parse6();
	if (op == '-'){
		stack.push(1);
	}
}

// Process a function, a parenthesized expression, a value or a variable
void parser::parse6()
{
	bool isfunc = (tok_type == FUNCTION);
	string temp_token;
	if (isfunc)
	{
		temp_token = string(token);
		get_token();
	}
	if (*token == '(')
	{
		get_token();
		parse2();
		if (isfunc)
		{
			stack.push(table.find(temp_token) -> second);
		}
	}
	else if(tok_type == VARIABLE){
		stack.push(128);
	}else{
		stack.push(atof(token));
	}
	get_token();
}

// Evaluate parsed expression stored in stack
double parser::evaluate(double value){
	x = value;
	return rev(stack.getNode());
}

// Parse and evaluate given expression
double parser::evaluate(char* exp){
	x = 0.0;
	return rev(parse(exp));
}

// Recursive evaluation loop
double parser::rev(TreeNode* node){
	if(node == NULL) return 0;
	else if(node -> data == 129) return ((DataNode*)node) -> number;
	else if(node -> data == 128) return x;
	else if(node -> data == -1) return rev(node -> left) + rev(node -> right);
	else if(node -> data == -2) return rev(node -> left) - rev(node -> right);
	else if(node -> data == -3) return rev(node -> left) * rev(node -> right);
	else if(node -> data == -4) return rev(node -> left) / rev(node -> right);
	else if(node -> data == -5) return pow(rev(node -> left), rev(node -> right));
	else if(node -> data == 1) return -rev(node -> right);
	else if(node -> data == 2) return sin(rev(node -> right));
	else if(node -> data == 3) return cos(rev(node -> right));
	else if(node -> data == 4) return tan(rev(node -> right));
	else if(node -> data == 5) return -tan(rev(node -> right) + PI/2);
	else if(node -> data == 6) return asin(rev(node -> right));
	else if(node -> data == 7) return acos(rev(node -> right));
	else if(node -> data == 8) return atan(rev(node -> right));
	else if(node -> data == 9) return -atan(rev(node -> right)) + PI/2;
	else if(node -> data == 10) return sinh(rev(node -> right));
	else if(node -> data == 11) return cosh(rev(node -> right));
	else if(node -> data == 12) return tanh(rev(node -> right));
	else if(node -> data == 13) return 1/tanh(rev(node -> right));
	else if(node -> data == 14) return asinh(rev(node -> right));
	else if(node -> data == 15) return acosh(rev(node -> right));
	else if(node -> data == 16) return atanh(rev(node -> right));
	else if(node -> data == 17) return atanh(1/rev(node -> right));
	else if(node -> data == 18) return exp((-pow(rev(node -> right), 2))/2)/SQRT2PI;
	else if(node -> data == 19) return erf(rev(node -> right));
	else if(node -> data == 20) return erfc(rev(node -> right));
	else if(node -> data == 21) return tgamma(rev(node -> right));
	else if(node -> data == 22) return log(rev(node -> right));
	else if(node -> data == 23) return log10(rev(node -> right));
	else if(node -> data == 24) return exp(rev(node -> right));
	else if(node -> data == 25) return sqrt(rev(node -> right));
	else if(node -> data == 26) return cbrt(rev(node -> right));
	else if(node -> data == 27) return round(rev(node -> right));
	else if(node -> data == 28) return floor(rev(node -> right));
	else if(node -> data == 29) return ceil(rev(node -> right));
	return 0;
}

// Obtain the next token.
void parser::get_token()
{
	char *temp;
	tok_type = 0;
	temp = token;
	*temp = '\0';
	if (!*exp_ptr)  // at end of expression
		return;
	while (isspace(*exp_ptr))  // skip over white space
		++exp_ptr; 
	if (strchr("+-*/%^=()", *exp_ptr)) 
	{
		tok_type = DELIMITER;
		*temp++ = *exp_ptr++;  // advance to next char
	}
	else if (isalpha(*exp_ptr))
	{
		while (!strchr(" +-/*%^=()\t\r", *exp_ptr) && (*exp_ptr))
			*temp++ = toupper(*exp_ptr++);
		while (isspace(*exp_ptr))  // skip over white space
			++exp_ptr;
		tok_type = (*exp_ptr == '(') ? FUNCTION : VARIABLE;
	}
	else if (isdigit(*exp_ptr) || *exp_ptr == '.')
	{
		while (!strchr(" +-/*%^=()\t\r", *exp_ptr) && (*exp_ptr))
			*temp++ = toupper(*exp_ptr++);
		tok_type = NUMBER;
	}
	*temp = '\0';
}
