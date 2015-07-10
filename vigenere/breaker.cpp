
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cmath>
#include "../encoding.h"

using namespace std;
using namespace encoding;

typedef vector< vector<double> > Mat;

Mat loadMat() {
  Mat ans;
  ifstream in("lm.tmp");
  string line;
  while (getline(in, line)) {
    vector<double> v;
    stringstream ss;
    ss << line;
    double token;
    while (ss >> token) {
      v.push_back(token);
    }
    ans.push_back(v);
  }
  return ans;
}

double logp(int xn, const Mat& lang_model) {
  double ans=0.0;
  for (int xnext=0; xnext<lang_model.size(); xnext++) {
    ans += lang_model[xn][xnext];
  }
  return log(ans);
}

double logp(int xnext, int xprev, const Mat& lang_model) {
  return log(lang_model[xprev][xnext]) - logp(xprev, lang_model);
}

vector<unsigned int> getKey(const vector<unsigned int>& cipher, const Mat& lang_model, int key_len) {
  int alph = lang_model.size();
  double tot_prob = -1e100;
  vector<unsigned int> answer;
  for (int k0=0; k0<alph; k0++) {
    Mat results(key_len+1, vector<double>(alph));
    vector< vector<int> > best_keys(key_len+1, vector<int>(alph));
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
  cout << tot_prob << endl;
  return answer;
}

int main() {
  Mat lang_model = loadMat();
  Mapper mapper("files/upper_eng.tmp");
  vector<unsigned int> cipher = mapper.stringToInts(
"SLCGNPOEEFBCZMUPRTHUGKCDMNRTMXLXZWSXUWLECRCRWVIVJYDCEISIQDOOQPEJCSECOBUIFLIJYOKRSVNLZLEXNVJCIJ");
  vector<unsigned int> key = getKey(cipher,lang_model,9);
  cout << mapper.intsToString(key) << endl;
}
