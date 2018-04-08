#if 0
#include <stdio.h>
#include <sys/types.h>
//#include <unistd.h>

#define PRINT(x)  printf("%s: %d\n",#x,x); 
#define PRINTH(x) printf("%s: 0x%08x\n",#x, x);
#define PRINTL(t) printf("%s: 0x%016x\n",#t, t);
#define NUMBITS_INT_MINUS1 (sizeof(int)*8 - 1)
#define PRINTBINARY(x, type) \
	do{ int i;\
		for(i = sizeof(type)*8 - 1; i >= 0; i--)\
		putchar('0' + ((x >> i) & 1));\
		printf("b\n"); \
	}while(0)

#define PRINTBINARY_AUTO(x)\
	do{ int i;\
		for(i = sizeof(x)*8 - 1; i >= 0; i--)\
		putchar('0' + ((x >> i) & 1));\
		printf("b\n"); \
	}while(0)
/* dangerous macro: dont pass stuff that modifies itself, such as x++ */
#define PRINTHEX(x) printf("0x %04x,%04x\n",((x) >> 16) & 0x0000FFFF, (x) & 0x0000FFFF);

#define SIGN_OF_INTEGER 	   1
#define DETECT_DIFFERENT_SIGNS 0
#define ABSOLUTE_VALUE_WO_BRANCH 0
#define MINMAX_WO_BRANCH		0
#define ISPOWEROF2				0
#define SIGNEXTEND				0
#define SETRESET_BITS_CONDITIONALLY 0
#define CONDITIONALLY_NEGATE 0
#define MERGE_BITS			 0
#define COUNTING_BITS		 0
#define COUNTING_BITS_TILL_A_POSITION 0
#define COMPUTE_PARITY			0
#define SWAP_BITS 				0
#define REVERSE_BITS			0
#define COUNT_TRAILING_ZEROS 	0
#define ROUND_TO_NEXT_HIGHEST_POWER_OF_2 0
#define IS_BYTE_ZERO			0
#define TEST_PRINT_BINARY_AUTO  0
int main(void)
{
	
#if SIGN_OF_INTEGER // hmm does not make sense
	{
		int x = -2;
		PRINT(-(x < 0)); /* -1 or 0 */
		/* below are compiler dependent */
		PRINT( x>>NUMBITS_INT_MINUS1 ); /* -1, 1 for 0, 0 for +ve */		
		PRINT( (x != 0) | (x>>NUMBITS_INT_MINUS1) ); /* -1 for -ve, 0 for 0, 1 for +ve */		 
	}
#endif	
#if DETECT_DIFFERENT_SIGNS
	{
		int x = 10, y = -3;
		PRINT((x ^ y) < 0) /* 1 if different signs */
	}
#endif
#if ABSOLUTE_VALUE_WO_BRANCH
	{
		int x = -10;
		unsigned int mask = (unsigned int) (x >> NUMBITS_INT_MINUS1);
		PRINT( (x + mask) ^ mask );
		/* TIP: Conditional compliment: XORing x with all ones is equivalent to taking 
		 * one's complement of the number x. XORing with zeros does not change value.
		 * TIP: Adding all ones to x is equivalent to subtracting 1 from x.
		 * TIP: For doing ones complement/bitwise toggle conditioned on the sign of the 
		 * number x, use (x>> NUMBITS_INT_MINUS1) for XORing x. 
		 * y = x + mask is equal to 11. 
		 * ~y = (y ^ allones) = twos complement(y) minus 1 
		 */
	}
#endif	
#if MINMAX_WO_BRANCH
	{
		int x = -10, y = 10;
		printf("Minumum: ");
		PRINT( x ^ ((x ^ y) & (x < y)));		
		printf("Maximum: ");
		PRINT( y ^ ((x ^ y) & (x < y)));
		/* TIP: x^(x^y) = (x^x)^y = (0^y) = y */
		/* TIP: Conditional bit selection from two bits x, y:
		 * x^( (x^y)&f ) = x, if f is 0, y if f is 1. See merging below 
		 */
		
		/* without using XOR and comparison operators */
		printf("Minumum: ");
		PRINT( y + ((x - y) & ((x-y)>>NUMBITS_INT_MINUS1)));
	}	
#endif	
#if ISPOWEROF2
	{
		int x = -8; /* only 1 bit is set in the word */
		printf("Power of two: 1 true, 0 false\n");
		PRINT((x/*for zero*/) && !(x & (x-1))); /* false for -8 */
		
		/* TIP: (x - 1) toggles the rightmost 1 and all the zeros right to it */
		/* TIP: Use x & (x - 1) to clear the rightmost 1 */
	}	
#endif	
#if SIGNEXTEND	
	{
		/* Sign extension is automatic for built-in types, such as chars and ints */
		char x8 = -1;
		int x32;
		PRINTBINARY(x8, char);
		x32 = (int) x8;
		PRINTBINARY(x32, int);		
		
		/* but not for custom lengths */
		int b = 4; /* 4 bit rep. */
		int mask = 1U << (b -1);
		int x4 = 0x09;
		printf("%10s","x4: ");
		PRINTBINARY(x4, int);
		printf("%10s","mask: ");		
		PRINTBINARY(mask, int);
		printf("%10s","x4^mask: ");		
		PRINTBINARY((x4^mask), int);
		printf("%10s", "-mask: ");		
		PRINTBINARY((-mask), int);		
		
		x32 = (x4 ^ mask) - mask; /* this assumes that x4 has zeros
		 in rest of 32 -4 positions */
		PRINT((x4 ^ mask) - mask);
		/* -mask does the 1s extension. These ones are nullfied by adding 
		 * one in the (b-1)th bit position.
		 * (x4^mask) puts '1' in MSB place, if MSB of x4 is 1.
		 */  
		printf("%10s", "x32: ");
		PRINTBINARY(x32, int);
		PRINTHEX(x32);		
		
		/* TIP: to generate (32 - b) 1s followed by (b) zeros, use -mask,
		 * where mask is -(1U << b)
		 */ 
		
		int shift = NUMBITS_INT_MINUS1 - (b - 1); 
		PRINT( (x4 << shift) >> shift);
	}
#endif
	
#if SETRESET_BITS_CONDITIONALLY	
	int f = 1; /* if (f) x |= m (OR with m); else x &= ~m; (AND with ~m)*/ 
	int m = 0xF0F0F0F0;
	int x = 0xABCDABCD;
	
	PRINTHEX(x);
	PRINTHEX(m);
	
	/* long winded */
	PRINT((f && (x |= m)) | ((!f) && (x &= ~m))); /* compiler dependent.. 
	it is not guaranteed that rest of expression is not evaluated if f = 0; */		
	PRINTHEX(x);
	
	x = 0xABCDABCD;
	PRINTH((x & ~m) | (-f & m));
	/* TIP: Conditional masking: (-f & m) is m if f is 1, 0 if f is 0. */
	/* cleared bits in (x & ~m) are again set by ORing with m, if f is 1. */	
#endif
#if CONDITIONALLY_NEGATE
	int fDontNegate = 0; /* negate if fDontNegate = 0; */
	int fNegate = 1 - fDontNegate;
 	int x = 5; /* if (f) result = -x; */
	
	/* using multiplication */
	PRINT( (fDontNegate^(fDontNegate-1)) * x );
	/* TIP: To convert f={0,1} to s={-1, 1}, use s=f^(f-1)= f|(f -1) */
	
	/* Compute twos complement: ones complement + 1 */
	PRINT( (x^(-fNegate)) + fNegate); 	
#endif	
#if MERGE_BITS
	unsigned int a = 0xABCDABCD;    // value to merge in non-masked bits
	unsigned int b = 0x0EF00EF0;    // value to merge in masked bits
	unsigned int mask = 0xFF00FF00; // 1 where bits from b should be selected; 0 where from a.
	// result of (a & ~mask) | (b & mask) goes here
	
	PRINTH(a);
	PRINTH(b);
	PRINTH(mask);
	PRINTH((a & ~mask) | (b & mask)); // 4 operations
	PRINTH( a ^( (a^b)&mask ));	 //3 operations
	/* TIP: Conditionally select a bit in either a or b. Use above expression*/
#endif
#if COUNTING_BITS
	int c, i, bitsSetInChar[256];
	unsigned int x = 0xABCDABCD;
	PRINTBINARY(x, int);
	for(c = 0; x != 0; c++)
	{
		/* clear rightmost 1 */
		x = x & (x - 1); 
	}
	printf("Bit Count %d\n", c);
	
	x = 0xABCDABCD;
	bitsSetInChar[0] = 0;
	for(i = 1; i < 256; i++)
	{
		bitsSetInChar[i] = bitsSetInChar[(i >> 1)] + (i & 1);
	}
	
	PRINT(bitsSetInChar[x & 0xff]
	       + bitsSetInChar[(x>>8) & 0xff]
	       + bitsSetInChar[(x>>16) & 0xff]
	       + bitsSetInChar[(x>>24) /*&& 0xff */]);
	
	unsigned int v = 0xFFFFFFFF, count, t;
	printf("Another way!\n");
	PRINTBINARY(v, int);

	v = (v & 0x55555555) + ((v >> 1) & 0x55555555); //two bits
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333); //four bits = max representation 15
													//however, max sum is 4.
	PRINTH(v);
	
	v = (v & 0x0F0F0F0F) + ((v >> 4) & 0x0F0F0F0F);
	v = (v & 0x00FF00FF) + ((v >> 8) & 0x00FF00FF);
	v = (v & 0x0000FFFF) + ((v >> 16) & 0x0000FFFF);
	
	PRINT(v);
