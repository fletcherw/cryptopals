#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>

#include "bytevector.h"

using std::string;

int main() {
  std::ifstream in_file("data/problem08.data");
  string line;
  int row = 0;
  while(in_file.peek() != EOF) {
    getline(in_file, line);
    std::vector<string> chunks;
    for (unsigned i = 0; i < line.size(); i += 16) {
      chunks.push_back(line.substr(i, 16));
    }

    std::unordered_multiset<string> strings;
    for (string s : chunks) {
      strings.insert(s);
    }

    for (string s : chunks) {
      if (strings.count(s) > 1) {
        std::cout << row << std::endl;
        break;
      }
    }
    row++;
  }
  return 0;
}
