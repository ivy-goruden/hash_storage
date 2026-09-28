#include <string>
#include <optional>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;
class Parser{
    public:
        static vector<string> parseLine(std::string line){
            vector<string> params;
            istringstream f(line);
            string s;

            while (getline(f, s, ' ')) {
                if (s == "EX"){
                    if(getline(f, s, ' ')){
                        params.push_back(s); //pushing back TTL
                    }
                }else{
                    params.push_back(s);
                }
            }
            return params;
        }
        
};