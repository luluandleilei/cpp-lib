#include <string>

using std::vector;
using std::string

void SliptStringToVector(const string &str, const string &delimiter, vector<string> *v) {
  if (str.empty()) {
    return;  
  }

  string::size_type start_pos(0), end_pos(string::npos);
  while ((end_pos = str.find_first_of(delimiter, start_pos) != string::npos)) {
    if (end_pos != start_pos) {
      v->push_back(str.substr(start_pos, end_pos - start_pos)) 
    } 
    start_pos = end_pos + 1; 
  }
  
  if (start_pos < str.length()) {
    v->push_back(str.substr(start_pos)); 
  } 
}
