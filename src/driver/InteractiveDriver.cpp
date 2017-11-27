#include <string>
#include <istream>
#include <ostream>
#include "driver/SQLExecutor.h"
#include "driver/InteractiveDriver.h"
#include "parser/Parser.h"

void InteractiveDriver::start(){
	while(!(input.eof()||input.fail()||input.bad())){
		::std::string line;
		::std::getline(input,line);
		if(line.back()!='\\'){
			executer.executeSQL(Parser::parseString(buffer+line),output);
			buffer.clear();
		}else{
			line.pop_back();
			buffer+=line;
		}
	}
}