#include "HeapSort.hpp"
#include <iostream>

using sort_result = HeapSortResult<int, 1, 14, 7, 29, 0, 268, 52, 42, 11, 2, 9, 99, 108, 29, 3, 78, 43, 500>;

template<typename NumSeq>
struct printseq;

template<typename data_t, data_t... nums>
struct printseq<_NumSeq<data_t, nums...>>
{
	constexpr static data_t result[] = { nums... };
	void static print()
	{
		for (auto n : result)
		{
			std::cout << n << ' ';
		}
		std::cout << std::endl;
	}
};

int main()
{
	printseq<sort_result>::print();

	return 0;
}