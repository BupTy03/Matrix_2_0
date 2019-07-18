#include "pch.h"
#include "../fixed_matrix/fixed_matrix.hpp"

#include <algorithm>
#include <random>
#include <tuple>

TEST(FixedMatrixConstruction, DefaultConstructor) {
	constexpr int rowsCount = 3;
	constexpr int columnsCount = 4;
	constexpr int defaultValue = 0;

	fixed_matrix<int, rowsCount, columnsCount> mtx;

	EXPECT_EQ(mtx.count_rows(), rowsCount);
	EXPECT_EQ(mtx.count_columns(), columnsCount);
	EXPECT_TRUE(std::all_of(std::cbegin(mtx), std::cend(mtx), [defaultValue](auto val) { return val == defaultValue; }));
}

TEST(FixedMatrixConstruction, ConstructorWithValueToFill) {
	constexpr int rowsCount = 3;
	constexpr int columnsCount = 4;
	constexpr short defaultValue = 10;

	fixed_matrix<short, rowsCount, columnsCount> mtx(defaultValue);

	EXPECT_TRUE(std::all_of(std::cbegin(mtx), std::cend(mtx), [defaultValue](auto val) { return val == defaultValue; }));
}

TEST(FixedMatrixConstruction, ConstructorWithOneDimentionalArray) {
	constexpr int rowsCount = 3;
	constexpr int columnsCount = 3;
	constexpr char defaultValue = 'A';

	char arr[rowsCount*columnsCount] { defaultValue };
	fixed_matrix<char, rowsCount, columnsCount> mtx(arr);

	decltype(mtx)::const_iterator it;
	std::tie(it, std::ignore) = std::mismatch(std::cbegin(mtx), std::cend(mtx), reinterpret_cast<char*>(arr));
	EXPECT_TRUE(std::cend(mtx) == it);
}

TEST(FixedMatrixConstruction, ConstructorWithTwoDimentionalArray) {
	constexpr int rowsCount = 3;
	constexpr int columnsCount = 3;
	constexpr char defaultValue = 'A';

	char arr[rowsCount][columnsCount]{ defaultValue };
	fixed_matrix<char, rowsCount, columnsCount> mtx(arr);

	decltype(mtx)::const_iterator it;
	std::tie(it, std::ignore) = std::mismatch(std::cbegin(mtx), std::cend(mtx), reinterpret_cast<char*>(arr));
	EXPECT_TRUE(std::cend(mtx) == it);
}

TEST(FixedMatrixConstruction, ConstructorWithInitialisationList) {
	constexpr int rowsCount = 3;
	constexpr int columnsCount = 3;
	const auto init_lst = { 1, 3, 5, 1, 2, 9, 6, 4, 7 };

	fixed_matrix<int, rowsCount, columnsCount> mtx(init_lst);
	decltype(mtx)::const_iterator it;
	std::tie(it, std::ignore) = std::mismatch(std::cbegin(mtx), std::cend(mtx), std::cbegin(init_lst));
	EXPECT_TRUE(std::cend(mtx) == it);


	try {
		fixed_matrix<int, rowsCount, columnsCount> mtx({ 1, 3, 4, 6, 4, 8, 9, 0, 2, 3, 4, 5, 6 });
	}
	catch (const fixed_matrix_error&) {
		SUCCEED();
		return;
	}
	FAIL();
}

TEST(FixedMatrixConstruction, CopyConstructor) {
	constexpr int rowsCount = 3;
	constexpr int columnsCount = 5;

	std::random_device rd;
	std::mt19937 g(rd());
	std::uniform_int_distribution<> int_dist(0, 200);
	const auto gen = [&g, &int_dist]() { return int_dist(g); };

	fixed_matrix<long, rowsCount, columnsCount> mtx;
	std::generate(std::begin(mtx), std::end(mtx), gen);

	auto cloned_mtx = mtx;
	decltype(mtx)::const_iterator it;
	std::tie(it, std::ignore) = std::mismatch(std::cbegin(mtx), std::cend(mtx), std::cbegin(cloned_mtx));
	EXPECT_TRUE(std::cend(mtx) == it);
}

