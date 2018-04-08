#if 0
//http://www.iti.fh-flensburg.de/lang/algorithmen/pattern/horsen.htm
/* worst case: O(mn) when haystack is a^n, and needle is ba^n
best case: O(n/m)
*/


#include <iostream>
//choosing 128 avoids us constantly subtracting the offset '0' or 'a' from each character
#define ALPHABET_SIZE 128 //('z' - '0' + 1) 
int occurrence[ALPHABET_SIZE];

void horspool_init(const char* needle)
{
    for (int i = 0; i < ALPHABET_SIZE; ++i)
        occurrence[i] = -1;

    int needle_length = strlen(needle);

    //Note the loop bound: it excludes the last character of needle.
    //Also, the ascending order of the loop iterator makes sure
    //that the right-most occurrence position of a character in the needle
    //is filled in occurrence array.
    for (int i = 0; i < needle_length - 1; ++i) 
        occurrence[needle[i]] = i;
}

void horspool_search(const char* haystack, const char* needle)
{
    int needle_length = strlen(needle);
    int haystack_length = strlen(haystack);

    //Note the loop bound
    for (int i = 0; i < haystack_length - needle_length + 1; /* i is updated inside */)
    {
        bool match = true;
        for (int j = needle_length - 1; j >= 0; --j)
        {
            if (haystack[i + j] != needle[j])
            {
                match = false;
                break;
            }
        }
        
        if (true == match) 
            std::cout << "Match at " << i << std::endl;

        //now irrespective of match/mistmatch, 
        //consider the i + (needle_length -1) character of the 
        //haystack (rightmost character in the current haystack window) 
        //for further searches.
        char last = haystack[i + needle_length - 1];
        i = i + (needle_length - 1) - occurrence[last];
    }

}

int main(int, char**)
{
    char needle[] = "ababaa";
    char haystack[] = "ababbababaababaa";  

    horspool_init(needle);
    horspool_search(haystack, needle);

    return 0;
}
#endif