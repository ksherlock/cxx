#ifndef cxx_map_h
#define cxx_map_h

#include <map>
#include <algorithm>

// tsv2 stuff.

template <class K, class T, class C, class A, class Predicate>
void erase_if(std::map<K, T, C, A>& c, Predicate pred) {
	for (auto i = c.begin(), last = c.end(); i != last; ) {
	  if (pred(*i)) {
	    i = c.erase(i);
	  } else {
	    ++i;
	  }
	}
}

#endif
