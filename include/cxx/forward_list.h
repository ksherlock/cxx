#ifndef cxx_forward_list_h
#define cxx_forward_list_h

#include <forward_list>
#include <algorithm>

// tsv2 stuff.

template <class T, class A, class U>
void erase(std::forward_list<T, A>& c, const U& value) {
	c.remove_if([&](auto& elem) { return elem == value; });
}

template <class T, class A, class Predicate>
void erase_if(std::forward_list<T, A>& c, Predicate pred) {
	c.remove_if(pred);
}

#endif
