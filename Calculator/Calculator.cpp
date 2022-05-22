/*
Программа выполняет функции калькулятора

Грамматика программы:	

Вычисление:
	Инструкция
	Вывод
	Выход
	Вычисление Инструкция

Инструкция:
	Объявление
	Присваивание
	Выражение

Объявление:
	'#' Имя '=' Выражение

Присваивание:
	"assign" Имя '=' Выражение

Вывод:
	;
	
Выход:
	x
	
Выражение:
	Терм
	Выражение + Терм
	Выражение - Терм
	
Терм:
	Первичное
	Терм * Первичное
	Терм / Первичное
	Терм % Первичное

Первичное:
	Число
	'(' Выражение ')'
	-Первичное
	Имя
	Квадратный корень
	Возведение в степень

Квадратный корень:
	"sr" '(' Выражение ')'

Возведение в степень:
	exp '(' Первичное ',' Первичное ')'

Число:
	Литерал с плавающей точкой

Имя:
	последовательность символов, которая начинается с буквы и включает только буквы и цифры
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
	//присваивает объекту типа Variable с именем s значение d
{
	for (Variable &v : var_table)
		if (v.name == s)
		{
			v.value = d;
			return;
		}
	error("set_value: неопределённая переменная ", s);
}
double get_value(std::string s)
	//возвращает значение переменной с именем s
{
	for (Variable v : var_table)
		if (v.name == s) return v.value;
	error("get_value: неопределённая переменная", s);
}
bool is_defined(std::string var)
	//проверяет наличие переменной var в векторе var_table
{
	for (Variable v : var_table)
		if (v.name == var) return true;
	return false;
}
double define_name(std::string var, double val)
	//добавляет пару (var, val) в вектор var_table
{
	if (is_defined(var)) error("Уже определённая переменная ", var);
	var_table.push_back(Variable{ var, val });
	return val;
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
			error("Пропущена ',' либо введдён некорректный символ в записи возведения в степень");
		double y = primary();
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

double square_root()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{
		double d = expression();
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
		error("Неизвестная переменная ", t.name);
	case sr:
		return square_root();
	case p:
		return exponentiation();
	default:
		error("Нераспознанный ввод");
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
			error("Неожидаемый ввод числа");
		case name:
			error(t.name, " - неопределённая лексема или переменная");
		default:
			ts.putback(t);
			return left;
		}
	}
}

double declaration()
{
	Token t = ts.get();
	if (t.kind != name) error("В объявлении ожидается имя переменной");
	std::string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("Пропущен символ '=' в объявлении ", var_name);

	double d = expression();
	define_name(var_name, d);
	return d;
}


double assignment()
{
	Token t = ts.get();
	if (t.kind != name) error("В присваивании ожидается имя переменной");
	std::string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("Пропущен символ '=' в присваивании ", var_name);

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
	std::cout << "Добро пожаловать в программу-калькулятор!" << std::endl << std::endl
		<< "Вводите выражения с числами с плавающей точкой. (для подтеверждения введите символ " << print << ")" << std::endl
		<< "Допустимые операторы: '+', '-', '*', '/', sr(9) - квадратный корень, exp(2, 3) - возведение в степень." << std::endl << std::endl
		<< "Также возможна работа с переменными:" << std::endl
		<< "Объявление -> # 'Имя' = 'число или выражение' || Присваивание -> assign 'Имя' = 'число или выражение'" << std::endl << std::endl
		<< "Чтобы выйти, введите " << quit << std::endl << std::endl;
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