TEST(FixedMatrixUsage, CopyAssignmentOperator) {
	constexpr int rowsCount = 3;
	constexpr int columnsCount = 5;

	std::random_device rd;
	std::mt19937 g(rd());
	std::uniform_int_distribution<> int_dist(0, 200);
	const auto gen = [&g, &int_dist]() { return int_dist(g); };

	fixed_matrix<long, rowsCount, columnsCount> mtx;
	std::generate(std::begin(mtx), std::end(mtx), gen);

	decltype(mtx) cloned_mtx;
	cloned_mtx = mtx;

	decltype(mtx)::const_iterator it;
	std::tie(it, std::ignore) = std::mismatch(std::cbegin(mtx), std::cend(mtx), std::cbegin(cloned_mtx));
	EXPECT_TRUE(std::cend(mtx) == it);
}

TEST(FixedMatrixUsage, OperationsWithSize) {
	constexpr int rowsCount = 4;
	constexpr int columnsCount = 5;

	fixed_matrix<int, rowsCount, columnsCount> mtx;
	EXPECT_EQ(mtx.count_rows(), rowsCount);
	EXPECT_EQ(mtx.count_columns(), columnsCount);
	EXPECT_EQ(mtx.count_elements(), rowsCount*columnsCount);
}

TEST(FixedMatrixUsage, OperationsWithData) {
	constexpr int rowsCount = 4;
	constexpr int columnsCount = 5;

	fixed_matrix<int, rowsCount, columnsCount> mtx;
	EXPECT_EQ(&mtx(0, 0), mtx.data());
}

TEST(FixedMatrixUsage, SwapFunction) {
	constexpr int rowsCount = 4;
	constexpr int columnsCount = 5;

	fixed_matrix<int, rowsCount, columnsCount> mtx;
	const decltype(mtx) mtx_clone = mtx;

	fixed_matrix<int, rowsCount, columnsCount> other_mtx;
	const decltype(mtx) other_mtx_clone = other_mtx;

	mtx.swap(other_mtx);

	decltype(mtx)::const_iterator it;
	std::tie(it, std::ignore) = std::mismatch(std::cbegin(mtx), std::cend(mtx), std::cbegin(other_mtx_clone));
	EXPECT_TRUE(std::cend(mtx) == it);

	decltype(other_mtx)::const_iterator other_it;
	std::tie(other_it, std::ignore) = std::mismatch(std::cbegin(other_mtx), std::cend(other_mtx), std::cbegin(mtx_clone));
	EXPECT_TRUE(std::cend(other_mtx) == other_it);
}

TEST(FixedMatrixUsage, Indexing) {
	constexpr int rowsCount = 4;
	constexpr int columnsCount = 5;

	std::random_device rd;
	std::mt19937 g(rd());
	std::uniform_int_distribution<> int_dist(-500, 500);
	const auto gen = [&g, &int_dist]() { return int_dist(g); };

	fixed_matrix<int, rowsCount, columnsCount> mtx;
	std::generate(std::begin(mtx), std::end(mtx), gen);

	EXPECT_EQ(mtx(0, 0), mtx[0][0]);
	EXPECT_EQ(mtx(2, 3), mtx[2][3]);
	EXPECT_EQ(mtx(3, 1), mtx[3][1]);
	EXPECT_EQ(mtx(3, 4), mtx[3][4]);

	auto it = std::cbegin(mtx);
	for (int row = 0; row < mtx.count_rows(); ++row) {
		for (int col = 0; col < mtx.count_columns(); ++col) {
			EXPECT_EQ(mtx[row][col], mtx(row, col));
			EXPECT_EQ(mtx(row, col), *it);
			++it;
		}
	}

	EXPECT_THROW(mtx(-1, 2), fixed_matrix_error);
	EXPECT_THROW(mtx(mtx.count_rows(), 2), fixed_matrix_error);
}

TEST(FixedMatrixUsage, OperationsWithIterators) {
	constexpr int rowsCount = 4;
	constexpr int columnsCount = 5;

	std::random_device rd;
	std::mt19937 g(rd());
	std::uniform_int_distribution<> int_dist(-500, 500);
	const auto gen = [&g, &int_dist]() { return int_dist(g); };

	fixed_matrix<int, rowsCount, columnsCount> mtx;
	std::generate(std::begin(mtx), std::end(mtx), gen);

	decltype(mtx) reversed_mtx;
	std::copy(std::crbegin(mtx), std::crend(mtx), std::begin(reversed_mtx));

	decltype(mtx)::const_iterator it;
	std::tie(it, std::ignore) = std::mismatch(std::cbegin(mtx), std::cend(mtx), std::crbegin(reversed_mtx));
	EXPECT_TRUE(std::cend(mtx) == it);
}