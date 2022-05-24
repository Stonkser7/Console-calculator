/*
This program performs calculator operations

Grammar:	

Calculate:
	Statement
	Input
	Quit
	Calculate Statement
	Help

Statement:
	Declaration
	Constant_Declaration
	Assignment
	Deletion
	Expression

Declaration:
	'#' Name '=' Expression

Constant_Declaration:
	"const" Declaration

Assignment:
	"let" Name '=' Expression

Deletion:
	"delete" Name

Help:
	"help"

Input:
	';'
	
Quit:
	"exit"
	
Expression:
	Term
	Expression + Term
	Expression - Term
	
Term:
	Primary
	Term * Primary
	Term / Primary
	Term % Primary

Primary:
	Number
	'(' Expression ')'
	-Primary
	Name
	Square root
	Exponentiation

Square root:
	"sr" '(' Expression ')'

Exponentiation:
	exp '(' Primary ',' Primary ')'

Number:
	Floating-point literal

Name:
	Sequence of characters that begins with a letter and includes only letters, numbers and '_'
*/
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <Windows.h>
#include <iomanip>
#include <ctime>
#include <conio.h>
#include <cctype>


void error(std::string s1, std::string s2 = "")
{
	throw std::runtime_error(s1 + s2);
}

const char help = 'H';
const std::string helpkey = "help";

const char print = ';';

const char quit = 'x';
const std::string quitkey = "exit";

const char number = '8';
const std::string prompt = "> ";
const std::string result = "= ";

const char name = 'N';						//Variable name Token

const char decl = '#';						//declaration Token

const char constprefix = 'C';
const std::string constprefixkey = "const";	//constant variables declaration

const char assign = 'A';					//assignment Token
const std::string assignkey = "let";

const char del = 'D';						//delete Token
const std::string delkey = "delete";

const char sr = 'S';						//square root Token
const std::string srkey = "sr";

const char e = 'E';							//exponentiation Token
const std::string ekey = "exp";

class Token
{
public:
	char kind;
	double value;
	std::string name;

	Token() {};

	Token(char ch) :kind{ ch } {};

	Token(char ch, double d) :kind{ ch }, value{ d } {};

	Token(char ch, std::string s) :kind{ ch }, name{ s } {};
};

class Variable {
public:
	std::string name;
	double value;
	bool is_const;

	Variable(std::string s, double d, bool b) :name{ s }, value{ d }, is_const{ b } {};
};

class Symbol_table {
private:
	std::vector <Variable> var_table;
public:
	void set_value(std::string s, double d);	//����������� ������� ���� Variable � ������ s �������� d
	double get_value(std::string s);			//���������� �������� ���������� � ������ s
	bool is_defined(std::string name);			//��������� ������� ���������� c ������ name � ������� var_table
	double define_var(Variable var);			//��������� ����� ���������� var � ������ var_table
	double delete_var(std::string name);		//������� ���������� � ������ name �� ������� var_table
};

void Symbol_table::set_value(std::string s, double d)
{
	for (Variable& v : var_table)
		if (v.name == s) {
			if (!v.is_const) {
				v.value = d;
				return;
			}
			error("������� ��������� �������� ����������� ���������� ", v.name);
		}
	error("������������� ���������� ", s);
}
double Symbol_table::get_value(std::string s)
{
	for (Variable v : var_table)
		if (v.name == s) return v.value;
	error("������� ������ �������� � ������������� ���������� ", s);
}
bool Symbol_table::is_defined(std::string name)
{
	for (Variable v : var_table)
		if (v.name == name) return true;
	return false;
}
double Symbol_table::define_var(Variable var)
{
	if (is_defined(var.name)) error("��� ����������� ���������� ", var.name);
	var_table.push_back(var);
	return var.value;
}
double Symbol_table::delete_var(std::string name)
{
	for (int i = 0; i < var_table.size(); ++i) {
		if (var_table[i].name == name) {
			double d = var_table[i].value;
			var_table.erase(var_table.begin() + i);
			return d;
		}
	}
	error("������� ������� �������������� ���������� ", name);
}

