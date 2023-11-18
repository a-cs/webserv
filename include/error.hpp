#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>

class Error {
    private:

    public:
        Error();
        ~Error();

        std::string msg;
        bool onError;
};

#endif
