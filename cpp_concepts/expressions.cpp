#if 0
#include <iostream>
#include <string>
#include <functional>
#include <algorithm>
#include <vector>
using namespace std;

struct EXP
{
	void left() { cout << "left"; }
	void right() { cout << "right"; }

	int a;
	int b;
};

typedef std::function<void (const std::string&)> fxn_string_t;

class CSayHello
{
public:
	CSayHello(fxn_string_t before, fxn_string_t after)
	{
		m_before = before;
		m_after  = after;
	}
	void say_hello(const string& username)
	{
		m_before("Welcome ");
		cout << username << " ";
		m_after("to the lambdas!");
	}
private:
	fxn_string_t m_before;
	fxn_string_t m_after;
};

struct SPrinter
{
	bool caseIntact;
	void print_fxn(const string& s)
	{
		if(caseIntact)
		{
			cout << s;
		}
		else
		{
			string us(s);
			transform(s.begin(), s.end(), us.begin(), toupper);
			cout << us;
		}
	}
	void print_fxn_v2(const string& s, bool quote)
	{
		if(quote)
			cout << "\"" << s << "\"";
		else
			cout << s;
	}
};

void error_check() {}

int main()
{
	vector<int> v(10);
	generate(v.begin(), v.end(), [](){ 
		return rand()%99; });
}

#if 0
int main(int argc, char** argv)
{
	EXP e = {10, 20};

	//pointer to members are useful when you have multiple objects
	// and you want to access one of their member functions/variables
	
	int EXP::* pma = &EXP::a; //declares and defines!
	void (EXP::* pmr)() = &EXP::right;
	auto pml = &EXP::left;

	//pmd is a pointer to a member of EXP that is type int.
	//how to remember?
	//For normal pointers, it is:
	//	int * pa = &a;
	//So replace * and & with EXP::* and &EXP::

	cout << e.*pma << endl;
	(e.*pml)(); //<- unfortunately, parenths are required around e.*pml
	cout << endl;
	(e.*pmr)();
	cout << endl;

	SPrinter a, b;
	a.caseIntact = false;
	b.caseIntact = true;
	//CSayHello(&(a.print_fxn), &(a.print_fxn)); Wont work :P
	CSayHello guest([&a](const string& s){ 
		a.print_fxn(s); },
			 [&b](const string& s){ 
		b.print_fxn(s); });
		
	guest.say_hello("zen ball master");
	cout << endl;
	//changed my mind
	b.caseIntact = false;
	guest.say_hello("zen ball master");
	cout << endl;

	//Note: The lambda type should match the fxn_string_t
	//but no need for the internal actual functions calls to match it.
	//Lamda expression just acts as a wrapper around functions.
	CSayHello guestV2([&a](const string& s){ 
		a.print_fxn_v2(s, true); },
			 [&a](const string& s){ 
		a.print_fxn_v2(s, true); });

	guestV2.say_hello("zen ball master");

	vector<int> v(10);
	generate(v.begin(), v.end(), [](){ 
		error_check();
		return rand()%99; });


	return 0;
}
#endif
#endif