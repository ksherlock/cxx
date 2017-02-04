#ifndef cxx_list_h
#define cxx_list_h

#include <list>
#include <algorithm>

// tsv2 stuff.

template <class T, class A, class U>
void erase(std::list<T, A>& c, const U& value) {
	c.remove_if([&](auto& elem) { return elem == value; });
}

template <class T, class A, class Predicate>
void erase_if(std::list<T, A>& c, Predicate pred) {
	c.remove_if(pred);
}

#endif
