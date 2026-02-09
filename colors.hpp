#include <iostream>


namespace Color {
    inline std::ostream & green(std::ostream &s) {return s << "\033[34m";}
    inline std::ostream & yellow(std::ostream &s) {return s << "\033[33m";}
    inline std::ostream & reset(std::ostream &s) {return s << "\033[0m";}
}