class Token_stream {
public:
	Token get();
	void putback(Token t);
	void ignore(char c);	//����������� ������� �� ������� c ������������
private:
	bool full{ false };
	Token buffer;
};

void Token_stream::putback(Token t)
{
	if (full)
		error("Token_stream::putback(): buffer is full");
	buffer = t;
	full = true;
}

Token Token_stream::get()
{
	if (full)
	{
		full = false;
		return buffer;
	}
	char ch;
	std::cin >> ch;
	switch (ch) {
	case print:	//print confirmation
	case decl:	//declaration Token
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case '=':
	case ',':	//for exponentiation
		return Token{ ch };
	case '.':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		std::cin.putback(ch);
		double val;
		std::cin >> val;
		return Token{ number, val };
	default:									//string Tokens
		if (std::isalpha(ch)) {
			std::cin.putback(ch);
			std::string s;
			while (std::cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch;
			std::cin.putback(ch);
			if (s == assignkey)
				return Token{ assign };			//assign operator
			if (s == srkey)
				return Token{ sr };				//square root
			if (s == ekey)
				return Token{ e };				//exponentiation
			if (s == quitkey)
				return Token{ quit };			//exit
			if (s == constprefixkey)
				return Token{ constprefix };	//constant variables declaration
			if (s == helpkey)
				return Token{ help };			//help information output
			if (s == delkey)
				return Token{ del };			//deletion variable
			return Token{ name, s };
		}
		error(std::string{ ch }, " - ������������� �������");
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {	//��������� �����
		full = false;
		return;
	}

	full = false;

	//��������� ������� ������
	char ch = 0;
	while (std::cin >> ch)
		if (ch == c) return;
}



Token_stream ts;

Symbol_table symbol_table;



void clean_up_mess()
{
	ts.ignore(print);
}

double primary();
double expression();

double exponentiation()
	//exp(x,y)
	//exp(2,3) == 2*2*2 == 8
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{
		double x = primary();
		t = ts.get();
		if (t.kind != ',')
			error("��������� ',' ���� ������ ������������ ������ � ������ ���������� � �������");
		double y = primary();
		if (y != static_cast<int>(y))								//y must be integer
			error("������ �������� ����� � ������� �������");
		t = ts.get();
		if (t.kind != ')')
			error("��������� ')' � ������ ���������� � �������");
		double d = std::pow(x, y);
		return d;
	}
	default:
		error("��������� '(' � ������ ���������� � �������");
	}
}

double square_root()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{
		double d = expression();
		t = ts.get();
		if (t.kind != ')')
			error("��������� ')' � ������ ����������� �����");
		if (d < 0)
			error("������� ������� ���������� ������ �� �������������� �����");
		return std::sqrt(d);
	}
	default:
		error("� ������ ����������� ����� ����������� '('");
	}
}


double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{
		double d = expression();
		t = ts.get();
		if (t.kind != ')')
		{
			error("required ')'");
		}
		return d;
	}
	case number:
		return t.value;
	case '-':
		return -primary();
	case name:
		if (symbol_table.is_defined(t.name)) {
			double d = symbol_table.get_value(t.name);
			return d;
		}
		error("����������� ���������� ", t.name);
	case sr:
		return square_root();
	case e:
		return exponentiation();
	default:
		error("��������� ���������� ����");
	}
}

