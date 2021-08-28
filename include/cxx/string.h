#ifndef cxx_string_h
#define cxx_string_h

#include <string>
#include <algorithm>

// tsv2 stuff.

template <class charT, class traits, class A, class U>
void erase(std::basic_string<charT, traits, A>& c, const U& value) {
	c.erase(std::remove(c.begin(), c.end(), value), c.end());
}

template <class charT, class traits, class A, class Predicate>
void erase_if(std::basic_string<charT, traits, A>& c, Predicate pred) {
	c.erase(std::remove_if(c.begin(), c.end(), pred), c.end());
}

#endif
