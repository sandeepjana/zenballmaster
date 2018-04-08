#if 0
#include <iostream>
#include <utility>
#include <vector>
#include <memory>
#include <string>
#include <cassert>
using namespace std;

//Goal is to write a vector that can take different types provided by the client and print them, without 
//needing the client to inherit from any base class but just requiring to provide an overloaded print/draw function.

/*
The runtime-concept idiom allows polymorphism when needed without inheritance.
• Client isn't burdened with inheritance, factories, class registration, and memory management.
• Penalty of runtime polymorphism is only payed when needed.
• Polymorphic types are used like any other types, including built-in types.
*/

/*
Type erasure is a technique to handle different data types without restoring to inheritance. This doesn't force client classes to inherit from a base class. TE is implemented using templates and inheritance inside, but in a manner that client doesn't need to bother about. It solves problem of this kind: how to store int, char, my_class all in the same std::vector. If inheritance were to be used, int should inherit from a base class, and that base class would be the type of the vector. TE during compilation time creates derived classes (models) that inherit from a base class (concept). The (smart) pointer of derived class object can be stored and deferenced from its base class object pointer. Inheritance helps here to keep the model in some other file/DLL (unlike templates).

Type erasure works by creating a class that can accept arbitrary data type T for its constructor by using templated ctor inside. T is wrapped by a class which in turn inherits from a base class of virtual functions such as 'draw'. The wrapper class invokes type specific 'draw'. This function is implemented by client code either as an overload or a member function of that class, or the library itself can provide a templated draw.
*/

//let us have a draw template to handle built-in types and any type that can be fed to <<
template <typename T>
void draw(const T& drawable, ostream& out, size_t position)
{
	out << string(position, ' ') << drawable << endl;
}

