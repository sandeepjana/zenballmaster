#if 0
#include <iostream>
#include <algorithm>
#include <utility>
#include <exception>
#include <string>

/* Notes:
Exception neutral: we (the library) allow the exception to propagate to the client code unmodifed
Mutator functions should not return objects by value. Because the copy in the return statement can throw.

All containers provide at least basic guarantee for all operations
All containers must provide strong guarantee except two exceptions:
	1. For all, multi element inserts ("iterator range" inserts) are never strongly exception safe
	2. For vector<T> and deque<T>, inserts and erases (whether single or multi element) are strongly exception safe
		as long as the copy and assignment operators of T do not throw. For e.g., insert of vector<string> is not strongly exception safe.
All node-based containers of STL such as list, set, map, multimap provide strong exception guarantee.
All iterators returned by the standard containers are exception safe.

*/

/* stack_impl_t will be such that v_ buffer will contain as many constructed T objects
as T objects pushed into the stack -- i.e., no default constructed dummy T objects */
template <typename T>
struct stack_impl_t
{
	stack_impl_t(size_t size) : //TODO: what is operator new? It just allocates memory -- it does not call the constructor of the object
		v_(static_cast<T*> (size == 0 ? NULL : operator new(sizeof(T)*size))), 
		size_(size),
		used_(0) 
	{
	}
	
	//subtle: call destructor for the objects stored!
	//this is the most important aspect: when a temporary object of stack is created (for latter swapping),
	//if the construction/push of stack throws after some populating/pushing some objects, this destructor makes sure that
	//no leak happens.
	~stack_impl_t()
	{
		T* first = v_;
		T* last = v_ + used_;
		while(first != last)
		{
			first->~T();
			first++;
		}
		operator delete (v_);
	}

	void swap(stack_impl_t& other)
	{
		//using namespace std; //TODO: dragging std here does not overload swap properly!
		std::swap(v_, other.v_);
		std::swap(size_, other.size_);
		std::swap(used_, other.used_);
	}


	T* v_;
	size_t size_;
	size_t used_;
};

struct my_exception : public std::exception
{
	std::string msg;
	my_exception(const std::string& s) : msg(s) {}
	~my_exception() {}
	//if you really want to use override keyword, then use the exact signature:
	//const char* what() const noexcept override { return msg.c_str(); }	 
	//otherwise, just go with...
	const char* what() { return msg.c_str(); }
};

template <typename T>
class my_stack_t 
{
public:
	my_stack_t(size_t size = 0) : impl_(size) {}
	~my_stack_t() {}
	//subtle:
	my_stack_t(my_stack_t& other) : impl_(other.impl_.used)
	{
		while(impl_.used_ != other.impl_.used)
		{
			T* ptr = impl_.v_ + impl_.used;
			//TODO: read more about placement new
			new (ptr) T(other.impl_.v_[other.impl_.used]);
			impl_.used_++;
		}
		//even if the above T ctor throws, the destructor of impl_ will make sure that there are no leaks
	}

	my_stack_t& operator=(my_stack_t other)
	{
		impl_.swap(other.impl_);
		return *this;
	}
	
	size_t count()
	{
		return impl_.used_;
	}
	void push(const T& t)
	{
		//if the stack is full, reallocate
		if(impl_.used_ == impl_.size_)
		{
			my_stack_t temp(impl_.size_ * 2 + 1);
			while(temp.count() != impl_.size_)
			{
				//push causes the count to increase!
				temp.push(impl_.v_[temp.impl_.used_]);
			}
			temp.push(t);

			impl_.swap(temp.impl_);
		}
		else
		{
			T* ptr = impl_.v_ + impl_.used_;
			new (ptr) T(t);
			++impl_.used_;
		}
	}

	void pop()
	{
		if(impl_.used_ == 0)
		{
			throw my_exception("empty stack");
		}

		--impl_.used_;
		T* ptr = impl_.v_ + impl_.used_;
		ptr->~T();
	}

	T& top()
	{
		if(impl_.used_ == 0)
		{
			throw my_exception("empty stack");
		}

		return impl_.v_[impl_.used_ - 1];
	}

private:
	stack_impl_t<T> impl_;
};

int main()
{
	my_stack_t<std::string> stk(2);

	stk.push("hello");
	stk.push("world");
	stk.push("cheers!");

	while(0 != stk.count())
	{
		std::cout << stk.top() << std::endl;
		stk.pop();
	}

	return 0;
}

#endif