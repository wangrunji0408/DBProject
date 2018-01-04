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
	bool prompt;
public:
	InteractiveDriver(::std::istream& input,::std::ostream& output,bool prompt=false):input(input),output(output),prompt(prompt){}
	void start();
	void execute(std::string const& cmd);
};

#endif //INTERACTIVEDRIVER_H