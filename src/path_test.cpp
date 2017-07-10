#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <cxx/filesystem.h>

namespace fs = FS_NAMESPACE;


TEST_CASE("fs.path.append", "[std]") {

	CHECK((fs::path("foo") / "").native() == "foo/");
	CHECK((fs::path("foo") / "/bar").native() == "/bar");

}

TEST_CASE("fs.path.modifiers", "[std]") {

	SECTION("remove_filename") {
		CHECK(fs::path("foo/bar").remove_filename().native() == "foo/");
		CHECK(fs::path("foo/").remove_filename().native() == "foo/");
		CHECK(fs::path("/foo").remove_filename().native() == "/");
		CHECK(fs::path("/").remove_filename().native() == "/");
	}

	SECTION("replace_filename") {
		CHECK(fs::path("/foo").replace_filename("bar").native() == "/bar");
		CHECK(fs::path("/").replace_filename("bar").native() == "/bar");
	}

}


TEST_CASE("fs.path.decompose", "[std]") {

	SECTION("filename") {
		CHECK(fs::path("foo/bar").remove_filename().native() == "foo/");
		CHECK(fs::path("foo/").remove_filename().native() == "foo/");
		CHECK(fs::path("/foo").remove_filename().native() == "/");
		CHECK(fs::path("/").remove_filename().native() == "/");

		CHECK(fs::path("/foo/bar.txt").filename().native() == "bar.txt");
		CHECK(fs::path("/foo/bar").filename().native() == "bar");
		CHECK(fs::path("/foo/bar/").filename().native() == "");
		CHECK(fs::path("/").filename().native() == "");
		/* i don't support // magic. */
		//CHECK(fs::path("//host").filename().native() == "");
		CHECK(fs::path(".").filename().native() == ".");
		CHECK(fs::path("..").filename().native() == "..");
	}

	SECTION("stem") {
		CHECK(fs::path("/foo/bar.txt").stem().native() == "bar");

		fs::path p = "foo.bar.baz.tar";
		CHECK(p.extension().native() == ".tar");
		p = p.stem();
		CHECK(p.extension().native() == ".baz");
		p = p.stem();
		CHECK(p.extension().native() == ".bar");
		p = p.stem();
		CHECK(p.extension().empty());


		CHECK(fs::path("/foo/bar.txt").stem().native() == "bar");
		CHECK(fs::path("/foo/bar").stem().native() == "bar");
		CHECK(fs::path("/foo/.profile").stem().native() == ".profile");
		CHECK(fs::path(".bar").stem().native() == ".bar");
		CHECK(fs::path("..bar").stem().native() == ".");


	}

	SECTION("extension") {
		CHECK(fs::path("/foo/bar.txt").extension().native() == ".txt");
		CHECK(fs::path("/foo/bar").extension().native() == "");
		CHECK(fs::path("/foo/.profile").extension().native() == "");
		CHECK(fs::path(".bar").extension().native() == "");
		CHECK(fs::path("..bar").extension().native() == ".bar");
	}

}

TEST_CASE("fs.path.query", "[std]") {
	SECTION("is_absolute") {
		CHECK(fs::path("/").is_absolute());
	}
}

#if 0
/* not yet implemented */
TEST_CASE("fs.path.gen", "[std]") {

	SECTION("lexically_normal") {
		CHECK(fs::path("foo/./bar/..").lexically_normal().native() == "foo/");
		CHECK(fs::path("foo/.///bar/../").lexically_normal().native() == "foo/");	
	}

	SECTION("lexically_relative") {

		TEST_CASE(fs::path("/a/d").lexically_relative("/a/b/c").native() == "../../d");
		TEST_CASE(fs::path("/a/b/c").lexically_relative("/a/d").native() == "../b/c");
		TEST_CASE(fs::path("a/b/c").lexically_relative("a").native() == "b/c");
		TEST_CASE(fs::path("a/b/c").lexically_relative("a/b/c/x/y").native() == "../..");
		TEST_CASE(fs::path("a/b/c").lexically_relative("a/b/c").native() == ".");
		TEST_CASE(fs::path("a/b").lexically_relative("c/d").native() == "../../a/b");

	}

}
#endif

TEST_CASE("path iterator", "[path]") {

	SECTION("") {
		fs::path p("");
		CHECK(std::begin(p) == std::end(p));
	}

	SECTION("/") {
		fs::path p = fs::path("/");
		auto iter = p.begin();
		auto end = p.end();

		REQUIRE(iter != end);
		CHECK((*iter).native() == "/");
		++iter;
		CHECK(iter == end);
	}

	SECTION("/abc/def") {
		fs::path p = fs::path("/abc/def");
		auto iter = p.begin();
		auto end = p.end();

		REQUIRE(iter != end);
		CHECK((*iter).native() == "/");
		++iter;

		REQUIRE(iter != end);
		CHECK((*iter).native() == "abc");
		++iter;


		REQUIRE(iter != end);
		CHECK((*iter).native() == "def");
		++iter;

		CHECK(iter == end);
	}



	SECTION("/abc/") {
		fs::path p = fs::path("/abc/");
		auto iter = p.begin();
		auto end = p.end();

		REQUIRE(iter != end);
		CHECK((*iter).native() == "/");
		++iter;

		REQUIRE(iter != end);
		CHECK((*iter).native() == "abc");
		++iter;


		REQUIRE(iter != end);
		CHECK((*iter).native() == ".");
		++iter;

		CHECK(iter == end);
	}


	SECTION("/abc//////") {
		fs::path p = fs::path("/abc//////");
		auto iter = p.begin();
		auto end = p.end();

		REQUIRE(iter != end);
		CHECK((*iter).native() == "/");
		++iter;

		REQUIRE(iter != end);
		CHECK((*iter).native() == "abc");
		++iter;


		REQUIRE(iter != end);
		CHECK((*iter).native() == ".");
		++iter;

		CHECK(iter == end);
	}

}


