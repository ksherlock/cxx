#include <cxx/cxx_filesystem.h>

#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <unistd.h>
#include <sys/param.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

namespace filesystem {

	namespace {


		template<class FX, class... Args>
		auto syscall(error_code &ec, FX fx, Args&&... args) -> decltype(fx(std::forward<Args>(args)...))
		{
			auto rv = fx(std::forward<Args>(args)...);
			if (rv < 0) {
				ec = error_code(errno, std::generic_category());
			} else {
				ec.clear();
			}

			return rv;
		}

		int fs_stat(const path &p, struct stat *buf, error_code &ec) {
			int rv = stat(p.c_str(), buf);
			if (rv < 0) {
				ec = error_code(errno, std::generic_category());
			}
			else {
				ec.clear();
			}
			return rv;
		}

		int fs_lstat(const path &p, struct stat *buf, error_code &ec) {
			int rv = lstat(p.c_str(), buf);
			if (rv < 0) {
				ec = error_code(errno, std::generic_category());
			}
			else {
				ec.clear();
			}
			return rv;
		}

		template<class FX>
		file_status status_common(FX fx, const path& p, error_code& ec) noexcept {


			struct stat st;
			int rv = fx(p, &st, ec);
			if (rv < 0)
			{
				switch (ec.value())
				{
				case ENOENT:
				case ENOTDIR:
					return file_status(file_type::not_found);

				case EOVERFLOW:
					return file_status(file_type::unknown);

				//case ENAMETOOLONG: ???
				// case ELOOP ?

				default:
					return file_status(file_type::none);					
				}
			}

			ec.clear();
			perms prms = static_cast<perms>(st.st_mode) & perms::mask;

			if (S_ISREG(st.st_mode))
				return file_status(file_type::regular, prms);

			if (S_ISDIR(st.st_mode))
				return file_status(file_type::directory, prms);

			if (S_ISBLK(st.st_mode))
				return file_status(file_type::block, prms);

			if (S_ISFIFO(st.st_mode))
				return file_status(file_type::fifo, prms);

			if (S_ISSOCK(st.st_mode))
				return file_status(file_type::socket, prms);

			return file_status(file_type::unknown, prms);
		}

	}

	file_status status(const path& p) {
		error_code ec;
		file_status result = status(p, ec);
		if (result.type() == file_type::none)
			throw filesystem_error("filesystem::file_status", p, ec);
		return result;
	}

	file_status status(const path& p, error_code& ec) noexcept {

		return status_common(fs_stat, p, ec);
	}

	file_status symlink_status(const path& p) {
		error_code ec;
		file_status result = symlink_status(p, ec);
		if (result.type() == file_type::none)
			throw filesystem_error("filesystem::symlink_status", p, ec);
		return result;		
	}

	file_status symlink_status(const path& p, error_code& ec) noexcept {

		return status_common(fs_lstat, p, ec);
	}


	uintmax_t file_size(const path& p) {
		error_code ec;

		struct stat st;
		//if (fs_stat(p, &st, ec) < 0)
		if (syscall(ec, ::stat, p.c_str(), &st) < 0)
			throw filesystem_error("filesystem::file_size", p, ec);

		return st.st_size;

	}

	uintmax_t file_size(const path& p, error_code& ec) noexcept {

		struct stat st;

		//if (fs_stat(p, &st, ec) < 0)
		if (syscall(ec, ::stat, p.c_str(), &st)  < 0)
			return static_cast<uintmax_t>(-1);

		return st.st_size;
	}

	file_time_type last_write_time(const path& p) {
		error_code ec;
		auto tmp = last_write_time(p, ec);
		if (ec) throw filesystem_error("filesystem::last_write_time", p, ec);
		return tmp;
	}

	file_time_type last_write_time(const path& p, error_code& ec) noexcept {
		using namespace std::chrono;

		struct stat st;
		if (syscall(ec, ::stat, p.c_str(), &st) < 0)
			return file_time_type::min();

		auto ns = nanoseconds(st.st_mtimespec.tv_nsec);
		auto ms = microseconds(duration_cast<microseconds>(ns));
		auto s = seconds(st.st_mtimespec.tv_sec);
		return file_time_type(s + ms);
	}



	bool create_directory(const path& p) {
		error_code ec;
		bool rv = create_directory(p, ec);
		if (ec)
			throw filesystem_error("filesystem::create_directory", p, ec);

		return rv;
	}

	bool create_directory(const path& p, error_code& ec) noexcept {

		ec.clear();

		int rv = ::mkdir(p.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
		if (rv == 0) {
			return true;
		}

		int e = errno;
		error_code tmp;

		// special case -- not an error if the directory already exists.

		if (e == EEXIST && is_directory(p, tmp)) {
			return false;
		}

		ec = error_code(e, std::generic_category());
		return false;
	}

	void resize_file(const path& p, uintmax_t new_size) {
		error_code ec;

		if (syscall(ec, ::truncate, p.c_str(), new_size) < 0)
			throw filesystem_error("filesystem::create_directory", p, ec);
	}

	void resize_file(const path& p, uintmax_t new_size, error_code& ec) noexcept {

		syscall(ec, ::truncate, p.c_str(), new_size);

	}

	bool remove(const path& p) {
		error_code ec;

		if (syscall(ec, ::remove, p.c_str()) < 0) {
			throw filesystem_error("filesystem::remove", p, ec);
		}
		return true;
	}

	bool remove(const path& p, error_code& ec) noexcept {
		if (syscall(ec, ::remove, p.c_str()) < 0) return false;
		return true;
	}


	path current_path() {
		error_code ec;
		path p = current_path(ec);

		if (ec) 
			throw filesystem_error("filesystem::current_path", ec);

		return p;
	}

	path current_path(error_code& ec) {

		char *cp;
		char buffer[PATH_MAX+1];

		ec.clear();
		cp = ::getcwd(buffer, PATH_MAX);
		if (cp) return path(cp);

		ec = error_code(errno, std::generic_category());
		return path();
	}

	void current_path(const path& p) {
		error_code ec;
		syscall(ec, ::chdir, p.c_str());
		if (ec)
			throw filesystem_error("filesystem::current_path", p, ec);
	}

	void current_path(const path& p, error_code& ec) noexcept {

		syscall(ec, ::chdir, p.c_str());
	}

	path temp_directory_path() {
		return "/tmp";
	}

	path temp_directory_path( std::error_code& ec ) noexcept {
		/* todo - check $TMPDIR, $TMP, $TEMP, $TEMPDIR,
		 * check if directory exists and is writable */
		ec.clear();

		return "/tmp";
	}


	path absolute(const path& p, const path& base) {
		if (p.is_absolute()) return p;
		auto tmp = absolute(base);
		tmp /= p;
		return tmp;
	}

	path absolute(const path &p, std::error_code &ec) {
		ec.clear();
		if (p.is_absolute()) return p;
		path rv = current_path(ec);
		if (!ec) rv /= p;
		return rv;
	}

	path absolute(const path &p) {
		std::error_code ec;
		path rv = absolute(p, ec);
		if (ec)
			throw filesystem_error("filesystem::absolute", p, ec);
		return rv;
	}


	path canonical(const path& p) {
		error_code ec;
		path rv = canonical(p, ec);
		if (ec) 
			throw filesystem_error("filesystem::canonical", p, ec);
		return rv;
	}


	path canonical(const path& p, error_code& ec) {
		char *cp;
		char buffer[PATH_MAX+1];

		ec.clear();
		cp = realpath(p.c_str(), buffer);
		if (cp) return path(cp);
		ec = error_code(errno, std::generic_category());
		return path();
	}



}
