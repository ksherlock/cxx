#ifndef cxx_unordered_multimap_h
#define cxx_unordered_multimap_h

#include <unordered_multimap>
#include <algorithm>

// tsv2 stuff.

template <class K, class T, class H, class P, class A, class Predicate>
void erase_if(std::unordered_multimap<K, T, H, P, A>& c, Predicate pred) {
	for (auto i = c.begin(), last = c.end(); i != last; ) {
	  if (pred(*i)) {
	    i = c.erase(i);
	  } else {
	    ++i;
	  }
	}
}

#endif
