#include "nta/GLMConsoleOutput.h"

std::ostream& operator<<(std::ostream& lhs, crvec2 rhs) {
    lhs<<"<"<<rhs.x<<", "<<rhs.y<<">";
    return lhs;
}

std::ostream& operator<<(std::ostream& lhs, crvec4 rhs) {
    lhs<<"<"<<rhs.x<<", "<<rhs.y<<", "<<rhs.z<<", "<<rhs.w<<">";
    return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const glm::mat3& rhs) {
    lhs<<"{ ";
    for (int i = 0; i < 3; i++) {
        if (i != 0) {
            lhs<<"  ";
        }
        lhs<<rhs[i][0]<<", "<<rhs[i][1]<<", "<<rhs[i][2];
        if (i == 2) {
            lhs<<" }";
        } else {
            lhs<<std::endl;
        }
    }
    return lhs;
}
