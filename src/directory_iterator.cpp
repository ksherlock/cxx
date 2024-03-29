#include <cxx/cxx_filesystem.h>

#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>


namespace filesystem {


#pragma mark - directory_entry

#if 0
	directory_entry::directory_entry(const class path& p, file_status st, file_status symlink_st) :
			_path(p), _st(st), _lst(symlink_st)
	{}

	void directory_entry::assign(const class path& p, file_status st, file_status symlink_st)
	{
		_path = p;
		_st = st;
		_lst = symlink_st;
	}

	void directory_entry::replace_filename(const class path& p, file_status st, file_status symlink_st)
	{
		_path.replace_filename(p);
		_st = st;
		_lst = symlink_st;
	}

#endif

	directory_entry::directory_entry(const class path& p) :
			_path(p)
	{
		refresh();
	}

	directory_entry::directory_entry(const class path& p, std::error_code &ec) :
			_path(p)
	{
		refresh(ec);
	}



	void directory_entry::assign(const class path& p) {
		_path = p;
		refresh();
	}

	void directory_entry::assign(const class path& p, std::error_code &ec) {
		_path = p;
		refresh(ec);
	}


	void directory_entry::replace_filename( const class path& p ) {
		_path.replace_filename(p);
		refresh();
	}

	void directory_entry::replace_filename( const class path& p, std::error_code& ec ) {
		_path.replace_filename(p);
		refresh(ec);
	}

	void directory_entry::refresh() {
		_st = _lst = file_status(file_type::none);
		_st = _lst = filesystem::symlink_status(_path);
		if (filesystem::is_symlink(_lst)) {
			_st = filesystem::status(_path);
		}
	}

	void directory_entry::refresh(std::error_code &ec) {
		_st = _lst = file_status(file_type::none);

		_st = _lst = filesystem::symlink_status(_path, ec);
		if (ec) return;

		if (filesystem::is_symlink(_lst)) {
			_st = filesystem::status(_path, ec);
			if (ec) return;
		}
	}

	file_status directory_entry::status() const {

		if (!filesystem::status_known(_st))
			_st = filesystem::status(_path);
		return _st;
	}

	file_status directory_entry::status(error_code& ec) const noexcept {

		ec.clear();
		if (!filesystem::status_known(_st))
			_lst = filesystem::status(_path, ec);

		return _lst;
	}

	file_status directory_entry::symlink_status() const {

		if (!status_known(_lst))
			_lst = filesystem::symlink_status(_path);

		return _lst;
	}

	file_status directory_entry::symlink_status(error_code& ec) const noexcept {

		ec.clear();

		if (!status_known(_lst))
			_lst = filesystem::symlink_status(_path, ec);

		return _lst;
	}


	bool directory_entry::operator< (const directory_entry& rhs) const noexcept {
		return _path <  rhs._path;
	}
	bool directory_entry::operator==(const directory_entry& rhs) const noexcept {
		return _path == rhs._path;
	}
	bool directory_entry::operator!=(const directory_entry& rhs) const noexcept {
		return _path != rhs._path;
	}
	bool directory_entry::operator<=(const directory_entry& rhs) const noexcept {
		return _path <= rhs._path;
	}
	bool directory_entry::operator> (const directory_entry& rhs) const noexcept {
		return _path >  rhs._path;
	}
	bool directory_entry::operator>=(const directory_entry& rhs) const noexcept {
		return _path >= rhs._path;
	}


#pragma mark - directory_iterator

	directory_iterator::directory_iterator(const path& p) {


		error_code ec;

		open_dir(p, ec);
		if (ec) throw filesystem_error("directory_iterator::directory_iterator", p, ec);

		increment(ec);
		if (ec) throw filesystem_error("directory_iterator::directory_iterator", p, ec);
	}

	directory_iterator::directory_iterator(const path& p, error_code& ec) noexcept {
		ec.clear();

		open_dir(p, ec);
		if (!ec) increment(ec);
	}

	void directory_iterator::open_dir(const path &p, error_code &ec) noexcept {

		DIR *dp = opendir(p.c_str());
		if (!dp)
		{
				ec = error_code(errno, std::generic_category());
				return;
		}

		_imp = std::make_shared<imp>(p, dp);
	}



	directory_iterator& directory_iterator::operator++() {

		error_code ec;

		increment(ec);
		if (ec) throw filesystem_error("directory_iterator::operator++", ec);

		return *this;
	}

	directory_iterator& directory_iterator::increment(error_code& ec) noexcept {

		ec.clear();

		if (_imp) {

			struct dirent entry;
			struct dirent *result = nullptr;
			int rv;

			DIR *dp = _imp->_dp;

			for(;;)
			{


				rv = readdir_r(dp, &entry, &result);
				if (rv != 0)
				{
					ec = error_code(errno, std::generic_category());
					_imp.reset();
					break;
				}

				if (!result) {
					_imp.reset();
					break; // end of directory.
				}

				std::string s(entry.d_name);
				if (s == ".") continue;
				if (s == "..") continue;


				path p = _imp->_path / s;

				_imp->_entry.assign(p);

				break;
			}
		}

		return *this;
	}



}