#endif		
#if COUNTING_BITS_TILL_A_POSITION
	
	  __uint64_t v;          // Input value to find position with rank r.
	  unsigned int r = 4;      // Input: bit's desired rank [1-64].
	  unsigned int s;      // Output: Resulting position of bit with rank r [1-64]
	  __uint64_t a, b, c, d; // Intermediate temporaries for bit count.
	  unsigned int t;      // Bit count temporary.
	  
	  v = (__uint64_t) 0xFFFF;
	  printf("sizeof(__uint64_t) %d\n", sizeof(__uint64_t));
	  // a = (v & 0x5555...) + ((v >> 1) & 0x5555...);
	  a =  v - ((v >> 1) & ~0UL/3);
	  // b = (a & 0x3333...) + ((a >> 2) & 0x3333...);
	  b = (a & ~0UL/5) + ((a >> 2) & ~0UL/5);
	  // c = (b & 0x0f0f...) + ((b >> 4) & 0x0f0f...);
	  c = (b + (b >> 4)) & ~0UL/0x11;
	  // d = (c & 0x00ff...) + ((c >> 8) & 0x00ff...);
	  d = (c + (c >> 8)) & ~0UL/0x101;
	  t = (d >> 32) + (d >> 48);
	  
	  PRINTL(d);
	  PRINTL(t);
	  
	  s  = 64;
	  // if (r > t) {s -= 32; r -= t;}
	  s -= ((t - r) & 256) >> 3; r -= (t & ((t - r) >> 8));
	  t  = (d >> (s - 16)) & 0xff;
	  // if (r > t) {s -= 16; r -= t;}
	  s -= ((t - r) & 256) >> 4; r -= (t & ((t - r) >> 8));
	  t  = (c >> (s - 8)) & 0xf;
	  // if (r > t) {s -= 8; r -= t;}
	  s -= ((t - r) & 256) >> 5; r -= (t & ((t - r) >> 8));
	  t  = (b >> (s - 4)) & 0x7;
	  // if (r > t) {s -= 4; r -= t;}
	  s -= ((t - r) & 256) >> 6; r -= (t & ((t - r) >> 8));
	  t  = (a >> (s - 2)) & 0x3;
	  // if (r > t) {s -= 2; r -= t;}
	  s -= ((t - r) & 256) >> 7; r -= (t & ((t - r) >> 8));
	  t  = (v >> (s - 1)) & 0x1;
	  // if (r > t) s--;
	  s -= ((t - r) & 256) >> 8;
	  s = 65 - s;

	  PRINT(s);
