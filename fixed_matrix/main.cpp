#include "fixed_matrix.hpp"

#include <iostream>

template<class T, const std::size_t RowsCount, const std::size_t ColumnsCount>
std::ostream& operator <<(std::ostream& os, const fixed_matrix<T, RowsCount, ColumnsCount>& m)
{
	os << "{ ";
	for (std::size_t i = 0; i < RowsCount; ++i)
	{
		os << "{ ";
		for (std::size_t j = 0; j < ColumnsCount; ++j)
		{
			os << m(i, j);

			if (j != ColumnsCount - 1)
				os << ", ";
		}
		os << " }";

		if (i != RowsCount - 1)
			os << ", \n  ";
	}
	os << " }";

	return os;
}

int main()
{
	fixed_matrix<std::string, 3, 3> mtx{ "A", "B", "C", "D", "E", "F", "G", "H", "I" };
	std::cout << mtx << std::endl;

	fixed_matrix<std::string, 6, 9> mtx2("A");
	std::cout << mtx2 << std::endl;

	system("PAUSE");
	return 0;
}