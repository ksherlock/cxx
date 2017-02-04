#ifndef cxx_multimap_h
#define cxx_multimap_h

#include <multimap>
#include <algorithm>

// tsv2 stuff.

template <class K, class T, class C, class A, class Predicate>
void erase_if(std::multimap<K, T, C, A>& c, Predicate pred) {
	for (auto i = c.begin(), last = c.end(); i != last; ) {
	  if (pred(*i)) {
	    i = c.erase(i);
	  } else {
	    ++i;
	  }
	}
}

#endif
