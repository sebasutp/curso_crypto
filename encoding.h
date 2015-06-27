
#ifndef ENCODING_H
#define ENCODING_H

#include <utf8.h>
#include <map>
#include <string>

namespace encoding {
  using namespace std;

  utf8::unchecked::iterator<string::iterator> utf8_begin(string &str) {
    return utf8::unchecked::iterator<string::iterator>(str.begin());
  }

  utf8::unchecked::iterator<string::const_iterator> utf8_begin(const string &str) {
    return utf8::unchecked::iterator<string::const_iterator>(str.begin());
  }

  utf8::unchecked::iterator<string::iterator> utf8_end(string &str) {
    return utf8::unchecked::iterator<string::iterator>(str.end());
  }

  utf8::unchecked::iterator<string::const_iterator> utf8_end(const string &str) {
    return utf8::unchecked::iterator<string::const_iterator>(str.end());
  }

  struct LexicalError : exception {
    string msg;

    LexicalError(const string& message) {
      msg = message;
    }

    const char* what() const noexcept {
      return msg.c_str();
    }
  };

  class Mapper {
    std::map<uint32_t,uint32_t> forward;
    std::map<uint32_t,uint32_t> backward;

    public:

    Mapper(const std::string& filename) {
      std::ifstream file(filename.c_str());
      uint32_t chr;
      std::string token;
      for (int i = 0; file >> token; i++) {
        if (utf8::distance(token.begin(), token.end()) == 1) {
          chr = utf8::peek_next(token.begin(),token.end());
        } else if (token == "space") {
          chr = ' ';
        } else if (token == "\\n") {
          chr = '\n';
        } else {
          throw LexicalError("alphabet token not recognized: " + token);
        }
        if (forward.count(chr) > 0) {
          throw LexicalError("The symbol " + token + " appears morea than once in the alphabet");
        }
        forward[chr] = i;
        backward[i] = chr;
      }
      file.close();
    }

    bool containsChr(uint32_t x) const {
      return forward.count(x) > 0;
    }

    int size() const {
      return forward.size();
    }

    uint32_t charToInt(uint32_t x) const {
      return forward.find(x)->second;
    }

    uint32_t intToChar(uint32_t x) const {
      return backward.find(x)->second;
    }

    vector<uint32_t> stringToInts(const string& s, bool ignore=false) const {
      vector<uint32_t> ans;
      for (auto it = utf8_begin(s); it != utf8_end(s); it++) {
        if (!containsChr(*it)) {
          if (ignore) continue;
          else {
            stringstream ss;
            ss << "The character unicode=" << *it << " is not in the alphabet";
            throw LexicalError(ss.str());
          }
        }
        ans.push_back(charToInt(*it));
      }
      return ans;
    }

    string intsToString(const vector<uint32_t>& v) const {
      string ans;
      vector<uint32_t> tmp;
      for (auto sym : v) {
        if (backward.count(sym) == 0) {
          stringstream ss;
          ss << "Symbol out of bounds. Requested symbol " << sym << " out of " << size();
          throw LexicalError(ss.str());
        }
        tmp.push_back(intToChar(sym));
      }
      utf8::utf32to8(tmp.begin(),tmp.end(),back_inserter(ans));
      return ans;
    }
  };

};

#endif
