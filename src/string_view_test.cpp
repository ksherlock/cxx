#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <cxx/string_view.h>


TEST_CASE("constructors") {

	CHECK(string_view("hello").length() == 5);
	CHECK(string_view("hello hello", 5).length() == 5);

}


TEST_CASE("ends_with") {

	CHECK(string_view("this is a test").ends_with('t'));
	CHECK(string_view("this is a test").ends_with("test"));
	CHECK(string_view("this is a test").ends_with(string_view("test")));
}

TEST_CASE("starts_with") {

	CHECK(string_view("this is a test").starts_with('t'));
	CHECK(string_view("this is a test").starts_with("this"));
	CHECK(string_view("this is a test").starts_with(string_view("this")));

	CHECK(!string_view("").starts_with('x'));
}

TEST_CASE("find") {

	CHECK(string_view("abc").find('b') == 1);
	CHECK(string_view("abc").find("b") == 1);
	CHECK(string_view("abc").find("bc") == 1);
	CHECK(string_view("abc").find("bc", 1) == 1);
	CHECK(string_view("abc").find("bc", 1, 1) == 1);
	CHECK(string_view("abc").find("bc", 1, 2) == 1);
	CHECK(string_view("abc").find(string_view("bc")) == 1);
	CHECK(string_view("abc").find("xx", 0) == -1); // string_view::npos causes a link error....
}

TEST_CASE("contains") {
	CHECK(string_view("this is a test").contains("test"));
	CHECK(string_view("this is a test").contains("this"));
	CHECK(string_view("this is a test").contains(string_view("test")));
	CHECK(string_view("this is a test").contains('a'));
}
