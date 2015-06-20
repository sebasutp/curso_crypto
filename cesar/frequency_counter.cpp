
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

int main() {
  string text;
  cin >> text;
  vector<int> freq(26,0);
  for (int i=0; i<text.size(); i++) {
    freq[text[i]-'A']++;
  }
  vector< pair<int,char> > letters;
  for (int i=0; i<26; i++) {
    letters.push_back( make_pair(freq[i], 'A'+i) );
  }
  sort(letters.begin(), letters.end());
  for (int i=0; i<letters.size(); i++) {
    cout << letters[i].second << ": " << letters[i].first << endl;
  }
}
