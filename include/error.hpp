#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>

class Error {
	public:
		bool		onError;
		std::string	msg;

		Error();
		~Error();

		void	clear();
};

#endif
