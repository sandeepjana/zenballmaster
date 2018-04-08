#if 0
/* 
When decided to use dynamic programming, ask the following questions:
+ what is the recurrence relation?
	+ how many degrees of freedom the sub-problem has? In other words, what is the dimension of the table?
+ what are the boundary conditions? That is, for each axis, what is the smallest possible index in the table?
+ Make sure the sub-problem is operating on a smaller set 

Problem: Integer partition WITHOUT rearrangement
Partition S = {0 <= i < N : s[i]} and s[i] >= 0, into a 'k' or fewer partitions (or ranges) to
minimize the "maximum sum within a range" over all ranges.

Q. Wait, how fewer than k partitions can be better? The maximum sum will be lesser there are more partitions, isnt it?

k partitions => (k - 1) dividers to be placed.

Recurrence relation:
Let C[k] be the cost criterion (maximum sum within a range) when the k-th partition is formed
The position of k-th partition is 0 < i < N.
C[k] = max {sum of elements in this just formed k-th partition, largest cost in the rest of (k - 1) partitions }
The second term above is C[k - 1], the largest cost among the partitions left to k-th partition.

C[k, N] = min (over i) max { s[i] + .. + s[N - 1], C[k - 1, i] } where i is the k-th partition location (divider is in between i - 1 and i)

So it seems the table should be formed with k and i as the axes.
base condition: C[1, 1], C[1, 2] .. are simply prefix sum of first few elements.
C[2, 1] and c[3, 1] etc are defined to be s[0] itself (because you cannot do 2 partitions of 1 element)
C[k + 1, k], C[k + 2, k] etc are defined to be C[k, k] itself (same reasoning as above)
The sub problem, therefore, has a smaller set.

*/

#include <stdio.h>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <numeric>

typedef std::vector<std::vector<int> > vec2d;
void print_table(const vec2d &table)
{
	auto end = table.end();
	for(auto& it = table.begin(); it != end; ++it)
	{
		std::copy(it->begin(), it->end(), std::ostream_iterator<int>(std::cout, "\t"));
		printf("\n");
	}
}

void print_partition(const int* a, vec2d& parents)
{
	int numEle = parents[0].size(); 
	int numPart = parents.size();
	std::vector<int> n;
	
	int pos = numEle;
	for(int p = numPart; p > 0; --p)
	{
		int eleInLeftPartitions = parents[p - 1][pos - 1];
		std::cout << "Best way to partition " << pos << " elements " << 
			"into " << p << " partitions is to keep " << eleInLeftPartitions << " in left partitions. ";
		std::cout << "So next look at how to divide these " << eleInLeftPartitions << " into " << (p - 1) << " partitions \n"; 
 		pos = eleInLeftPartitions;
		n.push_back(eleInLeftPartitions);
	}

	std::reverse(n.begin(), n.end());
	std::adjacent_difference(n.begin(), n.end(), n.begin());
	std::cout << "\nThe number of elements in each partition: ";
	std::copy(n.begin() + 1, n.end(), std::ostream_iterator<int>(std::cout, "\t"));
	std::cout << "\n";
}

void fair_partition_without_rearrangement(const int* a, int numEle, int numPart)
{
	vec2d table(numPart, std::vector<int>(numEle));
	vec2d parents(numPart, std::vector<int>(numEle));
	std::vector<int> prefixSum(numEle);
	std::partial_sum(a, a + numEle, prefixSum.begin());

	//set base conditions
	for(int p = 1; p <= numPart; ++p)
	{
		table[p - 1][0] = a[0];
	}

	std::copy(prefixSum.begin(), prefixSum.end(), table[0].begin());

	for(int n = 2; n <= numEle; ++n)
	{
		for(int p = 2; p <= numPart; ++p)
		{
			int minCost = INT_MAX;
			int elementsOfLeftPartitions = 0;
			//i is the number of elements in all but the current (last) partition
			for(int i = 1; i <= (n - 1); ++i)
			{
				//table[p - 1 - 1] picks the one row above (one partition less)
				int cost = std::max(table[p - 1 - 1][i - 1], prefixSum[n - 1] - prefixSum[i - 1]);
				if(cost < minCost)
				{
					minCost = cost;
					elementsOfLeftPartitions = i;
				}
			}

			table[p - 1][n - 1] = minCost;
			parents[p - 1][n - 1] = elementsOfLeftPartitions;
		}
	}

	print_table(table);
	print_table(parents);

	print_partition(a, parents);
}
/*
numbers: {1, 2, 3, 4, 5, 6, 7, 8, 9}
table:
elements ->
1       3       6       10      15      21      28      36      45
1       2       3       6       9       11      15      21      24
1       2       3       4       6       9       11      15      17
e.g., C[2nd row][5th col] = min{max{1, 15 - 1}, max{3, 15 - (1 + 2)},
	max{6, 15 - (1 + 2 + 3)}, max{10, 15 - (1 + 2 + 3 + 4)}}
	= min{14, 12, 9, 10} = 9
where 15 above is (1 + 2 + 3 + 4 + 5)
*/

int main(int , char**)
{
	const int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	fair_partition_without_rearrangement(arr, 9, 3);

	return 0;
}


#endif