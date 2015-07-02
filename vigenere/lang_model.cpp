
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <boost/program_options.hpp>
#include "../encoding.h"

using namespace std;
using namespace boost::program_options;
using namespace encoding;

string myupper(const string& str) {
  string ans;
  for (char c : str) {
    if (c >= 'a' && c <= 'z')
      c = c - 'a' + 'A';
    if (c >= 'A' && c <= 'Z')
      ans.push_back(c);
  }
  return ans;
}

int main() {
  ifstream in("files/sawyer.tmp");
  Mapper m("files/upper_eng.tmp");
  vector< vector<int> > matrix(m.size(), vector<int>(m.size(),0));
  string line;
  int N=0;
  while (getline(in, line)) {
    vector<uint32_t> chars = m.stringToInts(myupper(line));
    for (int i=1; i<chars.size(); i++) {
      matrix[chars[i-1]][chars[i]]++;
      N++;
    }
  }
  for (int i=0; i<m.size(); i++) {
    for (int j=0; j<m.size(); j++) {
      cout << ((double)matrix[i][j] / (double)N) << " ";
    }
    cout << endl;
  }
}
