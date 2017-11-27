#ifndef INTERACTIVEDRIVER_H
#define INTERACTIVEDRIVER_H

#include <string>
#include <istream>
#include <ostream>
#include "driver/SQLExecutor.h"

class InteractiveDriver{
	::std::istream& input;
	::std::ostream& output;
	SQLExecutor executer;
	::std::string buffer;
public:
	InteractiveDriver(::std::istream& input,::std::ostream& output):input(input),output(output){}
	void start();
};

#endif //INTERACTIVEDRIVER_H