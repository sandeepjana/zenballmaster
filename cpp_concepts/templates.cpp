#if 0
#include <iostream>
#include <type_traits>
#include <vector>
#include <memory>
#include <array>

template<typename T>
void f(T&& t)
{
	std::cout << __FUNCSIG__ << "\n";
}

template<class T>
T mini(T a, T b) {
	return a < b ? a : b;
}

template <typename T>
struct is_void
{
	static const bool value = false;
};

template <>
struct is_void<void>
{
	static const bool value = true;
};

template<typename T>
const bool is_void_v = is_void<T>::value;

//read: http://stackoverflow.com/questions/28828159/usage-of-stdforward-vs-stdmove

template<int N>
struct Abs
{
	static_assert(N != 0, "hmm");
	static constexpr int value = N < 0 ? -N : N;
};

template<int N>
constexpr int Abs2 = N < 0 ? -N : N;

constexpr int Abs3(int N) { return N < 0 ? -N : N; }

template<int M, int N>
const int gcd1 = gcd1<N, M % N>;

//template<int M>
//const int gcd1<M, 0> = M;

template<int M, int N>
struct gcd2 {
	static const int value = gcd2<N, M % N>::value;
};

template<int M>
struct gcd2<M, 0> {
	static const int value = M;
};

//rank: number of dimensions of an array
template <class T>
constexpr size_t rank = 0;

template <class T, size_t N>
constexpr size_t rank<T[N]> = rank<T> + 1;

template <class T>
struct Rank {
	static const size_t value = 0;
};
template <class T, size_t N>
struct Rank<T[N]> {
	static const size_t value = Rank<T>::value +1;
};

//remove const 
template<class U>
struct remove_const {
	using type = U;
};
template<class U>
struct remove_const<U const> {
	using type = U;
};

template<class T>
struct type_is {
	using type = T;
};

//compile time if
//obtain type T or type U depending a boolean template argument
//trick: inherit from different stuctures depending on the predicate
template<bool, class T, class>
struct IF : type_is<T> {};

template<class T, class F>
struct IF<false, T, F> : type_is<F> {};

//or using the more straight-forward way
template<bool, class T, class F>
struct IF2 {
	using type = T;
};

template<class T, class F>
struct IF2<false, T, F> {
	using type = F;
};

//SFNAE example (alternatives such as tag dispatch are preferred)
//overloading a function depending on a type
template<typename T>
std::enable_if_t< std::is_integral<T>::value, void> foo(T ival)
{
	std::cout << "Integer specific logic\n";
}

template<typename T>
std::enable_if_t< std::is_floating_point<T>::value, void> foo(T fval)
{
	std::cout << "floating point specific logic\n";
}
////note: another way (without helper -- enable_if_t)
//template <typename T>
//typename std::enable_if< std::is_integral<T>::value, int>::type foo(T ival)
//{
//	return 0;
//}
//see the ::type -- this is the member that would be absent (and result in 
//compilation error) if enable_if takes a "false" argument.

//same goal as above but using tag dispatching
template<typename T>
void bar_impl(const T&, std::true_type)
{
	std::cout << "Integer specific logic\n";
}
template<typename T>
void bar_impl(const T&, std::false_type)
{
	std::cout << "floating specific logic\n";
}

template<typename T>
void bar(T val)
{
	//at the end brackets are necessary to create a default object
	bar_impl(val, typename std::is_integral<T>::type());
}
//note that above you got only either true or false case
//so bar(some_string) will resolve to floating point.
//secondly, in foo you used ::value, 
//in bar you used ::type()

template<typename T>
void bar2(T val)
{
	using type = std::is_integral<T>::type;
	bar_impl(val, type());
}

template<typename T>
void bar3(T val)
{
	//using type = std::enable_if_t<std::is_integral<T>::value, std::true_type>;
	using type = IF<std::is_integral<T>::value, std::true_type, std::false_type>::type;
	bar_impl(val, type());
}