#endif	
#if COMPUTE_PARITY
	  unsigned int x = 0xFFFFFFFE;
	  
	  PRINTBINARY(x, int);
	  x = x ^ (x >> 16);
	  x = x ^ (x >> 8);
	  x = x ^ (x >> 4);
#if 0	  
	  x = x ^ (x >> 2);
	  x = x ^ (x >> 1);
	  x = x & 0x1;
#else	  
	  x = x & 0xF; /*just need to compte the parity of last 4 bits */
	  x = 0x6996 >> x; /* miniature parity table for 16 bitsz */
	  x = x & 0x1;
#endif			  
	  PRINT(x);
#endif	  
	  
#if SWAP_BITS
	  
	 unsigned int x = 0xAAAABBBB;
	 unsigned int y = 0xCCCCDDDD;
	 int p1 = 3; //(the fourth bit from the right, and bits left to it)
	 int p2 = 24 - 1;
	 int n = 4;
	 unsigned int temp;
	 
	 x = x ^ y;
	 y = x ^ y; /* (x^y)^y */ 
	 x = x ^ y; /* (x^y)^(x) */
	 
	 PRINTH(x);
	 PRINTH(y);
	 
	 temp = ((x >> p1) ^ (x >> p2)) & ((1U << n) - 1);
	 PRINTH(temp);
	 
	 /*x = (x ^ (temp << p1)) | (x ^ (temp << p2)); Does not work for obvious reasons:
	  * bits at p1 in the expression before | are ORed with the original bits in x again
	  */ 
	 x = x ^ ((temp << p1) | (temp << p2));
	 PRINTH(x);
	 
