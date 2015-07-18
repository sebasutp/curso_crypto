
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <boost/multi_array.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include "../encoding.h"

using namespace std;
using namespace encoding;
using namespace boost::program_options;

typedef boost::multi_array<double,2> dmat;
typedef boost::multi_array<int, 2> imat;

dmat loadMat(const string& file_name, int alph_size) {
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
  dmat ans{boost::extents[alph_size][alph_size]};
  ifstream in(file_name);
  string line;
  boost::char_separator<char> sep{" "};
  int i=0,j;
  while (getline(in, line)) {
    tokenizer tok{line,sep};
    j=0;
    for (const string& token : tok) {
      ans[i][j++] = boost::lexical_cast<double>(token);
    }
    i++;
  }
  return ans;
}

double logp(int xn, const dmat& lang_model) {
  double ans=0.0;
  for (int xnext=0; xnext<lang_model.size(); xnext++) {
    ans += lang_model[xn][xnext];
  }
  return log(ans);
}

double logp(int xnext, int xprev, const dmat& lang_model) {
  return log(lang_model[xprev][xnext]) - logp(xprev, lang_model);
}

double getKey(vector<unsigned int>& answer, const vector<unsigned int>& cipher, const dmat& lang_model, int key_len) {
  int alph = lang_model.size();
  double tot_prob = -1e100;
  for (int k0=0; k0<alph; k0++) {
    dmat results {boost::extents[key_len+1][alph]};
    imat best_keys {boost::extents[key_len+1][alph]};
    for (int pos=key_len; pos>0; pos--) {
      for (int prev_key=0; prev_key<alph; prev_key++) {
        if (pos == key_len) {
          double ans = logp((cipher[0] - k0 + alph) % alph, lang_model);
          for (int x = key_len; x < cipher.size(); x+=key_len) {
            ans += logp( (cipher[x] - k0 + alph) % alph,
                (cipher[x-1] - prev_key + alph) % alph, lang_model);
          }
          results[pos][prev_key] = ans;//return ans
          continue;
        }
        double tot_ans = -1e100;
        int best_key;
        for (int k=0; k<alph; k++) {
          double ans = 0;
          for (int x = pos; x < cipher.size(); x+=key_len) {
            ans += logp( (cipher[x] - k + alph) % alph,
                (cipher[x-1] - prev_key + alph) % alph, lang_model);
          }
          ans += results[pos+1][k]; //maxProb(pos+1, k)
          if (ans > tot_ans) {
            tot_ans = ans;
            best_key = k;
          }
        }
        results[pos][prev_key] = tot_ans; //return tot_ans;
        best_keys[pos][prev_key] = best_key;
      }
    }
    if (results[1][k0] > tot_prob) {
      answer.clear();
      answer.push_back(k0);
      for (int pos=1; pos<key_len; pos++) {
        answer.push_back( best_keys[pos][answer.back()] );
      }
      tot_prob = results[1][k0];
    }
  }
  return tot_prob;
}

int main(int argc, char** argv) {
  try {
    options_description desc("Allowed options");
    string alphabet, language_model_file;
    int from_key_len=2, to_key_len=20;
    desc.add_options()
      ("help,h", "print help message")
      ("alphabet,a", value(&alphabet), "Text file containing the alphabet")
      ("lang_model,l", value(&language_model_file), "File with a matrix that represents the language model")
      ("from_key_length,f", value(&from_key_len), "Try keys with given minimum size")
      ("to_key_length,t", value(&to_key_len), "Try keys with maximum size");

    variables_map vm;
    store(parse_command_line(argc,argv,desc), vm);
    notify(vm);
    if (vm.count("help")) {
      cout << desc << endl;
      return 0;
    }
    if (vm.count("alphabet")==0 || vm.count("lang_model")==0) {
      cerr << "The alphabet and language model are not optional parameters, try --help" << endl;
      return 1;
    }

    Mapper mapper(alphabet);
    dmat lang_model = loadMat(language_model_file,mapper.size());
    string cipher_text;
    char ch;
    while ( (ch = cin.get()) != EOF ) {
      cipher_text.push_back(ch);
    }
    if (cipher_text.back() == '\n') cipher_text.pop_back();

    vector<unsigned int> cipher = mapper.stringToInts(cipher_text);
    vector<pair<double,string>> keys;
    for (int len=from_key_len; len<=to_key_len; len++) {
      vector<unsigned int> key;
      double prob = getKey(key, cipher, lang_model, len);
      keys.push_back( make_pair(-prob, mapper.intsToString(key)) );
    }
    sort(keys.begin(), keys.end());
    for (int i=0; i<=(to_key_len-from_key_len); i++) {
      cout << keys[i].second << " with log_probability " << -keys[i].first << endl;
    }
  } catch (const exception& e) {
    cerr << e.what() << endl;
  }
}