//check if the type passed is indeed float
template<typename T>
void bar4(T floatval)
{
	static_assert(std::is_floating_point<T>::value == true, "Not a floating number");
	//static_assert(std::is_same<T, float>::value ||
	//	std::is_same<T, double>::value, "Not a floating number");
	std::cout << __FUNCSIG__ << "\n";
}

//declval<U>() returns a temporary
//declval<U&>() returns an lvalue

//check copy assignability 
template<typename T>
struct my_is_copy_assignable {
private:
	//the goal is to write a function DECLARATION to check if compiles (SFINAE)
	template<class U, //now comes a placeholder for compilation check
		class /* not naming type because not using anyway */
		= decltype(std::declval<U&>() = std::declval<U const&>())> //assingning one lvalue to another lvalue
	static std::true_type try_assignment(U&&);
	//catch all function declaration -- not a template
	static std::false_type try_assignment(...); 
public:
	using type = decltype(try_assignment(std::declval<T>()));
};

//variadic templates
void print_stuff() {
	std::cout << '\n';
}
template<typename Tfirst, typename... Trest>
void print_stuff(Tfirst first, Trest... rest)
{
	std::cout << first << ' ';
	print_stuff(rest...);
}

//tuple!
//Involves variadic templates, inheritance, and static_casting
//Construction:
template<typename... T> //the parameter to this template is a variadic parameter pack
class Tuple; //just forward declaration? Is it required?
//base case:
template<>
class Tuple<> {}; //strange syntax
//recursive defintion: 
template <typename Tfirst, typename... Trest>
class Tuple<Tfirst, Trest...> : public Tuple<Trest...>
{
	Tfirst value;
};
//Access:

int main(int argc, char** argv)
{
	float a = 10.f;
	int b = 5;
	auto c = mini<float>(a, b);

	//std::cout << std::boolalpha << is_void<void>::value << "\n";
	//std::cout << std::boolalpha << is_void<int>::value << "\n";
	//std::cout << std::boolalpha << is_void_v<int> << "\n";

	//int i = 5;
	//f(i); //void __cdecl f<int&>(int &)
	//f(7); //void __cdecl f<int>(int &&)

	//std::cout << Abs<-50>::value << "\n";
	//std::cout << Abs2<-50> << "\n";
	//std::cout << Abs3(argc) << "\n";
	////constexpr int a = Abs3(argc); <-- this does not compile

	//std::cout << gcd1<9, 6> << "\n";
	//std::cout << gcd2<9, 6>::value << "\n";

	//using array_t = int[5][6][7];
	//std::cout << Rank<array_t>::value << "\n";
	//std::cout << rank<array_t> << "\n";

	//const int q = -5;
	//IF<(q < 0), int, unsigned>::type a; //a is type int!

	//IF2<(q < 0), int, unsigned int>::type b; //same

	//foo(5.0f);
	//foo(5);
	////foo("wont compile");

	//std::cout << "bar\n";
	//bar(5);
	//bar("hello");

	//std::cout << "bar2\n";
	//bar2(5);
	//bar2(5.0);
	//bar2("world");

	//std::cout << "bar3\n";
	//bar3(5);
	//bar3(5.0);
	//bar3("world");

	//bar4(5.0);
	//const float f = 5.f;
	//auto& ff = f;
	//bar4(ff);
	////does not compile: bar4(5);

	//using tt = std::is_copy_assignable<int>::type;
	//using type = my_is_copy_assignable<std::vector<float>>::type;

	//std::cout << std::boolalpha << std::is_same<type, tt>::value << '\n';
	//std::cout << std::boolalpha << my_is_copy_assignable<std::unique_ptr<int>>::type::value << "\n";

	//std::cout << std::boolalpha << std::true_type::value << '\n';

	print_stuff(4, 4.5, "hello");

}
#endif
