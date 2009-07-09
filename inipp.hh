/* inipp.hh - minimalistic ini file parser class in single header file

Copyright (c) 2009, Florian Wagner <florian@wagner-flo.net>.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef INIPP_HH
#define INIPP_HH

#include <string>
#include <map>
#include <fstream>
#include <stdexcept>

namespace inipp
{
  
  std::string __version__ = "0.1";

  class unknown_entry_error : public std::runtime_error
  {
    public:
      unknown_entry_error(const std::string& key)
        : std::runtime_error("Unknown sectionless entry '" + key + "'.")
      { /* empty */ };

      unknown_entry_error(const std::string& key, const std::string& section)
        : std::runtime_error("Unknown entry '" + key + "' in section '" +
                             section + "'.")
      { /* empty */ };
  };
  
  class unknown_section_error : public std::runtime_error
  {
    public:
      unknown_section_error(const std::string& section) 
        : std::runtime_error("Unknown section '" + section + "' requested.")
      { /* empty */ };
  };

  class syntax_error : public std::runtime_error
  {
    public:
      syntax_error(const std::string& msg) 
        : std::runtime_error(msg)
      { /* empty */ };
  };

  class inifile
  {
    public:
      inifile(std::ifstream& infile);
      std::string get(const std::string& section, const std::string& key);
      std::string get(const std::string& key);

    protected:
      std::map<std::string,std::map<std::string,std::string> > _sections;
      std::map<std::string,std::string> _defaultsection;
      
      std::string _trim(const std::string& str);
      bool _split(const std::string& in, const std::string& sep,
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

  std::string inifile::get(const std::string& section,
                           const std::string& key) {
    if(!this->_sections.count(section)) {
      throw unknown_section_error(section);
    }

    if(!this->_sections[section].count(key)) {
      throw unknown_entry_error(section, key);
    }

    return this->_sections[section][key];
  }

  std::string inifile::get(const std::string& key) {
    if(!this->_defaultsection.count(key)) {
      throw unknown_entry_error(key);
    }
    
    return this->_defaultsection[key];
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

}

#endif /* INIPP_HH */
