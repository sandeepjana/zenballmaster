#if 0
#include <iostream>
#include <string>
using namespace std;

#ifndef noexcept 
#define noexcept 
#endif

string modifiable_rvalue()
{
	return "hello";
}
const string const_rvalue()
{
	return "world";
}
string modifiable_lvalue("hello");
const string const_lvalue("world");

//remote integer
class rint 
{
	int* ri;
public:
	rint() { ri = nullptr; cout << "default ctor" << endl;}
	~rint() { cout << "dtor " << ri << endl; delete ri;}
	rint(const int i) {
		//how can I tell that this object is already 
		ri = new int(i); 
		cout << "unary ctor " << ri << endl; 
	}
	rint(const rint& other) {
		//self assignment can never happen 
		ri = (nullptr != other.ri) ? new int(*other.ri) : nullptr;
		cout << "copy ctor " << ri << endl;
	}
	
	//move constructor never seems to be invoked here because
	//its too simple a class
	rint(rint&& other) noexcept {
		cout << "move ctor" << endl;
		//ri is uninitialized here
		ri = other.ri;
		other.ri = nullptr;		
		//The following is the wrong way of doing it
		//*this = other; <-- invokes copy assignment operator
	}

	//member by member swap - used in Way 3 and 5
	friend void swap(rint& lhs, rint& rhs) {
		using std::swap;
		swap(lhs.ri, rhs.ri);
		//if more members are there, they are swapped here...
	}

#if 0		
	//Way 1: uses copy-assignment operator of *ri (content of ri is copy-assigned with ri itself unmodified)
	rint& operator=(const rint& other) {
		//ri is either nullptr or newed
		cout << "copy assigment (1)" << endl;
		if(nullptr == ri) {
			ri = new int(*other.ri);
		}
		else {
			*ri = *other.ri; //the content itself is copied
			//This will not cause leak even if the *ri is of some other
			//user defined type, because that type's copy constructor takes
			//care of deallocating any internal resources
		}
		
		return *this;
	}

	//Way 2: uses copy-ctor of *ri, (problem: code duplication?) 
	rint& operator=(const rint& other) {
		cout << "copy assigment (2)" << endl;
		if(this == &other)
			return *this;
			
		int* riOrig = ri; //for exception safety
		ri = new int(*other.ri);
		cout << "newed " << ri << endl;
		delete riOrig;
		cout << "deleted " << riOrig << endl;
		return *this;
	}

	//Way 3: copy-and-swap assignment operator (ri itself is swapped)
	rint& operator=(rint other) {
		cout << "copy assigment (3)" << endl;
		//though above swap member function can be 
		//avoided by replicating its implementation here, the swap
		//member function can be used in move assignment as well. 
		swap(*this, other); //ADL picks the correct swap
		//'other' carries the old this->ri to its destruction
		return *this;
	}

	//Way 4: move the content from a copy
	rint& operator=(rint other) {
		cout << "copy assignment using move (4) " << other.ri << endl;
		delete ri; //this step can be skipped if ri were a unique_ptr
		cout << "freed " << ri << endl;
		//below is just equivalent to ri = other.ri for int* type
		ri = move(other.ri); //How does move work if ri is not POD? ri's move assignment operator would be invoked
		other.ri = nullptr; //this step can be skipped if ri were a unique_ptr
		return *this;
	}
#endif
#if 0
	//Way 5: const qualified lvalue with copy-constructor inside the function
	rint& operator=(const rint& other) {
		cout << "copy assingment with copy-constructed temporary" << endl;
		rint tmp(other);  //For promising strong exception guarantee
		swap(tmp, *this); //
		return *this;
	}
#endif

	//Way 6: Move assignment operator
	//Overload cannot be disambiguously chosen among Way 3 (or 4) and move assignment operator
	//i.e., T&& and T cannot co-exist (but T&& and const T& do; T&& and T& also do)
	/*
	In general a move assignment operator should:
	1. Destroy visible resources (though maybe save implementation detail resources).
	2. Move assign all bases and members.
	3. If the move assignment of bases and members didn't make the rhs resource-less, then make it so.
	*/
	rint& operator=(rint&& other) noexcept {
		cout << "move assignment " << other.ri << endl;
		//when will this == &other happen? Probably when the original rint is 
		//passed in as an xvalue.
		if(this == &other) 
			return *this;

		delete ri; //no exceptions below (1)
		ri = other.ri; //(2)
		other.ri = nullptr; //(3)
		return *this;
	}

