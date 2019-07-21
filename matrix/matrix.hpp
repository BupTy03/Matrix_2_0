#pragma once
#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <type_traits>
#include <memory>
#include <scoped_allocator>
#include <stdexcept>
#include <initializer_list>
#include <algorithm>
#include <iterator>
#include <vector>
#include <cassert>

template<class T, class A>
struct matrix_base
{
	using allocator_type = typename std::allocator_traits<A>::template rebind_alloc<T>;
	using row_allocator = typename std::allocator_traits<A>::template rebind_alloc<T*>;
	using size_type = std::size_t;

	explicit matrix_base(const allocator_type& al) {}
	explicit matrix_base(const size_type rows, const size_type columns, const T& value, const allocator_type& al)
		: count_rows_{ rows }
		, count_columns_{ columns }
		, space_rows_{ rows }
		, space_columns_{ columns }
		, alloc_{ row_allocator(), al }
	{
		if (count_rows_ == 0) return;
		elem_ = alloc_.allocate(count_rows_);

		if (count_columns_ == 0) return;
		elem_ = allocate_matrix();
	}

	~matrix_base() { deallocate_matrix(); }

	allocator_type& get_allocator() { return alloc_.inner_allocator(); }

	void swap(matrix_base& other)
	{
		std::swap(alloc_, other.alloc_);
		std::swap(elem_, other.elem_);
		std::swap(count_rows_, other.count_rows_);
		std::swap(count_columns_, other.count_columns_);
		std::swap(space_rows_, other.space_rows_);
		std::swap(space_columns_, other.space_columns_);
	}

protected:
	inline T* allocate_row()
	{
		return (alloc_.inner_allocator()).allocate(space_columns_);
	}
	inline void deallocate_row(const size_type row)
	{
		assert(elem_ != nullptr);
		assert(row < count_rows_);
		return (alloc_.inner_allocator()).deallocate(elem_[row], space_columns_);
	}

	inline T** allocate_matrix()
	{
		auto result = alloc_.allocate(space_rows_);
		for (size_type row = 0; row < space_rows_; ++row) {
			result[row] = allocate_row();
		}
		return result;
	}
	inline void deallocate_matrix()
	{
		assert(elem_ != nullptr);
		for (size_type row = 0; row < space_rows_; ++row) {
			deallocate_row(row);
		}
		alloc_.deallocate(elem_, space_rows_);
	}

	inline void destroy_row(const size_type row)
	{
		assert(elem_ != nullptr);
		assert(row < count_rows_);
		for (size_type column = 0; column < count_columns_; ++column) {
			destroy(&elem_[row][column]);
		}
	}

	inline void destroy_matrix()
	{
		for (size_type row = 0; row < count_rows_; ++row) {
			destroy_row(row);
		}
	}

	inline void destroy_and_deallocate_row(const size_type row)
	{
		destroy_row(row);
		deallocate_row(row);
	}

	inline void destroy_and_deallocate_matrix()
	{
		for (size_type row = 0; row < count_rows_; ++row) {
			destroy_and_deallocate_row(row);
		}
	}

	template<class... Args>
	static inline void construct(T* ptr, Args&&... args) { ::new(static_cast<void*>(ptr)) T(std::forward<Args>(args)...); }
	static inline void destroy(T* ptr) noexcept { ptr->~T(); }

	inline void check_row_index(const size_type index) const
	{
		if (index >= count_rows_) {
			throw std::out_of_range{ "Row index is out of range" };
		}
	}

	inline void check_column_index(const size_type index) const
	{
		if (index >= count_columns_) {
			throw std::out_of_range{ "Column index is out of range" };
		}
	}

	inline void check_indexes(const size_type row_index, const size_type column_index) const
	{
		check_row_index(row_index);
		check_column_index(column_index);
	}

protected:
	T** elem_{ nullptr };

	size_type count_rows_{ 0 };
	size_type count_columns_{ 0 };

	size_type space_rows_{ 0 };
	size_type space_columns_{ 0 };

	std::scoped_allocator_adaptor<row_allocator, allocator_type> alloc_;
};

template<typename T, typename A = std::allocator<T>>
struct matrix : public matrix_base<T, A>
{
private:
	struct MstrixIteratorTag{};
	struct ConstMatrixIteratorTag{};

public:
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = T & ;
	using const_reference = const T&;
	using pointer = T * ;
	using const_pointer = const T*;

	template<class MatrixIteratorTag>
	struct MatrixIterator;

	using iterator = MatrixIterator<MstrixIteratorTag>;
	using const_iterator = MatrixIterator<ConstMatrixIteratorTag>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverce_iterator = std::reverse_iterator<const_iterator>;

	using base = matrix_base<T, A>;
	using allocator_type = typename base::allocator_type;

	inline iterator begin() noexcept { return { this->elem_, this->count_columns_ }; }
	inline iterator end() noexcept { return { (this->elem_ + this->count_rows_), this->count_columns_ }; }

	inline const_iterator cbegin() const noexcept { return { this->elem_, 0, this->count_columns_ }; }
	inline const_iterator cend() const noexcept { return { (this->elem_ + this->count_rows_), this->count_columns_ }; }

	inline const_iterator begin() const noexcept { return cbegin(); }
	inline const_iterator end() const noexcept { return cend(); }


	inline reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
	inline reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

