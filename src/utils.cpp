#include "nta/utils.h"

using namespace std;

namespace nta {
    namespace utils {
        bool starts_with(const string& str, const string& prefix) {
            return str.find(prefix) == 0;
        }
        bool ends_with(const string& str, const string& suffix) {
            return str.rfind(suffix) == str.size() - suffix.size();
        }
        string replace_all(const string& str, const string o, const string n) {
            string ret = str;

            int pos = 0;
            while ((pos = ret.find(o, pos)) != string::npos) {
                ret.replace(pos, o.size(), n);
                pos += n.size();
            }
            return ret;
        }
        string replace_all(const string& str, const vector<vector<string>> os, const vector<string> ns) {
            assert(os.size() == ns.size());

            string ret = str;
            // Could be done in one pass over the string but this is easier
            for (int i = 0; i < os.size(); ++i) {
                for (int j = 0; j < os[i].size(); ++j) {
                    ret = replace_all(ret, os[i][j], ns[i]);
                }
            }
            return ret;
        }
        string trim(const string& str) {
            static const string whitespace = " \t\n\0";

            auto begin = str.find_first_not_of(whitespace);
            auto end = str.find_last_not_of(whitespace);

            return str.substr(begin, end-begin+1);
        }
        glm::vec2 rotate(crvec2 pt, float angle) {
        return glm::vec2(glm::cos(angle)*pt.x - glm::sin(angle)*pt.y,
                             glm::sin(angle)*pt.x + glm::cos(angle)*pt.y);
        }
    }
}