#endif
#if REVERSE_BITS
	 unsigned char reversedbits[256];
	 int i;
	 
	 unsigned char a = 0xC6;
	 unsigned int x = 0xABCDABCD;
	 
	 reversedbits[0] = 0;
	 for(i = 1; i < 256; i++)
	 {
		 reversedbits[i] = 	((i & 0x1) << 7) | (reversedbits[i >> 1] >> 1);
	 }
	 
	 PRINTBINARY(a, char);
	 a = reversedbits[a];
	 PRINTBINARY(a, char);
	 
	 PRINTBINARY(x, int);
	 x = (reversedbits[x & 0xFF]  << 24)| // put first byte in the Left most byte position 
			 (reversedbits[(x >> 8) & 0xFF]  << 16)|
			 (reversedbits[(x >> 16) & 0xFF] << 8)|
			 (reversedbits[(x >> 24) & 0xFF]);
	 PRINTBINARY(x, int);
	 
	 x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
	 x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
	 x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);
	 x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);
	 x = ((x >> 16) & 0x0000FFFF) | ((x & 0x0000FFFF) << 16);
	 PRINTBINARY(x, int);	 
#endif	 
#if COUNT_TRAILING_ZEROS
	 int x,c, y;
	 
	 c = 32;
	 x = 0xABCD0000;
	 y = -x;
	 
	 PRINTBINARY(x, int);
	 PRINTBINARY(y, int);
	 /* TIP: -x is same as x till (and including) the first 1 that appears 
	  * on the rightmost side. And rest of the bits are complemented.
	  * For e.g.
	  * x = 10101011110011010000000000000000b
	  * -x= 01010100001100110000000000000000b
	  * TIP: (x & -x) has a single bit set and that bit is the rightmost 1 in x.
	  */ 
	 
	 x = (x & -x);
	 
	 //test: 
	 x = 0x80000000;
	 
	 if(x & 0x0000FFFF) c = c - 16; /* that is, 1 is in the lower 2 bytes;
	 so discard upper 16 zeros */
	 if(x & 0x00FF00FF) c = c - 8; /* that is, 1 is in the lower byte of byte pair */
	 if(x & 0x0F0F0F0F) c = c - 4; /* that is, 1 is in the lower byte of byte pair */	 
	 if(x & 0x33333333) c = c - 2; /* 1 is in lower nibble of byte */
	 if(x & 0x55555555) c = c - 1; /* 1 is in lower bit of a couple of bits */

	 if(x) c--; /* adjust count */
	 
	 /* TIP: Above algo for locating the position of 1 can be used to determine
	  * what power of 2 the number is, if the number is a power of 2.
	  * That is, finding the log of the number for base 2, if power of 2. 
	  */ 
	 PRINT(c);
	 
