#include "nta/utils.h"

namespace nta {
    namespace utils {
        bool starts_with(crstring str, crstring prefix) {
            return str.find(prefix) == 0;
        }
        bool ends_with(crstring str, crstring suffix) {
            return str.size() >= suffix.size() && str.rfind(suffix) == str.size() - suffix.size();
        }
        std::string replace_all(crstring str, crstring o, crstring n) {
            std::string ret = str;

            int pos = 0;
            while ((pos = ret.find(o, pos)) != std::string::npos) {
                ret.replace(pos, o.size(), n);
                pos += n.size();
            }
            return ret;
        }
        std::string replace_all(crstring str, const std::vector<std::vector<std::string>>& os, const std::vector<std::string>& ns) {
            assert(os.size() == ns.size());

            std::string ret = str;
            // Could be done in one pass over the string but this is easier
            for (int i = 0; i < os.size(); ++i) {
                for (int j = 0; j < os[i].size(); ++j) {
                    ret = replace_all(ret, os[i][j], ns[i]);
                }
            }
            return ret;
        }
        std::string trim(crstring str, crstring back_trash, crstring front_trash) {
            auto begin = str.find_first_not_of(front_trash);
            auto end = str.find_last_not_of(back_trash);

            return str.substr(begin, end-begin+1);
        }
        std::vector<std::string> split(crstring str, char delim) {    
            std::vector<std::string> items; 
            std::stringstream ss(str);  
            std::string item;   
            while (std::getline(ss, item, delim)) { 
                items.push_back(item);  
            }   
            return items;   
        }
        glm::vec2 rotate(crvec2 pt, float angle) {
        return glm::vec2(glm::cos(angle)*pt.x - glm::sin(angle)*pt.y,
                             glm::sin(angle)*pt.x + glm::cos(angle)*pt.y);
        }
    }
}