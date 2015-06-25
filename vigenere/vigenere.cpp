
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <boost/program_options.hpp>
#include <utf8.h>

using namespace std;
using namespace boost::program_options;

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

struct SebException : exception {
  string msg;

  SebException(const string& message) {
    msg = message;
  }

  const char* what() const noexcept {
    return msg.c_str();
  }
};

class Mapper {
  map<uint32_t,uint32_t> forward;
  map<uint32_t,uint32_t> backward;

public:

  Mapper(const string& filename) {
    ifstream file(filename.c_str());
    uint32_t chr;
    string token;
    for (int i = 0; file >> token; i++) {
      if (utf8::distance(token.begin(), token.end()) == 1) {
        chr = utf8::peek_next(token.begin(),token.end());
      } else if (token == "space") {
        chr = ' ';
      } else if (token == "\\n") {
        chr = '\n';
      } else {
        throw SebException("alphabet token not recognized: " + token);
      }
      if (forward.count(chr) > 0) {
        throw SebException("The symbol " + token + " appears morea than once in the alphabet");
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
};

string encrypt(const string& plain, const string& key, const Mapper& m) {
  string cipher;
  vector<uint32_t> ucipher;
  for (auto tchr = utf8_begin(plain), kchr=utf8_begin(key);
      tchr != utf8_end(plain); tchr++, kchr++) {
    if (kchr == utf8_end(key)) kchr = utf8_begin(key);
    uint32_t charId = m.charToInt(*tchr);
    uint32_t keyId = m.charToInt(*kchr);
    int cipherId = (charId + keyId) % m.size();
    ucipher.push_back(m.intToChar(cipherId));
  }
  utf8::utf32to8(ucipher.begin(),ucipher.end(),back_inserter(cipher));
  return cipher;
}

string decrypt(const string& cipher, const string& key, const Mapper& m) {
  string plain;
  vector<uint32_t> uplain;
  for (auto tchr = utf8_begin(cipher), kchr=utf8_begin(key);
      tchr != utf8_end(cipher); tchr++, kchr++) {
    if (kchr == utf8_end(key)) kchr = utf8_begin(key);
    uint32_t charId = m.charToInt(*tchr);
    uint32_t keyId = m.charToInt(*kchr);
    int cipherId = (charId - keyId + m.size()) % m.size();
    uplain.push_back(m.intToChar(cipherId));
  }
  utf8::utf32to8(uplain.begin(),uplain.end(),back_inserter(plain));
  return plain;
}

int main(int argc, char **argv) {
  try {
    string alphabet = "alfabeto.txt";
    string key, text_file;
    bool do_decrypt = false;
    bool ignore_unknown = false;
    options_description desc("Allowed options");
    desc.add_options()
      ("help,h", "print help message")
      ("alphabet,a", value(&alphabet), "text file containing the alphabet")
      ("key,k", value(&key), "key for vigenere")
      ("decrypt,d", bool_switch(&do_decrypt), "decrypt text instead of encrypting it")
      ("ignore,i", bool_switch(&ignore_unknown), "ignore unknown symbols of the alphabet");

    variables_map vm;
    store(parse_command_line(argc, argv,desc), vm);
    notify(vm);
    if (vm.count("help")) {
      cout << desc << endl;
      return 0;
    }
    if (vm.count("key") == 0) {
      cerr << "No key was given. Try --help to see options" << endl;
      return 1;
    }

    Mapper m(alphabet);
    string text, output;
    char ch;
    while ( (ch = cin.get()) != EOF ) {
      text.push_back(ch);
    }
    if (text.back() == '\n') text.pop_back();
    if (!ignore_unknown) {
      for (auto ch = utf8_begin(text); ch != utf8_end(text); ch++) {
        if (!m.containsChr(*ch)) {
          cerr << "The character Unicode=" << *ch << " of the input text is not in the alphabet" << endl;
          return 1;
        }
      }
    }

    if (!do_decrypt) {
      output = encrypt(text, key, m);
    } else {
      output = decrypt(text, key, m);
    }
    cout << output << endl;
  } catch (exception& e) {
    cerr << e.what() << endl;
  }
}
