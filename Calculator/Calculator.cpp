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

	const char e = 'E';							//exponentiation Token
	const std::string ekey = "exp";
}

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
	void set_value(std::string s, double d);	//присваивает объекту типа Variable с именем s значение d
	double get_value(std::string s) const;		//возвращает значение переменной с именем s
	bool is_defined(std::string name) const;	//проверяет наличие переменной c именем name в векторе var_table
	double define_var(Variable var);			//добавляет новую переменную var в вектор var_table
	double delete_var(std::string name);		//Удаляет переменную с именем name из вектора var_table
};

//Symbol_table class definitions
void Symbol_table::set_value(std::string s, double d)
{
	for (Variable& v : var_table)
		if (v.name == s) {
			if (!v.is_const) {
				v.value = d;
				return;
			}
			error("Попытка присвоить значение константной переменной ", v.name);
		}
	error("Неопределённая переменная ", s);
}
double Symbol_table::get_value(std::string s) const
{
	for (Variable v : var_table)
		if (v.name == s) return v.value;
	error("Попытка взятия значения у неопределённой переменной ", s);
}
bool Symbol_table::is_defined(std::string name) const
{
	for (Variable v : var_table)
		if (v.name == name) return true;
	return false;
}
double Symbol_table::define_var(Variable var)
{
	if (is_defined(var.name)) error("Уже определённая переменная ", var.name);
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
	error("Попытка удалить несуществующую переменную ", name);
}



class Token_stream {
public:
	Token get();
	void putback(Token t);
	void ignore(char c);	//отбрасывает символы до символа c включительно
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
	case ',':	//for exponentiation
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
			if (s == Tok_list::ekey)
				return Token{ Tok_list::e };				//exponentiation
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
		error(std::string{ ch }, " - неопределённая лексема");
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {	//проверяем буфер
		full = false;
		return;
	}

	full = false;

	//проверяем входные данные
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

double exponentiation(Token_stream& ts)
	//exp(x,y)
	//exp(2,3) == 2*2*2 == 8
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{
		double x = primary(ts);
		t = ts.get();
		if (t.kind != ',')
			error("Пропущена ',' либо введдён некорректный символ в записи возведения в степень");
		double y = primary(ts);
		if (y != static_cast<int>(y))								//y must be integer
			error("Нельзя возвести число в дробную степень");
		t = ts.get();
		if (t.kind != ')')
			error("Пропущена ')' в записи возведения в степень");
		double d = std::pow(x, y);
		return d;
	}
	default:
		error("Пропущена '(' в записи возведения в степень");
	}
}

double square_root(Token_stream& ts)
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{
		double d = expression(ts);
		t = ts.get();
		if (t.kind != ')')
			error("Пропущена ')' в записи квадратного корня");
		if (d < 0)
			error("Попытка извлечь квадратный корень из отрицательного числа");
		return std::sqrt(d);
	}
	default:
		error("В записи квадратного корня отсутствует '('");
	}
}


double primary(Token_stream& ts)
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{
		double d = expression(ts);
		t = ts.get();
		if (t.kind != ')')
		{
			error("required ')'");
		}
		return d;
	}
	case Tok_list::number:
		return t.value;
	case '-':
		return -primary(ts);
	case Tok_list::name:
		if (var::symbol_table.is_defined(t.name)) {
			double d = var::symbol_table.get_value(t.name);
			return d;
		}
		error("Неизвестная переменная ", t.name);
	case Tok_list::sr:
		return square_root(ts);
	case Tok_list::e:
		return exponentiation(ts);
	default:
		error("Ожидается корректный ввод");
	}
}

double term(Token_stream& ts) {
	double left = primary(ts);
	Token t = ts.get();
	while (true)
	{
		switch (t.kind)
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
			double d = primary(ts);
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
			double d = primary(ts);
			if (d == 0)
				error("division by zero");
			left = fmod(left, d);
			t = ts.get();
			break;
		}
		case Tok_list::number:						//error handling
			error("excess number");
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
		switch (t.kind) {
		case '+':
			left += term(ts);
			t = ts.get();
			break;
		case '-':
			left -= term(ts);
			t = ts.get();
			break;
		case Tok_list::number:								//error handling
			error("Неожидаемый ввод числа");
		case Tok_list::name:									//error handling
			error(t.name, " - неопределённая лексема или переменная");
		default:
			ts.putback(t);
			return left;
		}
	}
}

double declaration(Token_stream& ts, bool is_const = false)
{
	Token t = ts.get();
	if (t.kind != Tok_list::name) error("В объявлении ожидается имя переменной");
	std::string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("Пропущен символ '=' в объявлении ", var_name);

	double var_value = expression(ts);

	Variable var{ var_name, var_value, is_const };

	var::symbol_table.define_var(var);
	return var_value;
}

double constant_declaration(Token_stream& ts)
{
	Token t = ts.get();
	if (t.kind != Tok_list::decl) error("Пропущен спецификатор в объявлении константной переменной");

	bool is_constant = true;
	return declaration(ts, is_constant);
}

double assignment(Token_stream& ts)
{
	Token t = ts.get();
	if (t.kind != Tok_list::name) error("В присваивании ожидается имя переменной");
	std::string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("Пропущен символ '=' в присваивании ", var_name);

	double d = expression(ts);
	var::symbol_table.set_value(var_name, d);
	return d;
}

double deletion(Token_stream& ts)
{
	Token t = ts.get();
	if (t.kind != Tok_list::name) error("В опрерации удаления ожидается имя переменной");

	double d = var::symbol_table.delete_var(t.name);
	return d;
}


double statement(Token_stream& ts)
{
	Token t = ts.get();

	switch (t.kind) {

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
	std::cout << std::endl << "СПРАВКА:" << std::endl
		<< "Вводите инструкции, чтобы программа их выполнила. (для подтеверждения введите символ " << Tok_list::print << ")" << std::endl
		<< "Допустимые операторы: '+', '-', '*', '/', sr( ) - квадратный корень, exp( , ) - возведение в степень." << std::endl << std::endl
		<< "Также возможна работа с переменными:" << std::endl
		<< "---------------------------------------------" << std::endl
		<< "# 'Имя' = 'выражение' <- Объявление (префикс const для константных переменных)" << std::endl
		<< "let 'Имя' = 'выражение' <- Присваивание" << std::endl << std::endl
		<< "Имена переменных должны начинаться с буквы и могут содержать буквы, цифры и знаки подчёркивания" << std::endl
		<< "---------------------------------------------" << std::endl << std::endl
		<< "Чтобы выйти из программы, введите " << Tok_list::quitkey << std::endl << std::endl;
}

void start_message()
{
	std::cout << "Добро пожаловать в программу-калькулятор!" << std::endl
		<< "Введите " << Tok_list::helpkey <<  " для показа справочной информации" << std::endl
		<< "Чтобы выйти, введите " << Tok_list::quitkey << std::endl << std::endl;
}

void calculate(Token_stream& ts)
{
	while (std::cin) {
		try {
			std::cout << Tok_list::prompt;
			Token t = ts.get();
			while (t.kind == Tok_list::print || t.kind == Tok_list::quit || t.kind == Tok_list::help) {
				if (t.kind == Tok_list::quit)
					return;
				if (t.kind == Tok_list::help) {
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
	}
	catch (std::exception& e) {
		std::cerr << "ERROR : " << e.what() << std::endl;
	}
	start_message();
	Token_stream ts;
	calculate(ts);
}