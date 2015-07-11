
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sodium.h>
#include <boost/program_options.hpp>
#include <exception>

using namespace std;
using namespace boost::program_options;

vector<char> readAllBytes(const string& fname) {
  ifstream ifs(fname, ios::binary | ios::ate);
  ifstream::pos_type pos = ifs.tellg();
  vector<char> ans(pos);
  ifs.seekg(0,ios::beg);
  ifs.read(&ans[0], pos);
  return ans;
}

void writeBytes(const string& fname, const vector<char>& bytes) {
  ofstream ofs(fname, ios::binary);
  ofs.write(&bytes[0], bytes.size());
  ofs.flush();
  ofs.close();
}

int main(int argc, char **argv) {
  try {
    if (sodium_init() == -1) {
      return 1;
    }
    options_description desc("Options");
    string in_file, out_file, key_file;
    uint64_t nonce[2];
    desc.add_options()
      ("help,h", "Print help message")
      ("input,i", value<string>(&in_file)->required(), "Input file to encrypt or decrypt")
      ("output,o", value<string>(&out_file)->required(), "Output file to store results")
      ("key,k", value<string>(&key_file)->required(), "Binary file with the key to use")
      ("nonce,n", value<uint64_t>(&nonce[1])->required(), "Nonce for encrypt/decrypt");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
      cout << desc << endl;
      return 0;
    }
    notify(vm);

    vector<char> data = readAllBytes(in_file);
    vector<char> key = readAllBytes(key_file);
    vector<char> out(data.size());
    //unique_ptr<unsigned char> out = new unsigned char[data.size()];
    nonce[0] = 0;
    crypto_stream_aes128ctr_xor((unsigned char*)&out[0], (unsigned char*)&data[0], data.size(),
        (unsigned char*)nonce, (unsigned char*)&key[0]);
    writeBytes(out_file, out);
  } catch (const exception& e) {
    cerr << e.what() << endl;
    return 1;
  }
}