	inline const_reverce_iterator crbegin() const noexcept { return const_reverce_iterator(cend()); }
	inline const_reverce_iterator crend() const noexcept { return const_reverce_iterator(cbegin()); }

	inline const_reverce_iterator rbegin() const noexcept { return crbegin(); }
	inline const_reverce_iterator rend() const noexcept { return crend(); }

	explicit matrix(const allocator_type& alloc = allocator_type{}) 
		: base { alloc }
	{  }

	explicit matrix(
		const size_type rows,
		const size_type columns
	) : base{ rows, columns, T{}, allocator_type{} }
	{
		std::uninitialized_fill(begin(), end(), T{});
	}

	explicit matrix(
		const size_type rows, 
		const size_type columns, 
		const T& value,
		const allocator_type& alloc = allocator_type{}
	) : base{ rows, columns, value, alloc }
	{
		std::uninitialized_fill(begin(), end(), value);
	}

	~matrix() { destroy_all(); }


	inline size_type count_rows() const noexcept { return this->count_rows_; }
	inline size_type count_columns() const noexcept { return this->count_columns_; }

	inline size_type space_rows() const noexcept { return this->space_rows_; }
	inline size_type space_columns() const noexcept { return this->space_columns_; }

	T& operator()(const size_type row, const size_type column)
	{
		this->check_indexes(row, column);
		return this->elem_[row][column];
	}
	const T& operator()(const size_type row, const size_type column) const
	{
		this->check_indexes(row, column);
		return this->elem_[row][column];
	}

private:
	inline void destroy_all() noexcept
	{
		for (size_type row = 0; row < this->count_rows_; ++row) {
			for (size_type column = 0; column < this->count_columns_; ++column) {
				base::destroy(&(this->elem_[row][column]));
			}
		}
	}
};

template<class T, class A>
template<class MatrixIteratorTag>
struct matrix<T, A>::MatrixIterator : public std::iterator<std::random_access_iterator_tag, T, ptrdiff_t, T*, const T&>
{
	using mtx_t = matrix<T, A>;
	friend class mtx_t;

	using value_type = T;
	using size_type = mtx_t::size_type;
	using difference_type = mtx_t::size_type;
	using reference = std::conditional_t<std::is_same_v<MatrixIteratorTag, mtx_t::ConstMatrixIteratorTag>, const T&, T&>;
	using pointer = std::conditional_t<std::is_same_v<MatrixIteratorTag, mtx_t::ConstMatrixIteratorTag>, const T*, T*>;
private:
	MatrixIterator(T** ptr, const size_type columnsCount) noexcept
		: ptr_{ ptr }
		, maxColumnIndex_{ columnsCount - 1 }
	{}

public:
	MatrixIterator() noexcept = default;
	MatrixIterator(const MatrixIterator& other) noexcept
		: ptr_{ other.ptr_ }
		, maxColumnIndex_{ other.maxColumnIndex_ }
	{}
	MatrixIterator& operator=(const MatrixIterator& other)
	{
		if (this == &other) {
			return *this;
		}
		ptr_ = other.ptr_;
		currentColumn_ = other.currentColumn_;
		maxColumnIndex_ = other.maxColumnIndex_;
	}

	inline bool operator==(MatrixIterator const& other) const noexcept { return ptr_ == other.ptr_ && currentColumn_ == other.currentColumn_; }
	inline bool operator!=(MatrixIterator const& other) const noexcept { return !(*this == other); }

	inline bool operator<(MatrixIterator const& other) const noexcept { return ptr_ < other.ptr_ || (ptr_ == other.ptr_ && currentColumn_ < other.currentColumn_); }
	inline bool operator>(MatrixIterator const& other) const noexcept { return ptr_ > other.ptr_ || (ptr_ == other.ptr_ && currentColumn_ > other.currentColumn_); }

	inline bool operator<=(MatrixIterator const& other) const noexcept { return !(*this > other); }
	inline bool operator>=(MatrixIterator const& other) const noexcept { return !(*this < other); }

	inline reference operator*() const noexcept { return *((*ptr_) + currentColumn_); }
	inline pointer operator->() const noexcept { return (*ptr_) + currentColumn_; }

	inline MatrixIterator& operator++() noexcept
	{
		if (currentColumn_ == maxColumnIndex_) {
			++ptr_;
			currentColumn_ = 0;
		}
		else {
			++currentColumn_;
		}
		return *this;
	}
	inline MatrixIterator& operator++(int) noexcept
	{
		auto tmp = *this;
		this->operator++();
		return tmp;
	}

	inline MatrixIterator& operator--()
	{
		if (currentColumn_ == 0) {
			--ptr_;
			currentColumn_ = maxColumnIndex_;
		}
		else {
			--currentColumn_;
		}
		return *this;
	}
	inline MatrixIterator& operator--(int)
	{
		auto tmp = *this;
		this->operator--();
		return tmp;
	}

	inline difference_type operator-(const MatrixIterator& other) { return ptr_ - other.ptr_; }

	inline MatrixIterator operator+(const size_type indx) { return { ptr_ + indx, maxColumnIndex_ }; }
	inline MatrixIterator operator-(const size_type indx) { return { ptr_ - indx, maxColumnIndex_ }; }
private:
	T** ptr_{ nullptr };
	size_type currentColumn_{ 0 };
	size_type maxColumnIndex_{ 0 };
};



#endif // MATRIX_HPP