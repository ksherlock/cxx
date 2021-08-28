
#include <cxx/cxx_filesystem.h>


/*
   some fs normalization proposals require an explicit /. at the end
   to indicate a directory is a directory.
*/
//#define VMS_SUPPORT



namespace filesystem {


	namespace {
		const path::value_type separator = '/';

		// hmmm... these could be pre-studied since they're constant?
		const path path_dot = ".";
		const path path_dotdot = "..";
		const path path_sep = "/";
		const path path_empty = "";

		// special flags
		enum {
			none = 0,
			explicit_dot,
			implicit_dot,
			explicit_dotdot,
			slash_only
		};


	}

	void path::study() const {

		if (_info.valid) return;
		int length = _path.length();
		if (length == 0)
		{
			_info.valid = true;
			_info.special = none;
			_info.stem = _info.extension = 0;
			return;
		}

		auto back = _path[length - 1];

		// check for special cases (part 1)
		// if the path is all /s, filename is /
		// if the path ends with a / (but contains a non-/ char),
		// the filename is .

		value_type special = none;
		if (back == separator) {
			special = implicit_dot;
			if (_path.find_first_not_of(separator) == _path.npos)
				special = slash_only;

			_info.valid = true;
			_info.extension = length;
			_info.stem = length;
			_info.special = special;
			return;
		}

		int stem = 0;
		int extension = length;
		for (int i = length; i; ) {
			auto c = _path[--i];
			if (c == '.' && extension == length)
				extension = i;
			if (c == '/') {
				stem = i + 1;
				break;
			}
		}
		// .profile -> stem = ".profile", extension = ""
		if (extension == stem) extension = length;

		// check for special cases (part 2)
		// ".." and "." are not extensions.
		if (back == '.') {
			int xlength = length - stem;
			if (xlength == 1) {
				extension = length;
				special = explicit_dot;
			}
			if (xlength == 2 && _path[stem] == '.') {
				extension = length;
				special = explicit_dotdot;
			}
		}

		_info.valid = true;
		_info.stem = stem;
		_info.extension = extension;
		_info.special = special;
	}


#if 0
	path::path(const path &rhs) :
		_path(rhs._path), _info(rhs._info)
	{}

	path::path(path &&rhs) noexcept :
		_path(std::move(rhs._path))
	{
		rhs.invalidate();
	}
#endif


	path &path::append_common(const std::string &s) {

		invalidate();

		// "foo" / "" -> "foo/"
		if (_path.back() != separator) _path.push_back(separator);

		if (s.empty()) return *this;

		if (&s == &_path) {
			string_type tmp(s);
			_path.append(tmp);
		} else {
			_path.append(s);
		}
		return *this;

	}


	path& path::append(const path& p)
	{
		if (empty() || p.is_absolute()) {
			return (*this = p);
		}
		return append_common(p._path);
	}


	path& path::append(const string_type &s)
	{

		bool is_absolute = !s.empty() && s.front() == separator;
		if (empty() || is_absolute) {
			invalidate();
			_path = s;
			return *this;
		}
		return append_common(s);
	}




	path path::filename() const {

		if (empty()) return *this;
		if (!_info.valid) study();

		if (_info.special == slash_only) return path_empty;
		if (_info.special == implicit_dot) return path_empty;

		if (_info.stem == 0) return *this;
		return _path.substr(_info.stem);
	}

	path path::stem() const {

		// filename without the extension.

		if (empty()) return *this;
		if (!_info.valid) study();

		if (_info.special == slash_only) return path_empty;
		if (_info.special == implicit_dot) return path_empty;

		return _path.substr(_info.stem, _info.extension - _info.stem);
	}

	path path::extension() const {

		if (empty()) return *this;
		if (!_info.valid) study();

		return _path.substr(_info.extension);
	}

	bool path::remove_trailing_slashes() {
		if (_path.empty()) return false;
		if (_path.back() != separator) return false;

		do { _path.pop_back(); } while (!_path.empty() && _path.back() == separator);
		invalidate();
		return true;
	}

	bool path::has_parent_path() const
	{
		// if there is a /, it has a parent path.
		// ... unless it's /.

		if (empty()) return false;

		if (!_info.valid) study();

		if (_info.special == slash_only) return false;

		return _path.find(separator) != _path.npos;
	}

	path path::parent_path() const {

		/*
		 * special cases:
		 * /abc -> /
		 * /abc/ -> /abc
		 * all trailing /s are removed.
		 *
		 */


		if (empty()) return *this;

		if (!_info.valid) study();

		// "/" is a file of "/" with a parent of ""
		if (_info.special == slash_only) return path_empty;

		// stem starts at 0, eg "abc"
		if (!_info.stem) return path_empty;


		auto tmp = _path.substr(0, _info.stem - 1);

		// remove trailing slashes, but return "/" if nothing BUT /s.
		while (!tmp.empty() && tmp.back() == separator) tmp.pop_back();
		if (tmp.empty()) return path_sep;

		return path(tmp);
	}

