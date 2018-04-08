#if 0
#include <iostream>
#include <string>
using namespace std;

/*Q: write a derived class and invoke one of its function polymorphically */
class Base {
public:
	virtual ~Base() { cout << "~Base " << endl; } //destructor is public virtual because Base is expected to be used polymorphically
	virtual void alias() { cout << "Base alias" << endl; }
	void name() { cout << "Base name " << endl;}
	void name(const std::string& s) { cout << "Base name overload " << s << endl;}

	Base& operator=(const Base& b) { cout << "Base assignment op\n"; return *this; }
};
class Derived : public Base {
public:
	virtual ~Derived() { cout << "~Derived " << endl; }
	virtual void alias() { cout << "Derived alias" << endl; }
	void name() { cout << "{ Derived name " << endl;
		Base::name("\t from derived name"); 
		std::cout << "}\n";
	}

	int i = 0;
};

int main(int argc, char** argv) 
{
	Base b;
	Derived d;

	std::cout << "-------1--------\n";
	b.name(); 
	b.alias();

	std::cout << "-------2--------\n";
	//d.name("");<-- does not compile because of name hiding
	d.name();
	d.alias();

	std::cout << "-------3--------\n";
	Base b2 = b; //constructor Base
	Base b3;
	b3 = b;

	std::cout << "-------4--------\n";
	Derived d2;
	d.i = 5;
	d2 = d;

	std::cout << "-------5--------\n";
	Base* pb;
	pb = (Base*) new Derived;
	pb->name();
	pb->name("");
	pb->alias();
	delete pb; //if ~Base is not defined virtual, then ~Derived won't be called.

	std::cout << "-------6--------\n";
	{
		Base& rb = d;
		rb.name();
		rb.alias();
	}


	std::cout << "-------7--------\n";
}

#endif