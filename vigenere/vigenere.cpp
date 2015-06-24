
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <boost/program_options.hpp>

using namespace std;
using namespace boost::program_options;

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
  map<char,int> forward;
  map<int,char> backward;

public:

  Mapper(const string& filename) {
    ifstream file(filename.c_str());
    char c;
    string token;
    for (int i = 0; file >> token; i++) {
      if (token.size() == 1) {
        c = token[0];
      } else if (token == "space") {
        c = ' ';
      } else if (token == "\\n") {
        c = '\n';
      } else {
        throw SebException("alphabet token not recognized: " + token);
      }
      forward[c] = i;
      backward[i] = c;
    }
    file.close();
  }

  bool contains(char x) const {
    return forward.count(x) > 0;
  }

  int size() const {
    return forward.size();
  }

  int charToInt(char x) const {
    return forward.find(x)->second;
  }

  char intToChar(int x) const {
    return backward.find(x)->second;
  }
};

string encrypt(const string& plain, const string& key, const Mapper& m) {
  string cipher;
  for (int i=0; i<plain.size(); i++) {
    int charId = m.charToInt(plain[i]);
    int keyId = m.charToInt(key[i % key.size()]);
    int cipherId = (charId + keyId) % m.size();
    cipher.push_back(m.intToChar(cipherId));
  }
  return cipher;
}

string decrypt(const string& cipher, const string& key, const Mapper& m) {
  string plain;
  for (int i=0; i<cipher.size(); i++) {
    int cipherId = m.charToInt(cipher[i]);
    int keyId = m.charToInt(key[i % key.size()]);
    int plainId = (cipherId - keyId + m.size()) % m.size();
    plain.push_back(m.intToChar(plainId));
  }
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
      for (auto ch : text) {
        if (!m.contains(ch)) {
          cerr << "The character " << ch << " of the input text is not in the alphabet" << endl;
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
