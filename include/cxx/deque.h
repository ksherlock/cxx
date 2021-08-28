#ifndef cxx_deque_h
#define cxx_deque_h

#include <deque>
#include <algorithm>

// tsv2 stuff.

template <class T, class A, class U>
void erase(std::deque<T, A>& c, const U& value) {
	c.erase(std::remove(c.begin(), c.end(), value), c.end());
}

template <class T, class A, class Predicate>
void erase_if(std::deque<T, A>& c, Predicate pred) {
	c.erase(std::remove_if(c.begin(), c.end(), pred), c.end());
}

#endif
