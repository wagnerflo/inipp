#define BOOST_TEST_MODULE tests
#include <boost/test/included/unit_test.hpp>

#include <inipp.hh>

BOOST_AUTO_TEST_CASE( sunshine )
{
  std::ifstream cstream("tests-sunshine.cfg");
  inipp::inifile cfile(cstream);

  BOOST_REQUIRE_EQUAL(cfile.get("everything"),
                      "borked");
  BOOST_REQUIRE_EQUAL(cfile.get("inipp"),
                      "may not be borked");
  BOOST_REQUIRE_EQUAL(cfile.get("rule the world", "use lolcats"),
                      "en masse");
  BOOST_REQUIRE_EQUAL(cfile.get("sp3c14|_ c#4r4c73r2", "do"),
                      "work in inipp");
}
