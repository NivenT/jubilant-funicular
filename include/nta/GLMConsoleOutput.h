#ifndef GLMCONSOLEOUTPUT_H_INCLUDED
#define GLMCONSOLEOUTPUT_H_INCLUDED

#include <iostream>

#include "nta/MyEngine.h"

/// functions for writing vectors and matrices to streams (Ex. std::cout<<glm::vec2(0,1);)
extern std::ostream& operator<<(std::ostream&, crvec2);
extern std::ostream& operator<<(std::ostream&, crvec4);
extern std::ostream& operator<<(std::ostream&, const glm::mat3&);


#endif // GLMCONSOLEOUTPUT_H_INCLUDED
