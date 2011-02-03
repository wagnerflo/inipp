#define BOOST_TEST_MODULE tests
#include <boost/test/included/unit_test.hpp>

#include <inipp.hh>

BOOST_AUTO_TEST_CASE( sunshine_inifile )
{
  std::ifstream cstream("tests-sunshine.conf");
  inipp::inifile cfile(cstream);

  // correct queries
  BOOST_REQUIRE_EQUAL(cfile.get("everything"),
                      "borked");
  BOOST_REQUIRE_EQUAL(cfile.dget("inipp", "is buggy as hell"),
                      "may not be borked");
  BOOST_REQUIRE_EQUAL(cfile.dget("inivisible entry", "is invisible"),
                      "is invisible");
  BOOST_REQUIRE_EQUAL(cfile.get("rule the world", "use lolcats"),
                      "en masse");
  BOOST_REQUIRE_EQUAL(cfile.get("rule the world", "but do not"),
                      "fall over laughing");
  BOOST_REQUIRE_EQUAL(cfile.get("sp3c14|_ c#4r4c73r2", "do"),
                      "work in inipp");
  BOOST_REQUIRE_EQUAL(cfile.dget("sp3c14|_ c#4r4c73r2", "do", "not work"),
                      "work in inipp");
  BOOST_REQUIRE_EQUAL(cfile.dget("sp3c14|_ c#4r4c73r2", "are", "funky"),
                      "funky");
  BOOST_REQUIRE_EQUAL(cfile.get("whitespace aplenty", "these are double"),
                      "= signs");
  BOOST_REQUIRE_EQUAL(cfile.dget("no section", "without an entry",
                                 "and no value"),
                      "and no value");

  // queries supposed to throw up
  BOOST_REQUIRE_THROW(cfile.get("nothing"), inipp::unknown_entry_error);
  BOOST_REQUIRE_THROW(cfile.get("rule the world", "use loldogs"),
                      inipp::unknown_entry_error);
  BOOST_REQUIRE_THROW(cfile.get("normal characters", "work in inipp"),
                      inipp::unknown_section_error);
  BOOST_REQUIRE_THROW(cfile.get("everything = borked"),
                      inipp::unknown_entry_error);
}

BOOST_AUTO_TEST_CASE( sunshine_inisection )
{
  std::ifstream cstream("tests-sunshine.conf");
  inipp::inifile cfile(cstream);
  inipp::inisection rule = cfile.section("rule the world");

  // correct queries
  BOOST_REQUIRE_EQUAL(rule.get("use lolcats"), "en masse");
  BOOST_REQUIRE_EQUAL(rule.get("but do not"), "fall over laughing");
  BOOST_REQUIRE_EQUAL(rule.dget("use of force", "is a bad idea"),
                      "is a bad idea");

  // queries supposed to throw up
  BOOST_REQUIRE_THROW(cfile.section("nosection"),
                      inipp::unknown_section_error);
  BOOST_REQUIRE_THROW(rule.get("use of force"),
                      inipp::unknown_entry_error);
}

BOOST_AUTO_TEST_CASE( malformed )
{
  // we use a single ifstream
  std::ifstream cstream("tests-malformed.conf");

  // The test config has two lines with distinct errors on them. So we
  // construct a inipp::inifile twice without rewinding the ifstream
  // in between. Both times an error should be thrown.
  BOOST_REQUIRE_THROW(inipp::inifile cfile(cstream), inipp::syntax_error);
  BOOST_REQUIRE_THROW(inipp::inifile cfile(cstream), inipp::syntax_error);

  // But no error afterwards.
  BOOST_REQUIRE_NO_THROW(inipp::inifile cfile(cstream));
}
