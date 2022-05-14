#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <Windows.h>
#include <iomanip>
#include <ctime>
#include <conio.h>


void error(std::string s)
{
	throw std::runtime_error(s);
}


class Token
{
public:
	char kind;
	double value;
};

class Token_stream {
public:
	Token get();
	void putback(Token t);
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
	case ';':	//input confirmation
	case 'q':	//exit
	case '(': case ')':
	case '+': case '-': case '*': case '/':
		return Token{ ch };
	case '.':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		std::cin.putback(ch);
		double val;
		std::cin >> val;
		return Token{ '8', val };
	default:
		error("invalid token");
	}
}

Token_stream ts;

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind)
	{
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
	case '8':
		return t.value;
	default:
		error("required a number");
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
		case '8':
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
		default:
			ts.putback(t);
			return left;
		}
	}
}

int main()
{
	srand(time(0));
	rand();
	SetConsoleOutputCP(1251);
	try {
		std::cout << "Калькулятор." << std::endl
			<< "Чтобы выйти, введите 'q'" << std::endl
			<< "Вводите выражения(для подтверждения вводите ';'): " << std::endl;
		double val = 0;
		while (std::cin) {
			Token t = ts.get();
			if (t.kind == 'q')
				break;
			if (t.kind == ';')
			{
				std::cout << "=" << val << std::endl;
				val = 0;
			}
			else
				ts.putback(t);
			val += expression();
		}
	}
	catch (std::exception& e) {
		std::cerr << "ERROR : " << e.what() << std::endl;
	}
}