	//Way 7: construct a copy and pass it to move-assignent operator
	rint& operator=(const rint& other) {
		cout << "copy-constructed temporary followed by a move" << endl;
		rint tmp(other); 
		*this = move(tmp); //Note that we are moving the *whole* object
		return *this;
	}

#if 0
	//Way 8: Compiler generated function (by default, not generated):
	//It does not work correctly in this case, because the move assignment for this class 
	//is not member by member move (plus the base class move).
	//If we were to use smart pointer instead int* for ri, then default move-assignment would have been sufficient
	rint& operator=(rint&& other) noexcept = default;	
#endif

	//TODO: Make sure above function set are 
	//1. Exception safe. If not, discard that way.
	//2. Self-assignment is taken care.
	//3. No Code duplication. 
	//4. Performance in usual cases

	//It seems move-constructor along with copy-assignment operator with compiler assisted copy at argument is sufficient (for what?). Or is it?

	//The copy-assignment operator in Way 3, 4 can be called even with a const input argument. The compiler then invokes move 
	//or copy constructor (depending upon the argument to assingmnnet opertor is rvalue or lvalue) to pass the input by value.
	//Wait; #1) Doesn't (N)RVO already elide the copy by constructing the return value in the place of input argument? Then how
	//move constructor is helping us here?
	//#2) Move-ctor swaps default constructed members/content with the content of RHS. Then, the copy assignment again swaps it with content of LHS.
	//This can be costly if the class has lot of members (more swaps) with heavy default constructors (default ctor of the class is heavy).
	//We can get away with no default ctor and just one set of swaps if move assignment operator is implemented, isn't it? 
	//Probably the class should be designed using pimpl idiom so that only one swap (pimpl pointer) is required.

	//Way 3 and 4 invoke copy ctor for constructing input argument (and dtor when exiting). Way 1 and 2 do not.
	//This is a deciding factor in the speed of operator=.
	//Way 3, 4's copy-ctor (of other) + swaps + dtor (of old content of this) can be faster/slower compared to 
	//the custom logic in Way 1 and 2.
	//Passing by value in Way 3 and 4 can help elide the copies

	//Way 5 and 6: This seem be a nice set of assignment overloads. (Way 6 does not get along with Way 3 and 4 because of ambiguous overload signatures) 
	//All temporaries are directed to Way 6 and rest is handled by Way 5. 

	//Way 6 and 7: Seems even better because Way 7 reuses Way 6.
	
	//My class has std containers as members (vectors, strings etc). How can I do swap on them? I think we just std:move on them
	//This invokes their move assignment operator (which internally does swapping or something else - not our concern).
	//For e.g., str1 = move(str2) swaps the content of str1 and str2.

	//Conclusion for now: Have default ctor, copy ctor, move ctor, way 6 and 7

};

const rint func()
{
	return 5;
}

int main()
{
#if 0
	string one("1");
	cout << one << endl;
	cout << one.length() << endl;	
	cout << string("hello").append(" world") << endl;
	
	//fails: string &a = modifiable_rvalue();
	const string &b = modifiable_rvalue();
	string && c = modifiable_rvalue();
#endif	

	const rint r1 = 5; //unary ctor
	rint r2 = r1; //copy ctor
	rint r3 = 10;
	rint r4;
	r4 = r3; //copy-assignement
	r4 = move(r3);

	rint&& r5 = 11; //builds
	rint&& r6 = r4; //builds

#if 0
	rint r4 = func(); //copy is elided: just calls unary ctor
	rint r5(rint(10)); //thought move ctor would be invoked but nopes, compiler directly created r5!
#endif

	return 0;	
}

/*
lvalue and rvalue are property of expressions - not objects: an expression can create a temporary object (that expression is rvalue) that can be bind to a reference (expressions involving the reference can be lvalue). lvalues name objects that persist beyond a single expression. can take address of it.

++x is lvalue
std::string str("meow");
str is an lvalue (because it got name)

The following lvalues are not modifiable:
+ array type
+ const qualified type
+ a struct/union with one of their member as const
+ incomplete type (what? an incomplete type is not yet defined - but (forward) declared)

Rvalues are expressions that create anonymous temporary objects. rvlaues are temporaries that evaporate at the end of full expression. cannot take address of it because standard says that can be done for only lvalues. 

std:string("meow"), x++, 1729, a + b are rvalues
Foo make_foo(); //function declaration
f = make_foo();
Here make_foo() is an rvalue because it does not return a reference (see below).

Standard says: A reference type that is declared using & is called an lvalue reference, and a reference type that is declared
using && is called an rvalue reference.

Both l and r values can be const or modifiable.
Named as well as unnamed lvalue references are lvalues
Named rvalue references are lvalues
Unnamed rvalues references are rvalues (actually xvalues)

For e.g, vector<int> v(10, 1729); 
v[0] is unnamed lvalue reference (returns int&). Hence it is lvalue (I can take address of it). 

An rvalue reference behaves just like an lvalue reference except that it can bind to a temporary (an rvalue), whereas you can not bind a (non const) lvalue reference to an rvalue.

During initialization:
T& binds to modifiable lvalues only. It cannot bind to rvalues.

const T& can bind to everything - const or non-const l and rvalues. rvalues can bind to const T& (i.e., a const reference). Of course, such reference is an lvalue, though it refers to an rvalue.

WRONG: T&& can bind to modifiable l and rvalues! (Committee recognized fixed it)
rvalue references only bind to rvalues. const T&& can also bind to const and modifiable l an rvalues

It does not mean that we cannot modify rvalue: non-const member functions can be called on the modifiable rvalues:
string("hello ").append("world")

A function call is an lvalue if and only if the result type is a reference. In which standard?
C++ working draft (2013) says: 
A function call is an lvalue if the result type is an lvalue reference type or an rvalue reference to function
type, an xvalue if the result type is an rvalue reference to object type, and a prvalue otherwise.
If a function call is a prvalue of object type:
+ if the function call is either
	— the operand of a decltype-specifier or
	— the right operand of a comma operator that is the operand of a decltype-specifier,
	a temporary object is not introduced for the prvalue. The type of the prvalue may be incomplete.
	[ Note: as a result, storage is not allocated for the prvalue and it is not destroyed; thus, a class type is
	not instantiated as a result of being the type of a function call in this context. This is true regardless of
	whether the expression uses function call notation or operator notation (13.3.1.2). —end note ] [ Note:
	unlike the rule for a decltype-specifier that considers whether an id-expression is parenthesized (7.1.6.2),
	parentheses have no special meaning in this context. —end note ]
+ otherwise, the type of the prvalue shall be complete.

A function call is an lvalue if the result type is an lvalue reference type or an rvalue reference to function type, an xvalue if the result type is an rvalue reference to object type, and a prvalue otherwise.

Overloading:
It is sufficient to have overloads with const T& and T&&
(instead of having all: T&, const T&, T&&, const T&&). The latter is chosen for modifiable rvalue and the former takes care of the rest.

In the const T& overload of copy-assignment operator, swap cannot be performed on the input argument because of const-ness.
Is this annoying? For rvalues, the T&& overload is anyway will be chosen (and the functions should avoid const qualifying their results).
So what is the benefit of letting the compiler create a copy in case of non-const T&? Probably nothing because there is no RVO. RVO case is already handled by the move overload. Compiler at times uses RVO and NRVO to elide copies but not always. Multiple return statements, question-colon, returning argument itself inhibit (N)RVO.

T&& can bind to lvalue, but in the presence of const T& overload, the latter is preferred. (CHECK)
NOTE added later: I dont know whether T&& is talked in the context of templates (then it SEEMS to be called universal reference). 
If not in the context of templates, then you cannot bind an rvalue referene to an lvalue. Try this:
auto w = Widget{}; f(w);  <-- does not compile //where f(Widget&&) 

const T& can bind to rvalue, but in the presence of T&& overload, the latter is preferred. 
Note: if only T&& overload is implemented but neither of const T& and T& are implemented, then the compiler triggers error when function is called with lvalue! Strange given the fact that T&& can bind to both l and rvalues.

C++ working draft (2013):
An expression is an xvalue if it is:
+ the result of calling a function, whether implicitly or explicitly, whose return type is an rvalue reference
to object type [sandeep: for e.g., std::move],
+ a cast to an rvalue reference to object type
+ a class member access expression designating a non-static data member of non-reference type in which
the object expression is an xvalue, or
+ a .* pointer-to-member expression in which the first operand is an xvalue and the second operand is
a pointer to data member
In general, the effect of this rule is that named rvalue references are treated as lvalues and unnamed rvalue
references to objects are treated as xvalues; rvalue references to functions are treated as lvalues whether
named or not.

Function returning by value should return T instead of const T, the latter forbids move semantics optimization.

Move constructors and move assignment operators are never implicitly defined by the compiler.
A user defined copy constructor (or assignment operator) inhibits implicitly declared copy constructor, but not the move constructor (or assignment operator). (so we can just define move constructor, without bothering about copy constructor)

Standard allows to move not just rvalues but lvalues as well. For e.g., the std:swap (that takes lvalue references!) is now implemented as 
template <typename T>
void std::swap(T& a, T& b) {
 T tmp(move(a));
 a = move(b);
 b = move(a);
}

Things that are declared as rvalue reference can be lvalues or rvalues. The distinguishing criterion is: if it has a name, then it is an lvalue. Otherwise, it is an rvalue: For e.g.,
void foo(X&& x) {
 X y = x; //calls lvalue overload
}

Another subtlety: if move is not used below, then the copy constructor of Base would have been invoked instead of move constructor.
Derived(Derived&& rhs) 
  : Base(std::move(rhs)) // good, calls Base(Base&& rhs)
{
  // Derived-specific stuff
}

Guideline: Make move constructors/assignment operators are exception safe, and advertise that using noexcept keyword. This is necessary for the std library functions to use move overloads of our class in order to provide strong exception safety guarantee.


If a variable or parameter is declared to have type T&& (exactly T&&, not even const T&& or std::vector<T>&&) for some *deduced* type T, that variable or parameter is a universal reference. “&&” indicates a universal reference only where type deduction takes place.
In practice, almost all universal references are parameters to function templates. 
If the expression initializing a universal reference is an lvalue, the universal reference becomes an lvalue reference.
If the expression initializing the universal reference is an rvalue, the universal reference becomes an rvalue reference.

E.g.:
Widget&& var1 = someWidget;
auto&& var2 = var1
both var1 and var2 are lvalues.

On the other hand
template<typename T>
void f(T&& param);               // “&&” might mean rvalue reference
f(10) -> now param is indeed rvalue reference
where as 
int x = 10; f(x) -> param is lvalue reference (int&)

template<typename T>
void f(std::vector<T>&& param);     // “&&” means rvalue reference

template <class T, class Allocator = allocator<T> >
class vector {
public:
    ...
    void push_back(T&& x);       // fully specified parameter type -> no type deduction;
    ...                          // && is rvalue reference
};

More (subtle) examples:
Widget makeWidget();                       // factory function for Widget
 
Widget&& var1 = makeWidget()               // var1 is an lvalue, but
                                           // its type is rvalue reference (to Widget)
 
Widget var2 = static_cast<Widget&&>(var1); // the cast expression (RHS) yields an rvalue, but
                                           // its (RHS's) type is rvalue reference  (to Widget)
which is equivalent to: Widget var2 = std::move(var1);
The RHS above is a temporary (rvalue -- neither it has name nor we can take its address) and that temporary is of type rvalue reference to widget.

(The full expression invokes move constructor of Widget)
References of l and rvalues are lvalues:Named variables and parameters of rvalue reference type are lvalues. (You can take their addresses.) 

template<typename T>
class Widget {
    ...
    Widget(Widget&& rhs);        // rhs’s type is rvalue reference,
    ...                          // but rhs itself is an lvalue
};
 
template<typename T1>
class Gadget {
    ...
    template <typename T2>
    Gadget(T2&& rhs);            // rhs is a universal reference whose type will
    ...                          // eventually become an rvalue reference or
};

In Widget’s constructor, rhs is an rvalue reference, so we know it’s bound to an rvalue (i.e., an rvalue was passed to it), but rhs itself is an lvalue, so we have to convert it back to an rvalue if we want to take advantage of the rvalueness of what it’s bound to.  Our motivation for this is generally to use it as the source of a move operation, and that’s why the way to convert an lvalue to an rvalue is to use std::move.  Similarly, rhs in Gadget’s constructor is a universal reference, so it might be bound to an lvalue or to an rvalue, but regardless of what it’s bound to, rhs itself is an lvalue.  If it’s bound to an rvalue and we want to take advantage of the rvalueness of what it’s bound to, we have to convert rhs back into an rvalue. If it’s bound to an lvalue, of course, we don’t want to treat it like an rvalue.  This ambiguity regarding the lvalueness and rvalueness of what a universal reference is bound to is the motivation for std::forward:  to take a universal reference lvalue and convert it into an rvalue only if the expression it’s bound to is an rvalue.  The name of the function (“forward”) is an acknowledgment that our desire to perform such a conversion is virtually always to preserve the calling argument’s lvalueness or rvalueness when passing – forwarding – it to another function.

References to references are not permitted in C++11

std::move doesn’t itself do any moving. It merely converts its argument into an unnamed rvalue reference so that move optimizations can kick in.
std::move obtains an rvalue reference to its argument and converts it to an xvalue.

In "Inheritance is the base class of evil" talk, the following are implemented for the object_t class:
1. Unary ctor, 2. Copy ctor, 3. Copy-assignment operator (Way 7) and 4. Move assignment operator (Way 8)

*/
#endif