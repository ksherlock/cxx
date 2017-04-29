#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <cxx/filesystem.h>

namespace fs = FS_NAMESPACE;

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
		CHECK(*iter == "/");
		++iter;
		CHECK(iter == end);
	}

	SECTION("/abc/def") {
		fs::path p = fs::path("/abc/def");
		auto iter = p.begin();
		auto end = p.end();

		REQUIRE(iter != end);
		CHECK(*iter == "/");
		++iter;

		REQUIRE(iter != end);
		CHECK(*iter == "abc");
		++iter;


		REQUIRE(iter != end);
		CHECK(*iter == "def");
		++iter;

		CHECK(iter == end);
	}



	SECTION("/abc/") {
		fs::path p = fs::path("/abc/");
		auto iter = p.begin();
		auto end = p.end();

		REQUIRE(iter != end);
		CHECK(*iter == "/");
		++iter;

		REQUIRE(iter != end);
		CHECK(*iter == "abc");
		++iter;


		REQUIRE(iter != end);
		CHECK(*iter == ".");
		++iter;

		CHECK(iter == end);
	}


	SECTION("/abc//////") {
		fs::path p = fs::path("/abc//////");
		auto iter = p.begin();
		auto end = p.end();

		REQUIRE(iter != end);
		CHECK(*iter == "/");
		++iter;

		REQUIRE(iter != end);
		CHECK(*iter == "abc");
		++iter;


		REQUIRE(iter != end);
		CHECK(*iter == ".");
		++iter;

		CHECK(iter == end);
	}

}


