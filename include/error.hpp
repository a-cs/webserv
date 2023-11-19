#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>

class Error {
    private:

    public:
        Error();
        ~Error();
        void clear();

        std::string msg;
        bool onError;
};

#endif
