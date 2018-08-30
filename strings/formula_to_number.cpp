/* 
Excel style formula such as A1 + B5 - C99 + 33 to the number
Term format: [one letter][one or two one-digit numbers]
*/
#if 0
#include <iostream>
#include <cassert>

typedef const char cchar;
typedef long long llong;
enum Type { END, INDEX, PLUS, MINUS, VAL };
struct Index { int ro, co; };

bool is_number(char c) {
	return ('0' <= c && c <= '9');
}
bool is_alpha(char c) {
	return ('A' <= c && c <= 'Z');
}
//post: s points to the character next to the digit sequence
llong read_number(cchar*& s)
{
	llong v = 0;
	while (1)
	{
		if (is_number(*s)) v = 10 * v + (*s - '0');
		else break;
		++s;
	}
	return v;
}
//post: s points to the character next to this term, except when END is returned
Type get_term(cchar*& s, Index& i, llong& v)
{
	assert(s != nullptr);
	Type t = END;
	while (*s && ' ' == *s) ++s;
	char c = *s;
	if (is_number(c))	t = VAL, v = read_number(s);
	else if (is_alpha(c)) t = INDEX, i.ro = (c - 'A'), i.co = read_number(++s);
	else if ('+' == c) t = PLUS, ++s;
	else if ('-' == c) t = MINUS, ++s;
	return t;
}

int main(int, char**)
{
	using namespace std;
	cchar* s = "A1 + B1- C3 +33 ";
	cchar* ss = s;
	llong v; Index i;
	while (1)
	{
		Type t = get_term(ss, i, v);
		if (END == t) break;
		if (VAL == t) cout << v << '\n';
		else if (INDEX == t) cout << i.ro << " " << i.co << '\n';
		else std::cout << (PLUS == t ? '+' : '-') << '\n';
	}
	return 0;
}
#endif

