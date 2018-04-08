#if 0
//http://www.inf.fh-flensburg.de/lang/algorithmen/pattern/kmpen.htm
#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 128
char needle[] = "aaaaa";//"ababaa";
char haystack[] = "aaaaxaaaaxaaaax";//"ababbababaababaa";  

#if 0
/* def: a border is a proper (means not the entire string) prefix and suffix */
/* In preprocessing step, an array b[] is computed such that
	b[i] is length of the widest border for needle prefix of length i */
/* The array b is useful because at the point of mismatch while searching,
	fresh search can start from the position b[i] of the needle */
int kmp_preprocess(int * b, const char * const needle)
{
	int i, j;
	int needle_length = strlen(needle);
	char newchar;

	b[0] = -1; /* string of length 0 */
	b[1] = 0; /* no proper border */

	for(i = 2; i <= needle_length; i++)
	{
		/* compute b[i] */
		/* b[i] is the length of widest border 
				   for needle[0... i-1] */
		/* extend border of needle[0... i-2] below */
		newchar = needle[i-1];
		j = i - 1; /* considering the border for 
				   needle prefix of length (i-1) */
		b[i] = 0; /* worstcase guess */
		while(j >= 0) /* until we have some valid border */
		{
			/* needle[b[j]] gives the character "next"
			to the widest border of needle prefix of length j */
			if(newchar == needle[b[j]])
			{
				b[i] = b[j] + 1;
				break;
			}
			else
			{
				/* check border of border */
				j = b[j];
			}
		}
	}

	return 0;
}

int kmp_search(int * b, const char * const needle, const char * const haystack)
{
	int i, j;
	int needle_length = strlen(needle);
	int haystack_length = strlen(haystack);
	char newchar;

	j = 0;
	i = 0;
	/* not a for-loop because i is incremented conditionally */
	while(i < haystack_length) 
	{
		newchar = haystack[i];
		if(newchar == needle[j])
		{
			j++; i++;
			if(j == needle_length)
			{
				/* got a complete match */
				printf("Got a match at index %d\n", i-j);
				j = b[j]; 
			}
		}
		else
		{
			if(j > 0)
			{
				/* one or more characters in needle are matched previously */
				/*  since the match is broken now, try with the border of 
				previous match */
				j = b[j];
				/* now, needle[j] points to the character next 
				to the border. It will be compared with haystack[i] */
			}
			else
			{
				i++;
			}
		}
	}

	return 0;

}
#else
//eerily succint
int kmp_preprocess(int * b, const char * const needle)
{
    int i = 0;
	int j = -1;
	int needle_length = strlen(needle);

    b[i] = j; //b[0] = -1; because by definition

    while(i < needle_length)
    {
		while(j >= 0 && needle[i] != needle[j])
			j = b[j];

        i++;
		j++;
        b[i]=j; //if no border at all, then j = 0.
    }

	return 0;
}
int kmp_search(const int * const b, const char * const needle, const char * const haystack)
{
	int i = 0;
	int j = 0; //diff#1
	int haystack_length = strlen(haystack);
	int needle_length = strlen(needle);

	while(i < haystack_length) //diff#2
	{
		while(j >= 0 && haystack[i] != needle[j]) //diff#3
			j = b[j];

		i++;
		j++;

		if(j == needle_length)
		{
			printf("Match at %d \n", i - needle_length);
			j = b[j]; //diff#4 Important! 
		}
	}

	return 0;
}
#endif

int main()
{
	int b[MAX_LENGTH];
	int i;
	int needle_length = strlen(needle);
	
	kmp_preprocess(b, needle);
	for(i = 0; i <= needle_length; i++)
	{
		printf("%d\n",b[i]);
	}

	kmp_search(b, needle, haystack);

}

#endif




