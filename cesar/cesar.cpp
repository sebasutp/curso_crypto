
#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<int> encode(const string& s) {
  vector<int> ans;
  for (int i=0; i<s.size(); i++) {
    ans.push_back(s[i] - 'A');
  }
  return ans;
}

string decode(const vector<int>& v) {
  string ans;
  for (int i=0; i<v.size(); i++) {
    ans.push_back((char)(v[i] + 'A'));
  }
  return ans;
}

vector<int> encrypt(const vector<int> plain, int k) {
  vector<int> cipher;
  for (int i=0; i<plain.size(); i++) {
    cipher.push_back( (plain[i] + k) % 26 );
  }
  return cipher;
}

vector<int> decrypt(const vector<int> cipher, int k) {
  vector<int> plain;
  for (int i=0; i<cipher.size(); i++) {
    plain.push_back( (cipher[i] - k + 26) % 26 );
  }
  return plain;
}

void test() {
  string input;
  int k;
  cin >> input >> k;
  vector<int> text = encode(input);
  vector<int> cipher = encrypt(text, k);
  string ans = decode(cipher);
  cout << ans << endl;
}

int main() {
  string text;
  int k, op;
  vector<int> plain, cipher;
  while (true) {
    cout << "Menu" << endl;
    cout << "1) Cifrar" << endl;
    cout << "2) Descifrar" << endl;
    cout << "0) Salir" << endl;
    cin >> op;
    if (op==0) break;
    cout << "Ingrese el texto y la clave: ";
    cin >> text >> k;
    if (op==1) {
      plain = encode(text);
      cipher = encrypt(plain,k);
      text = decode(cipher);
    } else if (op == 2) {
      cipher = encode(text);
      plain = decrypt(cipher,k);
      text = decode(plain);
    }
    cout << text << endl;
  }
}
