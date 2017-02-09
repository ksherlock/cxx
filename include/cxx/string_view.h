#ifndef cxx_string_view_h
#define cxx_string_view_h

#include <string>
#include <stdexcept>
#include <iterator>

template< 
    class CharT, 
    class Traits = std::char_traits<CharT> 
> class basic_string_view {
	public:

	typedef Traits traits_type;
	typedef CharT value_type;
	typedef CharT* pointer;
	typedef const CharT* const_pointer;
	typedef CharT& reference;
	typedef const CharT& const_reference;

	typedef const_pointer const_iterator;
	typedef const_iterator iterator;

	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef const_reverse_iterator reverse_iterator;

	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	static constexpr size_type npos = size_type(-1);

	/* constructors */
	constexpr basic_string_view() = default;
	constexpr basic_string_view(const basic_string_view& other) = default;

	constexpr basic_string_view(const CharT* s, size_type count) : _begin(s), _end(s + count)
	{}

	template<class Allocator>
	basic_string_view(const std::basic_string<CharT, Traits, Allocator>& str) : basic_string_view(str.data(), str.size())
	{}

	constexpr basic_string_view(const CharT* s) : basic_string_view(s, Traits::length(s))
	{}

	/* iterators */

	constexpr const_iterator begin() const { return _begin; }
	constexpr const_iterator cbegin() const { return _begin; }

	constexpr const_iterator end() const { return _end; }
	constexpr const_iterator cend() const { return _end; }



	constexpr const_reverse_iterator rbegin() const {
		return const_reverse_iterator(begin());
	}

	constexpr const_reverse_iterator crbegin() const {
		return const_reverse_iterator(cbegin());		
	}

	constexpr const_reverse_iterator rend() const {
		return const_reverse_iterator(end());
	}
	constexpr const_reverse_iterator crend() const {
		return const_reverse_iterator(cend());
	}


	constexpr const_reference operator[](size_type pos) const { return _begin[pos]; }
	/*constexpr*/ const_reference at(size_type pos) const {
		if (pos >= length()) throw std::out_of_range("basic_string_view::at");
		return _begin[pos];
	}
	constexpr const_reference front() const {
		return *_begin;
	}
	constexpr const_reference back() const {
		return *(_end - 1);
	}
	constexpr const_pointer data() const {
		return _begin;
	}

	constexpr size_type size() const { return _end - _begin; }
	constexpr size_type length() const { return _end - _begin; }

	constexpr size_type max_size() const;
	constexpr bool empty() const { return _begin == _end; }

	void remove_prefix(size_type n) { _begin += n; }
	void remove_suffix(size_type n) { _end -= n; }

	void swap(basic_string_view& v) {
		std::swap(_begin, v._begin);
		std::swap(_end, v._end);
	}

	template<class Allocator = std::allocator<CharT>>
	std::basic_string<CharT, Traits, Allocator> to_string(const Allocator& a = Allocator()) const {
	    	return std::basic_string<CharT, Traits, Allocator>(_begin, _end);
	    }

	template<class Allocator>
	explicit operator std::basic_string<CharT, Traits, Allocator>() const {
    	return std::basic_string<CharT, Traits, Allocator>(_begin, _end);
	}


	size_type copy( CharT* dest,
	                size_type count,
	                size_type pos = 0) const {
		if (pos > size()) throw std::out_of_range("basic_string_view::copy");
		count = std::min(count, size() - pos);
		Traits::copy(dest, _begin + pos, count);
	}

	/*constexpr*/ basic_string_view substr(size_type pos = 0, size_type count = npos ) const {
		if (pos > size()) throw std::out_of_range("basic_string_view::substr");
		count = std::min(count, size() - pos);
		return basic_string_view(_begin + pos, count);
	}

	constexpr int compare(basic_string_view v) const;
	constexpr int compare(size_type pos1, size_type count1,
	                      basic_string_view v) const;
	constexpr int compare(size_type pos1, size_type count1, basic_string_view v,
	                      size_type pos2, size_type count2) const;
	constexpr int compare(const CharT* s) const;
	constexpr int compare(size_type pos1, size_type count1,
	                      const CharT* s) const;
	constexpr int compare(size_type pos1, size_type count1,
	                      const CharT* s, size_type count2) const;


	constexpr size_type find(basic_string_view v, size_type pos = 0) const;
	constexpr size_type find(CharT c, size_type pos = 0) const;
	constexpr size_type find(const CharT* s, size_type pos, size_type count) const;
	constexpr size_type find(const CharT* s, size_type pos = 0) const;

	constexpr size_type rfind(basic_string_view v, size_type pos = npos) const;
	constexpr size_type rfind(CharT c, size_type pos = npos) const;
	constexpr size_type rfind(const CharT* s, size_type pos, size_type count) const;
	constexpr size_type rfind(const CharT* s, size_type pos = npos) const;

	constexpr size_type find_first_of(basic_string_view v, size_type pos = 0) const;
	constexpr size_type find_first_of(CharT c, size_type pos = 0) const;
	constexpr size_type find_first_of(const CharT* s, size_type pos, size_type count) const;
	constexpr size_type find_first_of(const CharT* s, size_type pos = 0) const;

	constexpr size_type find_last_of(basic_string_view v, size_type pos = npos) const;
	constexpr size_type find_last_of(CharT c, size_type pos = npos) const;
	constexpr size_type find_last_of(const CharT* s, size_type pos, size_type count) const;
	constexpr size_type find_last_of(const CharT* s, size_type pos = npos) const;


	constexpr size_type 
	find_first_not_of(basic_string_view v, size_type pos = 0) const;
	constexpr size_type
	find_first_not_of(CharT c, size_type pos = 0) const;
	constexpr size_type
	find_first_not_of(const CharT* s, size_type pos, size_type count) const;
	constexpr size_type
	find_first_not_of(const CharT* s, size_type pos = 0) const;

	constexpr size_type 
	find_last_not_of(basic_string_view v, size_type pos = npos) const;
	constexpr size_type
	find_last_not_of(CharT c, size_type pos = npos) const;
	constexpr size_type
	find_last_not_of(const CharT* s, size_type pos, size_type count) const;
	constexpr size_type
	find_last_not_of(const CharT* s, size_type pos = npos) const;


private:

	const value_type * _begin = nullptr;
	const value_type * _end = nullptr;

};


typedef basic_string_view<char> string_view;
typedef basic_string_view<wchar_t> wstring_view;
typedef basic_string_view<char16_t> u16string_view;
typedef basic_string_view<char32_t> u32string_view;


#endif
