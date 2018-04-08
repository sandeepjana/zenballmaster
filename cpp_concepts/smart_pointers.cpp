#if 0
#include <iostream>
#include <string>
#include <memory>
#include <vector>
using namespace std;

class B
{
public:
	//don't want to be copied around
	B() { std::cout << "B ctor\n"; }
	B(const B&) { std::cout << "B copy ctor\n"; }
	~B() { std::cout << "B dtor\n"; }
};
class A
{
	std::shared_ptr<B> b;
public:
	A() : b(new B) { std::cout << "A ctor\n"; }
	~A() { std::cout << "A dtor\n"; }
};

struct Member
{
    Member() {
        std::cout << "Member ctor\n";
    }
    ~Member() { 
        std::cout << "Member dtor\n";
    }
};

/*
struct Widget
{
    shared_ptr<Widget> w;
    std::string name;
    Widget(std::string n) : name(n) {}
    ~Widget() {
        std::cout << name <<  "dtor\n";
    }

    Widget& operator=(const Widget&)
    {
        std::cout << "Copy\n";
    }

    Widget(Widget&&) = delete;
};
*/

struct Widget
{
    std::string name;
    Widget(std::string n) : name(n) {}
    ~Widget() {}
};

int accept_unique(unique_ptr<int> j)
{
	return 0;
}

template <typename T,
class = typename std::enable_if
    <
    !std::is_lvalue_reference<T>::value
    >::type
>
void receive_ownership(T&& t)
{
    // taking file descriptor of t, and clear t
}

void f(Widget&& w)
{

}

//using widget_ptr_t = Widget*;
typedef Widget* widget_ptr_t;
void g(widget_ptr_t ptr)
{

}

void take_ptr_by_rval(widget_ptr_t&& ptr)
{

}

widget_ptr_t factory()
{
    return widget_ptr_t{};
}

widget_ptr_t passthrucopy(widget_ptr_t same)
{
    return same;
}

widget_ptr_t passthruref(const widget_ptr_t& same)
{
    return same;
}


int main(int argc, char** argv)
{

    //auto w0 = Widget( "hello" );
    Widget w0{ "hello" };



#if 0
    unique_ptr<int> pi(new int);

    *pi = 5;

    accept_unique(move(pi));

    auto w = Widget{ "w" };
    //does not compile: f(w); //where f(Widget&&)
    f(Widget{ "" });
    g(&w);

    Widget* wnull = nullptr;
    auto wptr = &w;
    //does not compile: take_ptr_by_rval(wptr);
    take_ptr_by_rval(std::move(wptr));
    take_ptr_by_rval(&w);
    take_ptr_by_rval(factory());
    take_ptr_by_rval(passthrucopy(wnull));
    take_ptr_by_rval(passthruref(wnull));
    take_ptr_by_rval(new Widget{ "" });

    //does not compile: receive_ownership(wptr);
    receive_ownership(std::move(wptr));
    receive_ownership(&w);
    receive_ownership(factory());
    receive_ownership(passthrucopy(wnull));
    receive_ownership(passthruref(wnull));
    receive_ownership(new Widget{ "" });

    //std::vector<A> a(3);
    //auto b = a;

    auto w1 = make_shared<Widget>("w1");
    w1->w = w1;

    auto w2 = make_shared<Widget>("w2");

    auto ptr = new Widget{""};
    auto uptr = std::unique_ptr<Widget>(ptr);

    w1->w = w2;
    w2->w = w1;
#endif
	return 0;
}

#endif