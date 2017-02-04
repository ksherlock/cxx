#ifndef cxx_multiset_h
#define cxx_multiset_h

#include <multiset>
#include <algorithm>

// tsv2 stuff.

template <class K, class C, class A, class Predicate>
void erase_if(std::multiset<K, C, A>& c, Predicate pred) {
	for (auto i = c.begin(), last = c.end(); i != last; ) {
	  if (pred(*i)) {
	    i = c.erase(i);
	  } else {
	    ++i;
	  }
	}
}

#endif