#endif	 
#if ROUND_TO_NEXT_HIGHEST_POWER_OF_2
	 {
		 unsigned int v = 0x00007012;
		 /* It works by copying the highest set bit to all of the lower bits,
		  * and then adding one, which results in carries that set all of the
		  * lower bits to 0 and one bit beyond the highest set bit to 1.
		  * If the original number was a power of 2, then the decrement will reduce 
		  * it to one less, so that we round up to the same original value.
		  */  		 
		 v--;
		 PRINTBINARY(v, int);
		 v |= v >> 1;
		 PRINTBINARY(v, int);		 
		 v |= v >> 2;	
		 PRINTBINARY(v, int);
		 v |= v >> 4;
		 PRINTBINARY(v, int);		 
		 v |= v >> 8;
		 PRINTBINARY(v, int);		 
		 v |= v >> 16;
		 PRINTBINARY(v, int);		 
		 v++;
		 PRINTBINARY(v, int);
		 PRINTHEX(v);
		 
		 /* TIP: above repeated ORing is useful in setting all bits right to the 
		  * leftmost 1. To set the bits left to the leftmost 1, use (-x | x)
		  */
		 
	 }
#endif
#if IS_BYTE_ZERO
	 {
		unsigned int x = 0xFFFF00FF;
		int haszero, hasNoZero;
		/* straightforward way  1 */
		unsigned char * p = (unsigned char *) &x;
		hasNoZero = (*(p)) && (*(p + 1)) && (*(p + 2)) && (*(p + 3));		
		PRINT(hasNoZero);
		/* straightforward way 2 */		
		hasNoZero = (x & 0xFF) && (x & 0xFF00) && (x & 0xFF0000) && (x & 0xFF000000); 
		PRINT(hasNoZero);

		/* terrific way */
#define	HASZERO(v) (((v) - 0x01010101UL) & ~(v) & 0x80808080UL)		
		/* The subexpression (v - 0x01010101UL), evaluates to a high bit set
		 * in any byte whenever the corresponding byte in v is zero or greater
		 *  than 0x80. The sub-expression ~v & 0x80808080UL evaluates to high bits 
		 *  set in bytes where the byte of v doesn't have its high bit set 
		 *  (so the byte was less than 0x80). Finally, by ANDing these two sub-expressions 
		 *  the result is the high bits set where the bytes in v were zero, 
		 *  since the high bits set due to a value greater than 0x80 in the first sub-expression 
		 *  are masked off by the second.
		 */  
	 }	 
#endif	 
#if IS_BYTE_LESS_THAN_NUMBER
	 {
		 /* Test if a word x contains an *unsigned* byte with value < n. */
		 PRINTHEX((~0UL/255)); /*TIP: 0x01010101 = (~0UL/255) */
#define	HASLESS(v,n) (((v) - 0x01010101UL*(n)) & ~(v) & 0x80808080UL)		
		 /* The idea: byte(v) - n will have high bit set if byte(v) is less than
		  * n. 
		  * Case 1: if byte(v) has high bit set. And n is greater than it (so n has also high bit set).
		  * Then byte(v) - n results in high bit set.
		  * Case 2: if byte(v) has high bit set. And n is less than it.
		  * Then byte(v) - n may have high bit set or reset.
		  * Case 3: if byte(v) has high bit zero. And n is greater than it (may have high bit set).
		  * Then byte(v) - n results in high bit set.
		  * Case 4: if byte(v) has high bit zero. And n is less than it.
		  * Then byte(v) - n results in high bit zero.
		  */
		 /* So i dont understand how this works given Case 2 :( */		 
	 }	 
#endif	 
#if TEST_PRINT_BINARY_AUTO 
	 {
		char x8 = -1;
		int x32;
		PRINTBINARY_AUTO(x8);
		x32 = (int) x8;
		PRINTBINARY_AUTO(x32);	
	 }
#endif
	return 0;
}
#endif