double term() {
	double left = primary();
	Token t = ts.get();
	while (true)
	{
		switch (t.kind)
		{
		case '(':			//for (1+3)(2+4) or 5(2+3) notation (the same as Term * Primary)
			ts.putback(t);
			left *= primary();
			t = ts.get();
			break;
		case '*':
			left *= primary();
			t = ts.get();
			break;
		case '/':
		{
			double d = primary();
			if (d == 0)
			{
				error("division by zero");
			}
			left /= d;
			t = ts.get();
			break;
		}
		case '%':
		{
			double d = primary();
			if (d == 0)
				error("division by zero");
			left = fmod(left, d);
			t = ts.get();
			break;
		}
		case number:						//error handling
			error("excess number");
		default:
			ts.putback(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	Token t = ts.get();
	while (true)
	{
		switch (t.kind) {
		case '+':
			left += term();
			t = ts.get();
			break;
		case '-':
			left -= term();
			t = ts.get();
			break;
		case number:								//error handling
			error("����������� ���� �����");
		case name:									//error handling
			error(t.name, " - ������������� ������� ��� ����������");
		default:
			ts.putback(t);
			return left;
		}
	}
}

double declaration(bool is_const = false)
{
	Token t = ts.get();
	if (t.kind != name) error("� ���������� ��������� ��� ����������");
	std::string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("�������� ������ '=' � ���������� ", var_name);

	double var_value = expression();

	Variable var{ var_name, var_value, is_const };

	symbol_table.define_var(var);
	return var_value;
}

double constant_declaration()
{
	Token t = ts.get();
	if (t.kind != decl) error("�������� ������������ � ���������� ����������� ����������");

	bool is_constant = true;
	return declaration(is_constant);
}

double assignment()
{
	Token t = ts.get();
	if (t.kind != name) error("� ������������ ��������� ��� ����������");
	std::string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("�������� ������ '=' � ������������ ", var_name);

	double d = expression();
	symbol_table.set_value(var_name, d);
	return d;
}

double deletion()
{
	Token t = ts.get();
	if (t.kind != name) error("� ��������� �������� ��������� ��� ����������");

	double d = symbol_table.delete_var(t.name);
	return d;
}


double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case decl:
		return declaration();
	case constprefix:
		return constant_declaration();
	case assign:
		return assignment();
	case del:
		return deletion();
	default:
		ts.putback(t);
		return expression();
	}
}

void help_message()
{
	std::cout << std::endl << "�������:" << std::endl
		<< "������� ����������, ����� ��������� �� ���������. (��� �������������� ������� ������ " << print << ")" << std::endl
		<< "���������� ���������: '+', '-', '*', '/', sr( ) - ���������� ������, exp( , ) - ���������� � �������." << std::endl << std::endl
		<< "����� �������� ������ � �����������:" << std::endl
		<< "---------------------------------------------" << std::endl
		<< "# '���' = '���������' <- ���������� (������� const ��� ����������� ����������)" << std::endl
		<< "let '���' = '���������' <- ������������" << std::endl << std::endl
		<< "����� ���������� ������ ���������� � ����� � ����� ��������� �����, ����� � ����� �������������" << std::endl
		<< "---------------------------------------------" << std::endl << std::endl
		<< "����� ����� �� ���������, ������� " << quitkey << std::endl << std::endl;
}

void start_message()
{
	std::cout << "����� ���������� � ���������-�����������!" << std::endl
		<< "������� " << helpkey <<  " ��� ������ ���������� ����������" << std::endl
		<< "����� �����, ������� " << quitkey << std::endl << std::endl;
}

void calculate()
{
	while (std::cin) {
		try {
			std::cout << prompt;
			Token t = ts.get();
			while (t.kind == print || t.kind == quit || t.kind == help) {
				if (t.kind == quit)
					return;
				if (t.kind == help) {
					help_message();
				}
				t = ts.get();
			}
			ts.putback(t);
			std::cout << result << statement() << std::endl;
		}
		catch (std::exception& e) {
			std::cerr << "ERROR : " << e.what() << std::endl;
			clean_up_mess();
		}
	}
}

int main()
{
	srand(time(0));
	rand();
	SetConsoleOutputCP(1251);
	try {
		bool is_const = true;
		symbol_table.define_var(Variable{ "pi", 3.1415, is_const });
	}
	catch (std::exception& e) {
		std::cerr << "ERROR : " << e.what() << std::endl;
	}
	start_message();
	calculate();
}