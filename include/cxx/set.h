#ifndef cxx_set_h
#define cxx_set_h

#include <set>
#include <algorithm>

// tsv2 stuff.

template <class K, class C, class A, class Predicate>
void erase_if(std::set<K, C, A>& c, Predicate pred) {
	for (auto i = c.begin(), last = c.end(); i != last; ) {
	  if (pred(*i)) {
	    i = c.erase(i);
	  } else {
	    ++i;
	  }
	}
}

#endif
