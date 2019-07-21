#pragma once

#ifndef FIXED_MATRIX_HPP
#define FIXED_MATRIX_HPP

#include <stdexcept>
#include <algorithm>

struct fixed_matrix_error : std::runtime_error {
	explicit fixed_matrix_error(const char* q) : std::runtime_error(q) {}
	explicit fixed_matrix_error(const std::string& n) : std::runtime_error(n) {}
};

template<typename T, const std::size_t RowsCount, const std::size_t ColumnsCount>
struct fixed_matrix
{
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;

	using iterator = T*;
	using const_iterator = const T*;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverce_iterator = std::reverse_iterator<const_iterator>;

	enum { LINEAR_SIZE = RowsCount * ColumnsCount };

	constexpr explicit fixed_matrix() noexcept 
		: elems_{} 
	{}
	explicit fixed_matrix(const T& val) { std::fill(begin(), end(), val); }

	explicit fixed_matrix(const T(&arr)[LINEAR_SIZE]) { std::copy(std::cbegin(arr), std::cend(arr), begin()); }

	explicit fixed_matrix(const T(&arr)[RowsCount][ColumnsCount])
	{
		std::copy(reinterpret_cast<const T*>(arr), reinterpret_cast<const T*>(arr) + LINEAR_SIZE, begin());
	}

	explicit fixed_matrix(std::initializer_list<T> init_lst)
	{
		if (std::size(init_lst) != LINEAR_SIZE)
			throw fixed_matrix_error{ "Invalid argument for constructor fixed_matrix<T>::fixed_matrix(std::initializer_list<T>)" };

		std::copy(std::cbegin(init_lst), std::cend(init_lst), begin());
	}

	fixed_matrix(const fixed_matrix& other) { std::copy(std::cbegin(other), std::cend(other), begin()); }
	fixed_matrix& operator =(const fixed_matrix& other)
	{
		if (this == &other)
			return *this;

		std::copy(std::cbegin(other), std::cend(other), begin());
	}

	fixed_matrix(fixed_matrix&&) = delete;
	fixed_matrix& operator=(fixed_matrix&&) = delete;

	constexpr inline size_type count_rows() const noexcept { return RowsCount; }
	constexpr inline size_type count_columns() const noexcept { return ColumnsCount; }
	constexpr inline size_type count_elements() const noexcept { return LINEAR_SIZE; }

	constexpr inline pointer data() noexcept { return reinterpret_cast<T*>(elems_); }
	constexpr inline const_pointer data() const noexcept { return reinterpret_cast<const T*>(elems_); }

	constexpr inline void swap(fixed_matrix& other) { std::swap_ranges(begin(), end(), std::begin(other)); }

	constexpr inline const T& operator ()(const size_type row_index, const size_type col_index) const
	{
		range_check(row_index, col_index);
		return elems_[row_index][col_index];
	}
	constexpr inline T& operator ()(const size_type row_index, const size_type col_index)
	{
		range_check(row_index, col_index);
		return elems_[row_index][col_index];
	}

	constexpr inline const T* operator [](size_type row_index) const noexcept { return elems_[row_index]; }
	constexpr inline T* operator [](size_type row_index) noexcept { return elems_[row_index]; }

	constexpr inline iterator begin() noexcept { return reinterpret_cast<T*>(elems_); }
	constexpr inline iterator end() noexcept { return reinterpret_cast<T*>(elems_) + LINEAR_SIZE; }

	constexpr inline const_iterator cbegin() const noexcept { return reinterpret_cast<const T*>(elems_); }
	constexpr inline const_iterator cend() const noexcept { return reinterpret_cast<const T*>(elems_) + LINEAR_SIZE; }

	constexpr inline const_iterator begin() const noexcept { return cbegin(); }
	constexpr inline const_iterator end() const noexcept { return cend(); }


	constexpr inline reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
	constexpr inline reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

	constexpr inline const_reverce_iterator crbegin() const noexcept { return const_reverce_iterator(cend()); }
	constexpr inline const_reverce_iterator crend() const noexcept { return const_reverce_iterator(cbegin());}

	constexpr inline const_reverce_iterator rbegin() const noexcept { return crbegin(); }
	constexpr inline const_reverce_iterator rend() const noexcept { return crend(); }

private:
	constexpr inline void range_check(size_type row_index, size_type col_index) const
	{
		if (row_index >= RowsCount)
			throw fixed_matrix_error{ "range error: row_index is out of range" };
		if (col_index >= ColumnsCount)
			throw fixed_matrix_error{ "range error: col_index is out of range" };
	}

private:
	T elems_[RowsCount][ColumnsCount];
};

#endif // !FIXED_MATRIX_HPP
