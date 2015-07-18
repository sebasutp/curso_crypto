
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/multi_array.hpp>
#include "../encoding.h"

using namespace std;
using namespace boost::program_options;
using namespace encoding;

typedef long long int LL;

int main(int argc, char **argv) {
  try {
    options_description desc("Allowed options");
    string alphabet, out_file, in_file;
    int laplace_smooth = 0;
    char letter_case = 'n';
    bool ignore_unknown = false;
    desc.add_options()
      ("help,h", "print help message")
      ("alphabet,a", value(&alphabet), "Text file containing the alphabet")
      ("outfile,o", value(&out_file), "Ouput file")
      ("infile,i", value(&in_file), "Input file (Some text in target language)")
      ("letter_case,c", value(&letter_case), "Set to u for upper-case or l for lower-case")
      ("smooth,s", value(&laplace_smooth), "Set the initial counts to the given value (Laplace smooth)")
      ("ignore", bool_switch(&ignore_unknown), "Ignore unknown symbols of the alphabet");

    variables_map vm;
    store(parse_command_line(argc,argv,desc), vm);
    notify(vm);
    if (vm.count("help")) {
      cout << desc << endl;
      return 0;
    }
    if (vm.count("infile")) {
      /*ifstream in(in_file);
      cin.rdbuf(in.rdbuf());*/
      freopen(in_file.c_str(), "r", stdin);
    }

    Mapper m(alphabet);
    boost::multi_array<int,2> matrix(boost::extents[m.size()][m.size()]);
    string line;
    LL N = laplace_smooth*((LL)m.size())*m.size();
    while (getline(cin, line)) {
      switch(letter_case) {
        case 'u':
          boost::to_upper(line);
          break;
        case 'l':
          boost::to_lower(line);
          break;
        case 'n':
          //do nothing
          break;
        default:
          cerr << "Unknown case option " << letter_case << endl;
          return 1;
      }
      vector<uint32_t> chars = m.stringToInts(line, ignore_unknown);
      for (int i=1; i<chars.size(); i++) {
        matrix[chars[i-1]][chars[i]]++;
        N++;
      }
    }

    if (vm.count("outfile")) {
      /*ofstream out(out_file);
      cout.rdbuf(out.rdbuf());*/
      freopen(out_file.c_str(), "w", stdout);
    }
    for (int i=0; i<m.size(); i++) {
      for (int j=0; j<m.size(); j++) {
        cout << ((double)matrix[i][j] / (double)N) << " ";
      }
      cout << endl;
    }
  } catch (exception &e) {
    cerr << e.what() << endl;
  }
}
