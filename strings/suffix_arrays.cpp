#if 0
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>

#define PRINT(v, T) do{ copy(v.begin(), v.end(),\
 std::ostream_iterator<T>(std::cout, ",")); std::cout << std::endl; } while(0)

struct meta_t
{
    int id;
    int length;
    int val1; //prev val
    int val2; //update val
    int val; //combined val
};

std::vector<int> sort_suffix_strings_brute_force(const std::string& text)
{
    std::vector<int> start(text.size());
    std::iota(start.begin(), start.end(), 0);

    std::sort(start.begin(), start.end(), [&text](int a, int b)
    {
        //return text.substr(a) < text.substr(b); 
        return std::lexicographical_compare(text.begin() + a,
            text.end(), text.begin() + b, text.end());
    });

    return start;
}

std::vector<int> sort_strings(const std::vector<std::string>& vs)
{
    //std::vector<int> strIdToValue(vs.size());
    std::vector<meta_t> vm;
    int maxlen = INT_MIN;

    for (int i = 0; i < vs.size(); ++i)
    {
        auto& s = vs[i];
        vm.push_back({ i, s.size(), 0, 0, s[0] /* val */ });

        maxlen = std::max(maxlen, static_cast<int>(s.size()));
    }

    for (int i = 1; i < maxlen; ++i)
    {
        //modify the values and take a new character into account
        for (int j = 0; j < vs.size(); ++j)
        {
            auto& m = vm[j];
            m.val1 = m.val;
            m.val2 = (i < m.length) ? vs[m.id][i] : -1; //next character in string
        }

        //sort the (meta of) strings based on their previous value plus update value
        std::sort(vm.begin(), vm.end(), [](const meta_t& a, const meta_t& b)
        {
            auto p1 = std::make_pair(a.val1, a.val2);
            auto p2 = std::make_pair(b.val1, b.val2);
            return p1 < p2;
        });

        //find the combined value for each string
        //That depends not just on previous and update value of a particular string,
        //but also on the 
        for (int j = 0; j < vs.size(); ++j)
        {
            auto& m = vm[j];
            //Don't do: vm[j].val = j; <-- assings different numbers for same prefixes.

            //If the previous string value and this string value are 
            //the same (i.e., both share same prefix so far), and if their
            //update value is also the same, then they should have same combined value.
            //in this current turn

            //the position of elements in sorted vm tells how much they are in value
            // in ascending order
            m.val = (j > 0) && (m.val1 == vm[j - 1].val1 && m.val2 == vm[j - 1].val2)
                ? vm[j - 1].val : j;

            //strIdToValue[m.id] = m.val;
        }

        //PRINT(strIdToValue, int);
    }

    std::vector<int> ids;
    for (auto& m : vm)
    {
        ids.push_back(m.id);
    }

    return ids;
}

std::vector<int> sort_suffix_strings(const std::string& text)
{
    std::vector<meta_t> vm;
    //+
    std::vector<int> strIdtoValue;

    int maxlen = text.size();
    for (int i = 0; i < text.size(); ++i)
    {
        vm.push_back({ i, text.size() - i, -1, -1, text[i] /* val */ });
        //+
        strIdtoValue.push_back(text[i]);
    }

    //- for (int i = 1; i < maxlen; ++i)
    //+
    for (int twopower = 1; (twopower >> 1) < maxlen; twopower <<= 1) //TODO: why >> 1?
    {
        //modify the values and take a new bunch of characters into account
        for (int j = 0; j < text.size(); ++j)
        {
            auto& m = vm[j];
            m.val1 = m.val;
            //- m.val2 = (i < m.length) ? vs[m.id][i] : -1; 
            //+
            m.val2 = (m.id + twopower < text.size()) ? strIdtoValue[m.id + twopower] : -1;
        }

        std::cout << "\n";

        //sort the (meta of) strings based on their previous value plus update value
        std::sort(vm.begin(), vm.end(), [](const meta_t& a, const meta_t& b)
        {
            auto p1 = std::make_pair(a.val1, a.val2);
            auto p2 = std::make_pair(b.val1, b.val2);
            return p1 < p2;
        });

        //find the combined value for each string
        //That depends not just on previous and update value of a particular string,
        //but also on the 
        for (int j = 0; j < text.size(); ++j)
        {
            auto& m = vm[j];
            //Don't do: vm[j].val = j; <-- assings different numbers for same prefixes.

            //If the previous string value and this string value are 
            //the same (i.e., both share same prefix so far), and if their
            //update value is also the same, then they should have same combined value.
            //in this current turn

            //the position of elements in sorted vm tells how much they are in value
            // in ascending order
            m.val = (j > 0) && (m.val1 == vm[j - 1].val1 && m.val2 == vm[j - 1].val2)
                ? vm[j - 1].val : j;

            //+
            strIdtoValue[m.id] = m.val;
        }

        PRINT(strIdtoValue, int);
    }

    std::vector<int> ids;
    for (auto& m : vm)
    {
        ids.push_back(m.id);
    }

    return ids;
}
 
std::vector<int> construct_lcp(const std::string& text, const std::vector<int>& sa)
{
    std::vector<int> rank(text.size()); //sa-inverse
    for (int i = 0; i < rank.size(); ++i)
        rank[sa[i]] = i; // if rank[j] = sa^-1[j] then, rank[sa[i]] = i (here j = sa[i])

    std::vector<int> lcp_array(text.size()); //LCP[i] = lcp(text[sa[i]] = Ti, Ti predecessor)
    lcp_array[0] = -1; //NA

    int lcp = 0;
    for (int i = 0; i < text.size(); ++i) // for each index into text
    {
        int curlex = rank[i]; //current lexicographical index -- index in SA

        if (curlex == 0) continue; //by definition, LCP[0] is not defined

        int pretex = sa[curlex - 1]; //text index of predecessor
        
        while (i + lcp < text.size() && pretex + lcp < text.size() //dont go out of text boundary
            && text[i + lcp] == text[pretex + lcp])
        {
            ++lcp;
        }

        lcp_array[curlex] = lcp;
        lcp = lcp > 0 ? lcp - 1 : 0;
    }

    return lcp_array; //
}

int main(int, char**)
{
    auto text = std::string("mississippi$");
    //auto text = std::string("attcatg$");
    
    auto sa = sort_suffix_strings(text);


    //std::ifstream infile("some_text.txt");
    //std::vector<std::string> vs;

    //std::string s;
    //while (infile >> s)
    //{ 
    //    vs.push_back(s);
    //}

    //auto ids = sort_strings(vs);


    PRINT(sa, int);
    for (auto i : sa)
    {
        std::cout << text.substr(i) << "\n";
    }

    auto lcp = construct_lcp(text, sa);
    PRINT(lcp, int);

    return 0;
}
#endif