	path path::root_directory() const {
		// for unix, root directory is / or "".
		if (empty()) return *this;

		return _path.front() == '/' ? path_sep : path_empty;
	}

	path path::root_name() const {
		/*
		 * boost (unix) considers // or //component
		 * to be a root name (and only those cases).
		 *
		 * I do not.
		 */

		return path();
	}

	path path::root_path() const {
		// root_name + root_directory.
		// since root_name is always empty...

		return root_directory();
	}


	path path::relative_path() const {
		// first pathname *after* the root path
		// root_path is first / in this implementation.

		if (is_relative()) return *this;

		auto pos = _path.find_first_not_of(separator);
		if (pos == _path.npos) return path();

		return path(_path.substr(pos));
	}



	// compare
	int path::compare(const path& p) const noexcept {
		if (&p == this) return 0;
		return _path.compare(p._path);
	}

	int path::compare(const std::string& s) const {
		if (&s == &_path) return 0;
		return _path.compare(s);
	}

	int path::compare(const value_type* s) const {
		return _path.compare(s);
	}

#pragma mark - modifiers

	path& path::remove_filename() {

		if (empty()) return *this;
		if (!_info.valid) study();

		if (_info.special == slash_only) return *this;

		_path.resize(_info.stem);
		invalidate();
		return *this;
	}

	path& path::replace_filename(const path &replacement) {
		if (!_info.valid) study();

		if (_info.special == slash_only || _info.special == implicit_dot) {
			append(replacement);
			return *this;
		}
		if (empty() || _info.stem == 0) {
			return *this = replacement;
		}

		_path.resize(_info.stem);
		_path += replacement._path;
		invalidate();
		return *this;
	}

	// replace components.

	path& path::replace_extension(const path& replacement) {
		if (!_info.valid) study();

		// poorly defined if this is special, like / or .
		if (!_info.special) {
			// _info.extension
			_path.resize(_info.extension);
			if (!replacement.empty()) {
				// replacement may or may not have a leading .
				if (replacement._path.front() != '.')
					_path.push_back('.');
				_path += replacement._path;
			}
			invalidate();
		}
		return *this;
	}


	path& path::replace_extension() {
		if (!_info.valid) study();

		// poorly defined if this is special, like / or .
		if (!_info.special) {
			// _info.extension
			_path.resize(_info.extension);
			invalidate();
		}
		return *this;
	}



#pragma mark - generation

	path path::lexically_normal() const {

		if (empty()) return *this;

		if (!_info.valid) study();

		path rv;


		/*
		 *  remove [ . / ]
		 *  remove [ xxx / .. /? ]
		 *  remove [ / .. $ ]
		 */

		bool append_slash = false;
		for (auto p : *this) {
			auto cp = p.c_str();
			if (cp[0] == 0) continue;
			if (cp[0] == '.') {
				if (cp[1] == 0) continue;
				if (cp[1] == '.' && cp[2] == 0) {


					if (!rv._info.valid) rv.study();

					// .. is ok
					// ../.. is ok
					// file/.. is not.
					// /.. is not


					if (rv.empty() || rv._info.special == explicit_dotdot) {
						append_slash = false;
						rv /= p;
					} else {
						append_slash = true;
						rv.remove_filename();
					}
					continue;

				}
			}
			append_slash = false;
			rv /= p;
		}

		if (append_slash) rv /= "";

		// special case - don't remove explicit dot @ end.
		if (_info.special == explicit_dot) rv /= ".";
		// but implicit dot means there is a trailing / which should be preserved.
		if (_info.special == implicit_dot) rv /= "";

		// special case empty string -> '.'
		if (rv.empty()) rv = path_dot;
		return rv;
	}

#pragma mark - iteration

	size_t path::iterator::next() {
		if (_index == 0 && _data->front() == separator) {
			_current = "/";
			return 1;
		}
		auto pos = _data->find(separator, _index);
		if (pos == _data->npos) {
			_current = _data->substr(_index);
			return _data->length();
		}
		_current = _data->substr(_index, pos - _index);
		return pos;

	}

	path::iterator& path::iterator::operator++() {
		_current.clear();
		if (!_data || _index == _data->npos) return *this;
		if (_index == _data->size()) {
			_index = _data->npos; return *this;
		}

		// skip /s
		if ((*_data)[_index] == separator) {
			while (_index < _data->size() && (*_data)[_index] == separator)
				++_index;
			if (_index == _data->size()) {
				_current = path_empty;
				return *this;
			}
		}

		_index = next();
		return *this;
	}

	bool path::iterator::operator==(const iterator &rhs) const noexcept {
		if (_data == rhs._data && _index == rhs._index) return true;
		if (_data == nullptr) {
			if (rhs._index == rhs._data->npos) return true;
		}
		if (rhs._data == nullptr) {
			if (_index == _data->npos) return true;
		}
		return false;
	}
	bool path::iterator::operator!=(const iterator &rhs) const noexcept {
		return !(*this == rhs);
	}

}
