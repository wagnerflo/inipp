/* inipp.hh - minimal ini file parser class in single header file

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

namespace inipp {
  
  inline std::string trim(const std::string& str);
  inline bool split(const std::string& in, const std::string& sep,
                    std::string& first, std::string& second);

  class entry_error : public std::runtime_error {
    public:
      entry_error(const std::string& msg) 
        : std::runtime_error(msg) {
        /* empty */
      };
  };
  
  class section_error : public std::runtime_error {
    public:
      section_error(const std::string& msg) 
        : std::runtime_error(msg) {
        /* empty */
      };
  };

  class inifile {
    public:
      inifile(std::ifstream& infile) {
        std::map<std::string,std::string>* cursec = &this->_defaultsection;
        std::string line;

        while(std::getline(infile, line)) {
          // trim line
          line = trim(line);
          
          // ignore empty lines and comments
          if(line == "" || line[0] == '#') {
            continue;
          }

          // section?
          if(line[0] == '[') {
            if(line[line.size() - 1] != ']') {
              continue;
            }
            
            line = trim(line.substr(1, line.size() - 2));
            cursec = &this->_sections[line];
            continue;
          }
          
          // entry: split by "="
          std::string key;
          std::string value;

          // ignore invalid lines
          if(!split(line, "=", key, value)) {
            continue;
          }
          
          // then trim and set
          (*cursec)[trim(key)] = trim(value);
        }
      };
      
      std::string get(const std::string& name,
                      const std::string& key) {
        if(!this->_sections.count(name)) {
          throw section_error("Unknown section '" + name + "' requested.");
        }

        if(!this->_sections[name].count(key)) {
          throw entry_error(
                    "Unknown entry '" + key + "' in section '" + name + "'.");
        }

        return this->_sections[name][key];
      };

      std::string get(const std::string& key) {
        if(!this->_defaultsection.count(key)) {
          throw entry_error("Unknown sectionless entry '" + key + "'.");
        }

        return this->_defaultsection[key];
      };

    protected:
      std::map<std::string,std::map<std::string,std::string> > _sections;
      std::map<std::string,std::string> _defaultsection;
  };

  inline std::string trim(const std::string& str) {
    size_t startpos = str.find_first_not_of(" \t");
    size_t endpos = str.find_last_not_of(" \t");
    
    // only whitespace, return empty line
    if(startpos == std::string::npos || endpos == std::string::npos) {
      return "";
    }
    
    // trim leading and trailing whitespace
    return str.substr(startpos, endpos - startpos + 1);
  }

  inline bool split(const std::string& in, const std::string& sep,
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
