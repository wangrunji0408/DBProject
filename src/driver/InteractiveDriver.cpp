#include <string>
#include <istream>
#include <ostream>
#include "driver/SQLExecutor.h"
#include "driver/InteractiveDriver.h"
#include "parser/Parser.h"
#include "ast/Exceptions.h"

void InteractiveDriver::start(){
	if(prompt){
		output<<"> ";
	}
	while(!(input.eof()||input.fail()||input.bad())){
		::std::string line;
		::std::getline(input,line);
		if(line.back()!='\\'){
			::std::vector<::std::unique_ptr<Statement>> program;
			try{
				executer.executeSQL(Parser::parseString(buffer+line),output);
			}catch(const ParseError& error){
				output<<"Error when parsing command:\n"<<error.what()<<::std::endl;
			}
			if(prompt){
				output<<"> ";
			}
			buffer.clear();
		}else{
			line.pop_back();
			line.push_back('\n');
			buffer+=line;
			if(prompt){
				output<<"... ";
			}
		}
	}
}