/*
��������� ��������� ������� ������������

���������� ���������:	

����������:
	����������
	�����
	�����
	���������� ����������

����������:
	����������
	������������
	���������

����������:
	'#' ��� '=' ���������

������������:
	"assign" ��� '=' ���������

�����:
	;
	
�����:
	x
	
���������:
	����
	��������� + ����
	��������� - ����
	
����:
	���������
	���� * ���������
	���� / ���������
	���� % ���������

���������:
	�����
	'(' ��������� ')'
	-���������
	���
	���������� ������
	���������� � �������

���������� ������:
	"sr" '(' ��������� ')'

���������� � �������:
	exp '(' ��������� ',' ��������� ')'

�����:
	������� � ��������� ������

���:
	������������������ ��������, ������� ���������� � ����� � �������� ������ ����� � �����
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



const char print = ';';
const char quit = 'x';
const char number = '8';
const std::string prompt = "> ";
const std::string result = "= ";

const char name = 'n';	//Variable name Token

const char decl = '#';					//declaration Token

const char assign = 'A';				//assignment Token
const std::string assignkey = "assign";

const char sr = 'S';					//square root Token
const std::string srkey = "sr";

const char p = 'P';						//exponentiation Token
const std::string pkey = "exp";

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
};

std::vector <Variable> var_table;

void set_value(std::string s, double d)
	//����������� ������� ���� Variable � ������ s �������� d
{
	for (Variable &v : var_table)
		if (v.name == s)
		{
			v.value = d;
			return;
		}
	error("set_value: ������������� ���������� ", s);
}
double get_value(std::string s)
	//���������� �������� ���������� � ������ s
{
	for (Variable v : var_table)
		if (v.name == s) return v.value;
	error("get_value: ������������� ����������", s);
}
bool is_defined(std::string var)
	//��������� ������� ���������� var � ������� var_table
{
	for (Variable v : var_table)
		if (v.name == var) return true;
	return false;
}
double define_name(std::string var, double val)
	//��������� ���� (var, val) � ������ var_table
{
	if (is_defined(var)) error("��� ����������� ���������� ", var);
	var_table.push_back(Variable{ var, val });
	return val;
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
	case quit:	//exit
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
	default:							//string Tokens
		if (std::isalpha(ch)) {
			std::cin.putback(ch);
			std::string s;
			while (std::cin.get(ch) && (isalpha(ch) || isdigit(ch))) s += ch;
			std::cin.putback(ch);
			if (s == assignkey)
				return Token{ assign };
			if (s == srkey)
				return Token{ sr };
			if (s == pkey)
				return Token{ p };
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
		if (is_defined(t.name)) {
			double d = get_value(t.name);
			return d;
		}
		error("����������� ���������� ", t.name);
	case sr:
		return square_root();
	case p:
		return exponentiation();
	default:
		error("�������������� ����");
	}
}

double term() {
	double left = primary();
	Token t = ts.get();
	while (true)
	{
		switch (t.kind)
		{
		case '(':			//for (1+3)(2+4) or 5(2+3) notation
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
		case number:
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
		case number:
			error("����������� ���� �����");
		case name:
			error(t.name, " - ������������� ������� ��� ����������");
		default:
			ts.putback(t);
			return left;
		}
	}
}

double declaration()
{
	Token t = ts.get();
	if (t.kind != name) error("� ���������� ��������� ��� ����������");
	std::string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("�������� ������ '=' � ���������� ", var_name);

	double d = expression();
	define_name(var_name, d);
	return d;
}


double assignment()
{
	Token t = ts.get();
	if (t.kind != name) error("� ������������ ��������� ��� ����������");
	std::string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("�������� ������ '=' � ������������ ", var_name);

	double d = expression();
	set_value(var_name, d);
	return d;
}

double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case decl:
		return declaration();
	case assign:
		return assignment();
	default:
		ts.putback(t);
		return expression();
	}
}

void start_message()
{
	std::cout << "����� ���������� � ���������-�����������!" << std::endl << std::endl
		<< "������� ��������� � ������� � ��������� ������. (��� �������������� ������� ������ " << print << ")" << std::endl
		<< "���������� ���������: '+', '-', '*', '/', sr(9) - ���������� ������, exp(2, 3) - ���������� � �������." << std::endl << std::endl
		<< "����� �������� ������ � �����������:" << std::endl
		<< "���������� -> # '���' = '����� ��� ���������' || ������������ -> assign '���' = '����� ��� ���������'" << std::endl << std::endl
		<< "����� �����, ������� " << quit << std::endl << std::endl;
}

void calculate()
{
	while (std::cin) {
		try {
			std::cout << prompt;
			Token t = ts.get();
			while (t.kind == print)
				t = ts.get();
			if (t.kind == quit)
				break;
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
		define_name("pi", 3.1415);
	}
	catch (std::exception& e) {
		std::cerr << "ERROR : " << e.what() << std::endl;
	}
	start_message();
	calculate();
}