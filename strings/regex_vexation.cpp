#if 0
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <regex>

using namespace std;

//case sensitive matching by default
//a|b+ matches a, b, bb etc
//(a|b)+ matches a, b, ab, ba, aab etc
//wild card '.' matches anything except newline
//remember the anchors ^ and $
//Bracket expressions: [ch]at (that mathces hat and cat)
//[^b]at matches cat but not bat
//character classes with bracket expression:
//[[:xdigit:]] matches any hexdecimal digit
//Then escapes: \d, \s, \w, \D, \b, \B etc
//Positive and negative asserts (need to read)
//Quantifiers: *, +, and ? (zero or one)
//{3}, {3, } and {3,5} (three to five inclusive)
//greedy by default. Include '?' for non-greedy like this:
// a.*?b means from a to the 'first' occurance of b

//Regex: ([A-Z]+)-([0-9]+)
//String: 2161-NCC-1701-D
//Escape sequence – Replaced by:
//$1 – What matches the 1st capture group (e.g. NCC)
//$2 – What matches the 2nd capture group (e.g. 1701)
//$& – What matches the whole regex (e.g. NCC-1701)
//$` – What appears before the whole regex (e.g. 2161-)
//$' – What appears after the whole regex (e.g. -D)
//$$ – $
void smack_a_smatch()
{
	std::vector<std::string> vs;
	vs.emplace_back("02-10-1986");
	vs.emplace_back("2/10/1986");
	vs.emplace_back("2-10-1986");
	vs.emplace_back("02/10/1986");

	std::vector<const std::regex> dates;
	dates.emplace_back("(\\d{2})/(\\d{2})/(\\d{4})");

	for(auto it = vs.begin(); it != vs.end(); ++it)
	{
		std::string& s = *it;
		std::smatch m;
		for(auto jt = dates.begin(); jt != dates.end(); ++jt)
		{
			bool b = std::regex_match(s, m, *jt);
			if(b)
			{
				std::cout << "Entire match " << m[0] << "\n";
				std::cout << "First submatch " << m[1] << "\n";
				std::cout << "Second submatch " << m[2] << "\n";
				std::cout << "Third submatch " << *(m.begin() + 3) << "\n";

				printf("Second submatch range [%d, %d)\n", m[2].first - s.begin(),
					m[2].second - s.begin());
			}
		}
	}

}

void stepped_in_manhole_coz_lookahead()
{
	const std::regex r("?=(abab)"); //crashes!
	const std::string s("kababab");

	sregex_iterator end;
	for(sregex_iterator it(s.begin(), s.end(), r); it != end; ++it)
	{
		std::cout << ".........\n";
		std::smatch m = *it;
		for(auto sit = m.begin(); sit != m.end(); ++sit)
		{
			std::cout << *sit << "\n";
		}

		printf("Position %d, length %d \n", m.position(), m.length());
	}

}

void replace_example()
{
	//regex_replace is global replacement by default
	//regex_replace( string, regex, replacement string)
	const regex r("a(b+)(c+)d");
	//const string fmt("whatever"); //replace with just plain string
	const string fmt("$2$1"); //swap the matches

	for(string s; getline(cin, s); )
	{
		smatch m;	//match result		

		string repl = regex_replace(s, r, fmt);
		cout << repl << endl;
		cout << s << endl;
	}
	//regex: a(b+)(c+)d
	//fmt: $2$1
	//input: XXabbccdYY
	//output: XXccbbYY
}

int main(int , char ** )
{
	std::cout << std::boolalpha;
	smack_a_smatch();


	////regex_search: match any substring
	//const regex r("a(b+)(c+)d");

	//for(string s; getline(cin, s); )
	//{
	//	smatch m;	//match result		

	//	const bool b = regex_search(s, m, r);
	//	if(b)
	//	{
	//		cout << m[0] << endl; // retuns the substring of match
	//		cout << m[0].first - s.begin() << endl;
	//		cout << m[0].second - s.begin() << endl;
	//		// [first, second) is the index of the match (index starts with zero)
	//		cout << m[1] << endl; // substring of first capture group
	//	}
	//}
#if 0
	//multiple matches & iterators
	const regex r("a(b+)(c+)d");
	string s("XXabbccdYYabbbcccdYY");

	sregex_iterator end;
	for(sregex_iterator it(s.begin(), s.end(), r); it != end; it++) //NOTE!!
	{
		cout << "match at index " << (*it)[0].first - s.begin() << endl;
		cout << "second capture group started at " << (*it)[2].first - s.begin() << endl;
		cout << (*it)[2] << endl; //print the second capture group
	}
	//match at index 2
	//second capture group started at 5
	//cc
	//match at index 10
	//second capture group started at 14
	//ccc
#endif
#if 0
		//strtok style -- say parse CSV
		const regex r("\\s*([a-zA-Z\\s]+)\\s*(,|$)");
		//const regex r("\\s*([^,]+)\\s*(,|$)");
		string s(" Zen, ball,master blaster,, says,,  ,hello");		

		for(sregex_iterator it(s.begin(), s.end(), r), end; it != end; it++)
		{
			cout << "*" << (*it)[1] << "*" << endl;
		}
		//*Zen*
		//*ball*
		//*master blaster*
		//*says*
		//* *
		//*hello*

		//Another cool variant -- not just by comma
		const regex r2("\\w+");
		sregex_iterator i(s.begin(), s.end(), r2), end;
		//vector<string> result<regex_iterator>(i, end);
		vector<string> tokens;
		for(; i != end; i++)
		{
			tokens.push_back((*i)[0]);
		}
		PRINT(tokens, string);
		tokens.clear();

		//Few pages above we have seen stringstream
		//Another variation
		char delim(',');
		
		stringstream ss(s);
		string item;
		while(getline(ss, item, delim))
		{
			tokens.push_back(item);
		}
		copy(tokens.begin(), tokens.end(), ostream_iterator<string>(cout, "\n"));
		// Zen
		// ball
		//master
		//
		// says
		//
		//
		//hello
#endif

#if 0
	//still regex
	{
		const std::regex r("k(a|b|ab)");
		const std::string s("kabab");

		sregex_iterator end;
		for(sregex_iterator it(s.begin(), s.end(), r); it != end; ++it)
		{
			std::cout << ".........\n";
			std::smatch m = *it;
			for(auto sit = m.begin(); sit != m.end(); ++sit)
			{
				std::cout << *sit << "\n";
			}
		}

	}
#endif

	//still regex


}
#endif
