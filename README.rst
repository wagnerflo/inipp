.. -*- restructuredtext -*-
.. |VER| replace:: 1.0
.. |HOME| replace:: https://github.com/wagnerflo/inipp
.. |DOWN| replace:: https://github.com/wagnerflo/inipp/releases


About
=====
Homepage
  |HOME|

Author
  Florian Wagner <florian@wagner-flo.net>

Release
  v\ |VER|

License
  BSD 2-clause (see COPYING)

inipp is a minimalistic ini style config parser for C++. It is contained
in a single header file which can be added to a programs source code. It
is intended for tools that need to read and parse (but not write) ini
style files and do not want to add a dependency to one of the more
advanced config parsing libraries.


Installation and Dependencies
=============================
A tarball of the current version |VER| can be downloaded at |DOWN|. GnuPG
signature are available at the same spot. The lastest code is maintained
in a Git repository at |HOME|.

Copy the ``inipp.hh`` header to a directory of your choice within the
sources of the software you intend to use inipp with. Compilation
requires a C++ standard library with basic stream and string support
and a C++ compiler able to compile said standard library and one
additional class.

Example usage
=============
To read a config file create an instance of the *inipp::inifile* class
and pass it an *std::ifstream* object::
 
 #include "inipp.hh"
 #include <fstream>

 [...]

 std::ifstream cfstream("tests-sunshine.conf");
 inipp::inifile cfile(cfstream);

The constructor of *inipp::inifile* will read the given stream line
by line and parse its contents according to a set of rules:

1. Empty lines, lines with only whitespace in them and lines starting
   with a ``#`` character (and possibly whitespace before that) are
   completely ignored.

2. Lines starting with a ``[`` character and ending with a ``]``
   (ignoring whitespace before and after) are taken as starting
   definitions of sections. Section names are trimmed (``[a]`` is
   equal to ``[␣a␣]``). A missing closing braket makes *inipp*
   throw a *inipp::syntax_error*.

3. Lines containing a ``=`` character and some none whitespace text
   before and after that character are assumed to be key, value
   pairs and are added to the current section. Whitespace next to
   the equal sign is trimmed. Splitting is done on the first ``=``
   found in the line, thus all following equal signs end up in the
   value part.

4. Lines matching none of the previous conditions make *inipp*
   throw a *inipp::syntax_error*.

Assuming a configuration file named ``tests-sunshine.conf`` with the
following content::

 # this is a comment with a = sign
 
 everything = borked
 inipp = may not be borked
 
 [rule the world]
 use lolcats = en masse
 but do not = fall over laughing
 
 [sp3c14|_ c#4r4c73r2]
 do = work in inipp
 
   [   whitespace aplenty   ]
   these are double = = signs

Querying all these key, value pairs is as easy as calling the
*get(const std::string& key)* or
*get(const std::string& sectionname, const std::string& key)* method on
the *inipp::inifile* object::

 std::cout << "everything: " << cfile.get("everything")
           << std::endl
           << "inipp: " << cfile.get("inipp")
           << std::endl
           << "rule the world / use lolcats: "
                  << cfile.get("rule the world", "use lolcats")
           << std::endl
           << "rule the world / but do not: "
                  << cfile.get("rule the world", "but do not")
           << std::endl
           << "sp3c14|_ c#4r4c73r2 / do: "
                  << cfile.get("sp3c14|_ c#4r4c73r2", "do")
           << std::endl
           << "whitespace aplenty / these are double: "
                  << cfile.get("whitespace aplenty", "these are double")
           << std::endl;

This will print the following five lines to stdout::

 everything: borked
 inipp: may not be borked
 rule the world / use lolcats: en masse
 rule the world / but do not: fall over laughing
 sp3c14|_ c#4r4c73r2 / do: work in inipp
 whitespace aplenty / these are double: = signs

When querying non-existent entries or non-existent sections with
the *inipp::inifile::get* methods an *inipp::unknown_entry_error*
or respectivly *inipp::unknown_section_error* exception will be
thrown.

The two *inipp::inifile::dget* methods are an alternative to this
behaviour. These allow an additional parameter
*const std::string& default_value* and return this parameter instead of
throwing exceptions.

For querying multiple values within one section there is a shortcut
to repeatedly typing the sections name: The
*inipp::inifile::section(const std::string& sectioname)*
method returns a *inipp::inisection* object which acts as a wrapper
around a single section. It provides *get(const std::string& key)*
and *dget(const std::string& key, const std::string& default_value)*
methods which work exactly like their counterparts on *inipp::inifile*::

 inisection rule = cfile.section("rule the world");
 std::cout << "rule the world / use lolcats: " << rule.get("use lolcats")
           << std::endl
           << "rule the world / but do not: " << rule.get("but do not")
           << std::endl;

Changelog
=========
- **v1.0:** Typos, minimal refactoring and some changes for consistency.

- **v0.7:** Fix typos in the README.

- **v0.6:** All methods are now inline to allow correct linkage in
  projects using inipp from multiple object files. Also turn
  *inipp::__version__* variable into *INIPP_VERSION* define.

- **v0.5:** Make the methods get, dget, section constant correct.

- **v0.4:** Fix a bug only occuring with -Wall -Werror on gcc.

- **v0.3:** Added an *inipp::inisection* class and assorted methods: It
  provides a wrapper around a single section to allow multiple queries
  without repeating oneself.

- **v0.2:** Extended *inipp::inifile* with two *dget* methods which take
  an additional default_value parameter and return this parameter instead
  of throwing *inipp::unknown_entry_error* and
  *inipp::unknown_entry_error*.

- **v0.1:** Initial release with basic functionallity.

..
   Local Variables:
   mode: rst
   End:
