/*
This program performs simple calculator operations

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
	Power

Square root:
	"sr" '(' Expression ')'

Power:
	"pow" '(' Primary ',' Primary ')'

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


void error(std::string msg)
{
	throw std::runtime_error(msg);
}

namespace Tok_list {
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

	const char p = 'P';							//power Token
	const std::string powkey = "pow";
}

class Token
{
public:
	char m_kind;
	double m_value;
	std::string m_name;

	Token() {};

	Token(char ch) :m_kind{ ch } {};

	Token(char ch, double d) :m_kind{ ch }, m_value{ d } {};

	Token(char ch, std::string s) :m_kind{ ch }, m_name{ s } {};
};

class Variable {
public:
	std::string m_name;
	double m_value;
	bool m_is_const;

	Variable(std::string name, double val, bool is_const) :m_name{ name }, m_value{ val }, m_is_const{ is_const } {};
};

class Symbol_table {
private:
	std::vector <Variable> m_var_table;
public:
	void set_value(std::string name, double d);	//����������� ������� ���� Variable � ������ name �������� d
	double get_value(std::string name) const;		//���������� �������� ���������� � ������ name
	bool is_defined(std::string name) const;	//��������� ������� ���������� c ������ name � ������� var_table
	double define_var(Variable var);			//��������� ����� ���������� var � ������ var_table
	double delete_var(std::string name);		//������� ���������� � ������ name �� ������� var_table
};

//Symbol_table class definitions
void Symbol_table::set_value(std::string name, double d)
{
	for (Variable& v : m_var_table)
		if (v.m_name == name) {
			if (!v.m_is_const) {
				v.m_value = d;
				return;
			}
			error("������� �������� �������� ����������� ���������� " + v.m_name);
		}
	error("������������� ���������� " + name);
}
double Symbol_table::get_value(std::string name) const
{
	for (Variable v : m_var_table)
		if (v.m_name == name) return v.m_value;
	error("������� ������ �������� � ������������� ���������� " + name);
}
bool Symbol_table::is_defined(std::string name) const
{
	for (Variable v : m_var_table)
		if (v.m_name == name) return true;
	return false;
}
double Symbol_table::define_var(Variable var)
{
	if (is_defined(var.m_name)) error("��� ����������� ���������� " + var.m_name);
	m_var_table.push_back(var);
	return var.m_value;
}
double Symbol_table::delete_var(std::string name)
{
	for (int i = 0; i < m_var_table.size(); ++i) {
		if (m_var_table[i].m_name == name) {
			double d = m_var_table[i].m_value;
			m_var_table.erase(m_var_table.begin() + i);
			return d;
		}
	}
	error("������� ������� �������������� ���������� " + name);
}



class Token_stream {
public:
	Token get();
	void putback(Token t);
	void ignore(char c);	//����������� ������� �� ������� c ������������
private:
	bool m_full{ false };
	Token m_buffer;
};

void Token_stream::putback(Token t)
{
	if (m_full)
		error("Token_stream::putback(): buffer is full");
	m_buffer = t;
	m_full = true;
}

Token Token_stream::get()
{
	if (m_full)
	{
		m_full = false;
		return m_buffer;
	}
	char ch;
	std::cin >> ch;
	switch (ch) {
	case Tok_list::print:	//print confirmation
	case Tok_list::decl:	//declaration Token
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case '=':
	case ',':				//',' for functions
		return Token{ ch };
	case '.':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		std::cin.putback(ch);
		double val;
		std::cin >> val;
		return Token{ Tok_list::number, val };
	default:									//string Tokens
		if (std::isalpha(ch)) {
			std::cin.putback(ch);
			std::string s;
			while (std::cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch;
			std::cin.putback(ch);
			if (s == Tok_list::assignkey)
				return Token{ Tok_list::assign };			//assign operator
			if (s == Tok_list::srkey)
				return Token{ Tok_list::sr };				//square root
			if (s == Tok_list::powkey)
				return Token{ Tok_list::p };				//power
			if (s == Tok_list::quitkey)
				return Token{ Tok_list::quit };			//exit
			if (s == Tok_list::constprefixkey)
				return Token{ Tok_list::constprefix };	//constant variables declaration
			if (s == Tok_list::helpkey)
				return Token{ Tok_list::help };			//help information output
			if (s == Tok_list::delkey)
				return Token{ Tok_list::del };			//deletion variable

			return Token{ Tok_list::name, s };
		}
		error(std::string{ ch } + " - ������������� �������");
	}
}

void Token_stream::ignore(char c)
{
	if (m_full && c == m_buffer.m_kind) {	//��������� �����
		m_full = false;
		return;
	}

	m_full = false;

	//��������� ������� ������
	char ch = 0;
	while (std::cin >> ch)
		if (ch == c) return;
}



namespace var {
	Symbol_table symbol_table;
}



void clean_up_mess(Token_stream& ts)
{
	ts.ignore(Tok_list::print);
}

double primary(Token_stream&);
double expression(Token_stream&);

double power(Token_stream& ts)
	//exp(x,y)
	//exp(2,3) == 2*2*2 == 8
{
	Token t = ts.get();
	switch (t.m_kind) {
	case '(':
	{
		double x = expression(ts);
		t = ts.get();
		if (t.m_kind != ',')
			error("��������� ',' ���� ������ ������������ ������ � ������ ���������� � �������");
		double y = expression(ts);
		if (y != static_cast<int>(y))								//y must be integer
			error("���������� ������� ������ ���� ����� ������");
		t = ts.get();
		if (t.m_kind != ')')
			error("��������� ')' � ������ ���������� � �������");
		double d = std::pow(x, y);
		return d;
	}
	default:
		error("��������� '(' � ������ ���������� � �������");
	}
}

double square_root(Token_stream& ts)
{
	Token t = ts.get();
	switch (t.m_kind) {
	case '(':
	{
		double d = expression(ts);
		t = ts.get();
		if (t.m_kind != ')')
			error("��������� ')' � ������ ����������� �����");
		if (d < 0)
			error("������ ������� ���������� ������ �� �������������� �����");
		return std::sqrt(d);
	}
	default:
		error("� ������ ����������� ����� ����������� '('");
	}
}


double primary(Token_stream& ts)
{
	Token t = ts.get();
	switch (t.m_kind) {
	case '(':
	{
		double d = expression(ts);
		t = ts.get();
		if (t.m_kind != ')')
		{
			error("��������� ')'");
		}
		return d;
	}
	case Tok_list::number:
		return t.m_value;
	case '-':
		return -primary(ts);
	case Tok_list::name:
		if (var::symbol_table.is_defined(t.m_name)) {
			double d = var::symbol_table.get_value(t.m_name);
			return d;
		}
		error(t.m_name + " - ������������� ������� ��� ����������");
	case Tok_list::sr:
		return square_root(ts);
	case Tok_list::p:
		return power(ts);
	default:
		error("��������� ���������� ����");
	}
}

double term(Token_stream& ts) {
	double left = primary(ts);
	Token t = ts.get();
	while (true)
	{
		switch (t.m_kind)
		{
		case '(':			//for (1+3)(2+4) or 5(2+3) notation (the same as Term * Primary)
			ts.putback(t);
			left *= primary(ts);
			t = ts.get();
			break;
		case '*':
			left *= primary(ts);
			t = ts.get();
			break;
		case '/':
		{
			double right = primary(ts);
			if (right == 0)
			{
				error("������� �� 0");
			}
			left /= right;
			t = ts.get();
			break;
		}
		case '%':
		{
			double right = primary(ts);
			if (right == 0)
				error("������� �� ������� �� 0");
			left = fmod(left, right);
			t = ts.get();
			break;
		}
		case Tok_list::number:						//error handling
			error("����������� ���� �����");
		default:
			ts.putback(t);
			return left;
		}
	}
}

double expression(Token_stream& ts)
{
	double left = term(ts);
	Token t = ts.get();
	while (true)
	{
		switch (t.m_kind) {
		case '+':
			left += term(ts);
			t = ts.get();
			break;
		case '-':
			left -= term(ts);
			t = ts.get();
			break;
		case Tok_list::number:									//error handling
			error("����������� ���� �����");
		case Tok_list::name:									//error handling
			error(t.m_name + " - ������������� ������� ��� ����������");
		default:
			ts.putback(t);
			return left;
		}
	}
}

double declaration(Token_stream& ts, bool is_const = false)
{
	Token t = ts.get();
	if (t.m_kind != Tok_list::name) error("� ���������� ��������� ��� ����������");
	std::string var_name = t.m_name;

	Token t2 = ts.get();
	if (t2.m_kind != '=') error("�������� ������ '=' � ���������� " + var_name);

	double var_value = expression(ts);

	Variable var{ var_name, var_value, is_const };

	var::symbol_table.define_var(var);
	return var_value;
}

double constant_declaration(Token_stream& ts)
{
	Token t = ts.get();
	if (t.m_kind != Tok_list::decl) error("�������� ������������ � ���������� ����������� ����������");

	bool is_constant = true;
	return declaration(ts, is_constant);
}

double assignment(Token_stream& ts)
{
	Token t = ts.get();
	if (t.m_kind != Tok_list::name) error("� ������������ ��������� ��� ����������");
	std::string var_name = t.m_name;

	Token t2 = ts.get();
	if (t2.m_kind != '=') error("�������� ������ '=' � ������������ " + var_name);

	double d = expression(ts);
	var::symbol_table.set_value(var_name, d);
	return d;
}

double deletion(Token_stream& ts)
{
	Token t = ts.get();
	if (t.m_kind != Tok_list::name) error("� ��������� �������� ��������� ��� ����������");

	double d = var::symbol_table.delete_var(t.m_name);
	return d;
}


double statement(Token_stream& ts)
{
	Token t = ts.get();

	switch (t.m_kind) {

	case Tok_list::decl:
		return declaration(ts);

	case Tok_list::constprefix:
		return constant_declaration(ts);

	case Tok_list::assign:
		return assignment(ts);

	case Tok_list::del:
		return deletion(ts);

	default:
		ts.putback(t);
		return expression(ts);
	}
}

void help_message()
{
	std::cout << std::endl << "�������:" << std::endl
		<< "������� ����������, ����� ��������� �� ���������. (��� �������������� ������� ������ " << Tok_list::print << ")" << std::endl
		<< "���������� ���������: '+', '-', '*', '/', sr( ) - ���������� ������, pow( , ) - ���������� � �������." << std::endl << std::endl
		<< "����� �������� ������ � �����������:" << std::endl
		<< "---------------------------------------------" << std::endl
		<< "# '���' = '���������' <- ���������� (������� const ��� ����������� ����������)" << std::endl
		<< "let '���' = '���������' <- ������������" << std::endl
		<< "delete '���' <- �������� ����������" << std::endl << std::endl
		<< "����� ���������� ������ ���������� � ����� � ����� ��������� �����, ����� � ����� �������������" << std::endl
		<< "---------------------------------------------" << std::endl << std::endl
		<< "����� ����� �� ���������, ������� " << Tok_list::quitkey << std::endl << std::endl;
}

void start_message()
{
	std::cout << "����� ���������� � ���������-�����������!" << std::endl
		<< "������� " << Tok_list::helpkey <<  " ��� ������ ���������� ����������" << std::endl
		<< "����� �����, ������� " << Tok_list::quitkey << std::endl << std::endl;
}

void calculate(Token_stream& ts)
{
	while (std::cin) {
		try {
			std::cout << Tok_list::prompt;
			Token t = ts.get();
			while (t.m_kind == Tok_list::print || t.m_kind == Tok_list::quit || t.m_kind == Tok_list::help) {
				if (t.m_kind == Tok_list::quit)
					return;
				if (t.m_kind == Tok_list::help) {
					help_message();
				}
				t = ts.get();
			}
			ts.putback(t);
			std::cout << Tok_list::result << statement(ts) << std::endl;
		}
		catch (std::exception& e) {
			std::cerr << "ERROR : " << e.what() << std::endl;
			clean_up_mess(ts);
		}
	}

	if (std::cin.bad()) error("Fatal std::cin stream error");
}

int main()
{
	srand(time(0));
	rand();
	SetConsoleOutputCP(1251);
	try {
		bool is_const = true;
		var::symbol_table.define_var(Variable{ "pi", 3.1415, is_const });
		var::symbol_table.define_var(Variable{ "e", 2.71828, is_const });
	}
	catch (std::exception& e) {
		std::cerr << "ERROR : " << e.what() << std::endl;
	}
	start_message();
	Token_stream ts;
	calculate(ts);
}