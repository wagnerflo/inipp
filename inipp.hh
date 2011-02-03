// inipp.hh - minimalistic ini file parser class in single header file
//
// Copyright (c) 2009, Florian Wagner <florian@wagner-flo.net>.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef INIPP_HH
#define INIPP_HH

#define INIPP_VERSION "0.6"

#include <string>
#include <map>
#include <fstream>
#include <stdexcept>

namespace inipp
{
  class unknown_entry_error : public std::runtime_error
  {
    public:
      inline unknown_entry_error(const std::string& key)
        : std::runtime_error("Unknown sectionless entry '" + key + "'.")
      { /* empty */ };

      inline unknown_entry_error(const std::string& key,
                                 const std::string& sectionname)
        : std::runtime_error("Unknown entry '" + key + "' in section '" +
                             sectionname + "'.")
      { /* empty */ };
  };

  class unknown_section_error : public std::runtime_error
  {
    public:
      inline unknown_section_error(const std::string& sectionname)
        : std::runtime_error("Unknown section '" + sectionname +
                             "' requested.")
      { /* empty */ };
  };

  class syntax_error : public std::runtime_error
  {
    public:
      inline syntax_error(const std::string& msg)
        : std::runtime_error(msg)
      { /* empty */ };
  };

  class inifile;
  class inisection;

  class inisection
  {
    friend class inifile;

    public:
      inline std::string get(const std::string& key) const;
      inline std::string dget(const std::string& key,
                              const std::string& default_value) const;

    protected:
      inline inisection(const std::string& sectionname, const inifile& ini);

      const std::string _sectionname;
      const inifile& _ini;
  };

  class inifile
  {
    public:
      inline inifile(std::ifstream& infile);
      inline std::string get(const std::string& sectionname,
                             const std::string& key) const;
      inline std::string get(const std::string& key) const;

      inline std::string dget(const std::string& sectionname,
                              const std::string& key,
                              const std::string& default_value) const;
      inline std::string dget(const std::string& key,
                              const std::string& default_value) const;
      inline inisection section(const std::string& sectionname) const;

    protected:
      std::map<std::string,std::map<std::string,std::string> > _sections;
      std::map<std::string,std::string> _defaultsection;

      inline std::string _trim(const std::string& str);
      inline bool _split(const std::string& in, const std::string& sep,
                         std::string& first, std::string& second);

  };

  inifile::inifile(std::ifstream& infile) {
    std::map<std::string,std::string>* cursec = &this->_defaultsection;
    std::string line;

    while(std::getline(infile, line)) {
      // trim line
      line = this->_trim(line);

      // ignore empty lines and comments
      if(line == "" || line[0] == '#') {
        continue;
      }

      // section?
      if(line[0] == '[') {
        if(line[line.size() - 1] != ']') {
          throw syntax_error("The section '" + line +
                             "' is missing a closing bracket.");
        }

        line = this->_trim(line.substr(1, line.size() - 2));
        cursec = &this->_sections[line];
        continue;
      }

      // entry: split by "="
      std::string key;
      std::string value;

      // ignore invalid lines
      if(!this->_split(line, "=", key, value)) {
        throw syntax_error("The line '" + line + "' is undecidable.");
      }

      // then trim and set
      (*cursec)[this->_trim(key)] = this->_trim(value);
    }
  }

  std::string inifile::get(const std::string& sectionname,
                           const std::string& key) const {
    if(!this->_sections.count(sectionname)) {
      throw unknown_section_error(sectionname);
    }

    if(!this->_sections.find(sectionname)->second.count(key)) {
      throw unknown_entry_error(sectionname, key);
    }

    return this->_sections.find(sectionname)->second.find(key)->second;
  }

  std::string inifile::get(const std::string& key) const {
    if(!this->_defaultsection.count(key)) {
      throw unknown_entry_error(key);
    }

    return this->_defaultsection.find(key)->second;
  };

  std::string inifile::dget(const std::string& sectionname,
                            const std::string& key,
                            const std::string& default_value) const {
    try {
      return this->get(sectionname, key);
    }
    catch(unknown_section_error& ex) { /* ignore */ }
    catch(unknown_entry_error& ex) { /* ignore */ }

    return default_value;
  }

  std::string inifile::dget(const std::string& key,
                            const std::string& default_value) const {
    try {
      return this->get(key);
    }
    catch(unknown_entry_error& ex) { /* ignore */ }

    return default_value;
  }

  inisection inifile::section(const std::string& sectionname) const {
    if(!this->_sections.count(sectionname)) {
      throw unknown_section_error(sectionname);
    }

    return inisection(sectionname, *this);
  };

  inline std::string inifile::_trim(const std::string& str) {
    size_t startpos = str.find_first_not_of(" \t");
    size_t endpos = str.find_last_not_of(" \t");

    // only whitespace, return empty line
    if(startpos == std::string::npos || endpos == std::string::npos) {
      return "";
    }

    // trim leading and trailing whitespace
    return str.substr(startpos, endpos - startpos + 1);
  }

  inline bool inifile::_split(const std::string& in, const std::string& sep,
                              std::string& first, std::string& second) {
    size_t eqpos = in.find(sep);

    if(eqpos == std::string::npos) {
      return false;
    }

    first = in.substr(0, eqpos);
    second = in.substr(eqpos + sep.size(), in.size() - eqpos - sep.size());

    return true;
  }

  inisection::inisection(const std::string& sectionname, const inifile& ini)
    : _sectionname(sectionname),
      _ini(ini) {
    /* empty */
  }

  inline std::string inisection::get(const std::string& key) const {
    return this->_ini.get(this->_sectionname, key);
  }

  inline std::string inisection::dget(const std::string& key,
                                      const std::string& default_val) const {
    return this->_ini.dget(this->_sectionname, key, default_val);
  }
}

#endif /* INIPP_HH */