class object_t {
public:
	//the object ctor should be able accept different types: so make it a template
	template<typename T>
#ifdef UNIQUE_PTR_VERSION
	object_t(const T& x) : object_(new model<T>(x)) {
#else
	//object_t(const T& x) : object_(make_shared<model<T>>(x)) {
	//even better to pass by value:
	object_t(const T x) : object_(make_shared<model<T>>(move(x))) {
#endif
		cout << "object_t unary ctor\n";
	}

#ifdef UNIQUE_PTR_VERSION
	//object_t should have copy constructor because vector needs it
	//why can't we use default copy constructor? unique_ptr disallows simple member by member copying
	//This fails because unique_ptr does not have copy constructor:
	//object_t(const object_t& other) : object_(other.object_) {}
	//So deep-copy is needed:
	object_t(const object_t& other) : object_(other.object_->copy_()) {
		cout << "object_t copy ctor\n";
	}
	//above member initialization = object_(new model(*this of model))
#else
	//implicit copy contrstor is fine -- it will just copy the shared ptr 
#endif
	object_t(object_t&& x) : object_(move(x.object_)) {
		cout << "object_t move ctor \n";
	}

    object_t& operator=(object_t x) //<-- copy
    { 
		cout << "object_t operator=\n";
		object_ = move(x.object_); return *this; 
	}

	//friend is needed because we are dereferencing  obj
	//friend is therefore useful when a function needs to be overloaded based on several 
	//classes (our as well standard classes such as 'string') and built-in types.
	//For the classes that we can control we could make that function a member but we cannot do the same
	//for the built-in types and classes of other libraries. So in order to maintain uniformity, we make that 
	//function a friend
	
	//a friend function is not part of the class even though it is defined here. It is as good as a free function in the enclosing namespace
	friend void draw(const object_t& obj, ostream& out, size_t position) {
		//This does not work: error C2259: 'object_t::concept_t' : cannot instantiate abstract class
		//'static_cast' : cannot convert from 'const object_t' to 'object_t::concept_t'
		//obj.object_->draw_(static_cast<concept_t>(obj), out, position);

		//Here the polymorphism helps choose the correct model because the draw_ function is virtual
		obj.object_->draw_(out, position);
	}

private:
	//concept is a set of semantic and syntactic requirements on types
	struct concept_t {
		//virtual ~concept_t() = default; //unfortunately, does not compile in MSVC 2010
		virtual ~concept_t() { }

		//the copy constructor needs to know the exact type. so make it virtual.
		//notice the return type: a pointer. This is because the copy is new'ed.
		//http://www.parashift.com/c++-faq/copy-of-abc-via-clone.html
#ifdef UNIQUE_PTR_VERSION
		virtual concept_t* copy_() = 0; //virtual constructor idiom
#endif
		virtual void draw_ (/*const concept_t& base, */ostream& out, size_t position) const = 0; //the custom function
	};

	template<typename T>
	struct model : concept_t { //the defaul inheritance access for struct (class) is public (private)
		model(const T& x) : data_(x) {
			cout << "model unary ctor\n";
		}
#ifdef UNIQUE_PTR_VERSION
		virtual model* copy_() { // the return type in the derived class is intentionally different from the one in the base class
			cout << "model copy ctor\n";
			return new model(*this); //why not new model<T>(*this)? Because T is already decided
			//wait, above statement uses copy constructor, but it is not explictly defined! I can also use
			//return new model(this->data_);
		}
#endif
		virtual void draw_(/*const concept_t& base, */ostream& out, size_t position) const {
			//In practice, draw_ method might do lot of work such as initializing a display or printer etc here.
			//Since this is not a template code, this portion can reside in another file/DLL

			//call T's overload of the draw function
			//We dont implement the draw function here because this is 'the custom' function that the clients want to 
			//override by providing the overloads.
			//overload:
			draw(data_, out, position);
			//or invoke member function: Hmm.. for built-in types invoking member function does not make sense!
			//data_->draw(...);
		}

		T data_;
	};

	//concept_t is our base class/struct with pure virtual functions
	//since we are using unique ptr, virtual destructor may not be required, isn't it?: No, it is required.
#ifdef UNIQUE_PTR_VERSION
	unique_ptr<concept_t> object_;
#else
	shared_ptr<const concept_t> object_; //const is good, but not mandatory
#endif
};

/********** Client Code *********/

typedef vector<object_t> document_t;
void draw(const document_t& doc, ostream& out, size_t position)
{
	out << string(position, ' ') << "<document>\n";
	for(auto it = doc.begin(); it != doc.end(); ++it) {
		draw(*it, out, position + 2);
		//above draw invokes object_t's draw which in turn invokes model's draw via
		//base class (concept_t) pointer using polymorphism
	}
	out << string(position, ' ') << "</document>\n";
}

class my_class_t {};
void draw(const my_class_t& ,  ostream& out, size_t position)
{
	out << string(position, ' ') << "my_class" << endl;
}

/* problem with typedef-ing the built-in type:
typedef does not create a new type, but just creats another name. So cannot overload with 
two differents names aliasing the same type. For e.g., the following may not chose 'draw' overload of my_int_t,
but that of integer:
typedef int my_int_t;
void draw(const my_int_t& ,  ostream& out, size_t position)
{
	out << string(position, ' ') << "my_int_t" << endl;
}
*/

typedef vector<document_t> history_t;
#if 1
document_t& current(history_t& h) 
{ 
	assert(h.size()); 
	return h.back();
}
void commit(history_t& h)
{
	assert(h.size()); 
	h.push_back(h.back()); //repeating the last document?!
}
void undo(history_t& h)
{
	assert(h.size()); 
	h.pop_back();
}
#endif
int main()
{
	document_t doc;
	doc.reserve(20);
	doc.emplace_back(0); //0' is converted to object_t using the latter's unary (templated) ctor
	doc.emplace_back(1);
	doc.emplace_back(string("hello"));
	doc.emplace_back(doc); //Line @
	//doc.emplace_back(my_class_t()); //notice how my_class_t does not need to inherit from anything.
	//it just needed a draw function overload

	draw(doc, cout, 0);

	doc[2] = string("world"); //Copy On Write happens!
	//A new object_t is created for string "world". The shared ptr in doc[2] 'resets' to this new object.
	//Note that content of the copy of doc at line @ does not modify: There are two copies of the same shared pointer
	//before this assignment. After the assignment, the copy of pointer is modified -- not the CONTENT. The pointer in the  
	//doc[2] object modified to point to a new object, leaving the old copy of the pointer and content intact.

	cout << "---------------------\n";

	draw(doc, cout, 0);

	return 0;
}


/*
Templates:
1. Increase code size, compile time
2. Hard to debug
3. Code in headers <-- cannot hide implementation
4. Faster programs
5. Type-safety (type mismatches are detected at compile time)

void* based type-erasure:
1. It is not type-safe (does not check the type).
2. It may require the data to be contiguous because void* just gives the starting address, say, in a binary search example

OO-based type-erasure:
1. Provides type-safety because the operations on the derived class will be done by "dynamic casting" of the base class pointer. However this causes run-time exception rather than compile time error.
2. But since inheritance is required, built-in types cannot be directly used as derived classes: user’s types must now inherit from Object or else they won’t work with our component. 
3. Also, components from different third parties (sort function from one, and predicate from other) cannot be used together because the inheritance requirements would be different
4. Need to work on pointers rather than the value types
5. Similar to void* type-erasure, our component only works with pointers as iterators. We cannot search in a map or anything but a raw array.


*/

//http://stackoverflow.com/questions/18453145/how-is-stdfunction-implemented
#endif