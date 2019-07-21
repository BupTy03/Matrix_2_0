#include "matrix.hpp"
#include <iostream>
#include <string>

template<class T>
std::ostream& operator <<(std::ostream& os, const matrix<T>& m)
{
	os << "{ ";
	for (std::size_t i = 0; i < m.count_rows(); ++i)
	{
		os << "{ ";
		for (std::size_t j = 0; j < m.count_columns(); ++j)
		{
			os << m(i, j);

			if (j != m.count_columns() - 1)
				os << ", ";
		}
		os << " }";

		if (i != m.count_rows() - 1)
			os << ", \n  ";
	}
	os << " }";

	return os;
}

int main()
{
	matrix<std::string> mtx(3, 4, "matrix class works");
	std::cout << mtx << std::endl << std::endl;

	for (auto i : mtx) {
		std::cout << i << std::endl;
	}

	system("PAUSE");
	return 0;
}