#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>

#include "bytevector.h"

using std::vector;
using std::ifstream;
using std::string;
using std::cout;
using std::endl;

int main() {
  ifstream is("problem8.data");
  string line;
  int row = 0;
  while(is.peek() != EOF) {
    getline(is, line);
    vector<string> chunks;
    for (unsigned i = 0; i < line.size(); i += 16) {
      chunks.push_back(line.substr(i, 16));
    }
    
    std::unordered_multiset<string> strings;
    for (string s : chunks) {
      strings.insert(s);
    }

    for (string s : chunks) {
      if (strings.count(s) > 1) {
        cout << row << endl;
        break;
      }
    }
    row++;
  }
  return 0;
}
