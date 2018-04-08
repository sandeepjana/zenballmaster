#if 0
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cassert>
using namespace std;

#if 0
//addition copy-ctor like member functions 
/* consider a class fixed_vector which has a templated member function overload: */

/*
Note: A copy ctor and copy assignment operator specifically constructs/assignes from other object of 
EXACTLY same type -- including the same template parameters, if the class is templated.
Quote from standard: A templated constructor is never a copy constructor ... similarly for copy assign
*/

template<typename T, size_t size>
class fixed_vector 
{
public:
	typedef T* iterator; 
	typedef const T* const_iterator;
	fixed_vector() {}
	//...
	iterator begin() { return v_; }
	iterator end() { return v_ + size; }
	const_iterator begin() const { return v_; }
	const_iterator end() const { return v_ + size; }
	//copy from other vector which may have a different size
	//note that this is not copy-ctor. implicitly generated copy-ctor is still available
	//no strong-exception-safety guarantee
	template<typename O, size_t osize>
	fixed_vector(const fixed_vector<O, osize>& other)
	{
		copy(other.begin(), other.begin() + min(size, osize), begin());
	}

	//another alternative would be to work on iterators:
	//this helps to copy a portion of the vector
	template<typename RaIter>
	fixed_vector(RaIter first, RaIter last)
	{
		copy(first, first + min(size, (size_t) last - first), begin());
	}

private:
	T v_[size];
};
#endif

//goal is to implement a complex number class - std::complex is much better option, but here is for practice.
class complex_t
{
public:
	//explicit keyword avoids implicitly converting a double to a complex_t (because of the presence of optional second arg
	explicit complex_t( double real, double imaginary = 0.0) : m_real(real), m_imaginary(imaginary)
	{
	}
	//prefer writing a op= b instead of a = a op b (where op can be any operator such as +).
	/* += is more effcient than + itself because the former does not create temporaries
		check operator+ implementation and compare
	*/
	complex_t& operator+=(const complex_t& other)
	{
		m_real += other.m_real;
		m_imaginary += other.m_imaginary;
		return *this;
	}

	//pre-increment
	complex_t& operator++()
	{
		++m_real;
		return *this;
	}
	/* TODO
	//post-increment
	complex_t& operator(complex_t& c)
	{

	}
	*/

	//helper function for operator<<
	ostream& print(ostream& os)
	{
		return os << "(" << m_real << "," << m_imaginary << ")" ;
	}

private:
	double m_real;
	double m_imaginary;
};

//operator+ is not made a member function because it can be implemented using the public interface of complex_t.
const complex_t operator+(const complex_t& a, const complex_t& b)
{
	complex_t tmp(a); //implicitly generated copy-ctor is fine
	tmp += b; //operator+= 
	return tmp;
}

//stream operator<< cannot be a member function because the operator acts actually on the stream:
//the implicit "this" parameter is on the left side (which is std::cout) of the operator <<.
//Returns a reference to the ostream so that chaining can be done
ostream& operator<<(ostream& os, complex_t& c)
{
	return c.print(os);
}


#if 0
namespace A
{
	class X {};
	void f(X);
	void f(X)
	{
		std::cout << "A::f" << std::endl;
	}
}
namespace B
{
	void f(A::X);
	void g(A::X param)
	{
		f(param); 
		/* error C2668: 'B::f' : ambiguous call to overloaded function.
		could be 'void B::f(A::X)'
		or       'void A::f(A::X)' [found using argument-dependent lookup]
		*/
	}

	void f(A::X)
	{
		std::cout << "B::f" << std::endl;
	}
}
#endif

int main()
{
	//iterators
#if 0
	{
		vector<int> v(10);
		generate(v.begin(), v.end(), [](){ return rand()%99; });

		auto it1 = v.end() - 1;
		auto it2 = --v.end(); //herb says this can be illegal?!
		cout << "it1 -> " << *it1 << ", it2 -> "  << *it2 << endl;
	}
#endif
#if 0
	//case insensitive strings -- character traits
	{
		//TODO: read more about char_traits
		struct caseinsensitive_ctraits_t : public char_traits<char>
		{
			//TODO: why static?
			static bool eq(char c1, char c2)
			{
				return toupper(c1) == toupper(c2);
			}
			static bool lt(char c1, char c2)
			{
				return toupper(c1) < toupper(c2);
			}
			static bool compare(const char* s1, const char* s2, size_t n)
			{
				return _memicmp(s1, s2, n); //TODO see if this is available
			}
			//TODO: what is the signature of find()?
			static const char* find(const char* s, int n, char a)
			{
				while(n-- > 0 && toupper(*s) != toupper(a))
					++s;
				return n >= 0 ? s : NULL;
			}
		};

		//allocator is ignored
		typedef basic_string<char, caseinsensitive_ctraits_t> ci_string;
		
		ci_string s1("AbcdE");
		ci_string s2("ABcde");

		assert(s1 == s2); //case-insensitve comparison
		//yet, case preserving string
		cout << s1.c_str() << " " << s2.c_str() << endl;
		//however the following does not compile:
		//cout << s1 << endl; //because cout is basic_ostream templated on char_traits<char>
		//TODO: make above line work: implement operator<<() and operator>>() and other (+= etc) for ci_string
	}
#endif
#if 0	
	{
		fixed_vector<char, 10> v;
		/* compiler warning: 'std::_Copy_impl': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators' */
		fixed_vector<int, 5> w(v);
	}
#endif

#if 0
	//namespace test
	{
		A::X x;
		B::g(x);
	}
#endif

	//name hiding
	{

		struct Base
		{
			void f(int) { std::cout << "Base.f\n"; }
			void f(std::string) { std::cout << "Base.f\n"; }
		};
		struct Derived : public Base 
		{
			//using Base::f; //<--  'main::Derived::f' : function declaration conflicts with 'main::Base::f' introduced by using-declaration
			void f(int) { std::cout << "Derived.f\n"; }
		};

		Derived D;
		D.f(5); //calls Derived::f
		//D.f("hello"); //<-- compilation fails here because f(string) is hidden by D.f(int)
		D.Base::f("hello"); //<-- compiles well

		Derived * pD = new Derived;
		((Base *) pD)->f(5); //calls Base::f
		delete pD;
	}

	return 0;
}
#endif