#ifndef cxx_unordered_multiset_h
#define cxx_unordered_multiset_h

#include <unordered_multiset>
#include <algorithm>

// tsv2 stuff.

template <class K, class H, class P, class A, class Predicate>
void erase_if(std::unordered_multiset<K, H, P, A>& c, Predicate pred) {
	for (auto i = c.begin(), last = c.end(); i != last; ) {
	  if (pred(*i)) {
	    i = c.erase(i);
	  } else {
	    ++i;
	  }
	}
}

#endif
