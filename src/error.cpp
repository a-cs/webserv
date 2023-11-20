#include "../include/error.hpp"

Error::Error() {
    this->onError = false;
}

Error::~Error() {}

void Error::clear() {
    this->msg.clear();
    this->onError = false;
}
