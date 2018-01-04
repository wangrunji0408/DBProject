#include <string>
#include <fstream>
#include "driver/SQLExecutor.h"
#include "driver/InteractiveDriver.h"
#include "parser/Parser.h"
#include "ast/Exceptions.h"

// trim from start (in place)
static inline void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}

void InteractiveDriver::start(){
	if(prompt){
		output<<"> ";
	}
	while(!(input.eof()||input.fail()||input.bad())){
		::std::string line;
		::std::getline(input,line);
		if(line.back()!='\\'){
			buffer+=line;
			string command;
			if(buffer=="exit"){
				return;
			} else if(buffer.substr(0, 4) == "load") {
				auto path = buffer.substr(4);
				trim(path);
				try {
					auto fin = std::fstream(path);
					std::getline(fin, command, '\0');
				} catch (std::exception const& e) {
					output << "Error when read file:\n" << e.what() << endl;
				}
			} else {
				command = buffer;
			}
			execute(command);
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

void InteractiveDriver::execute(std::string const &cmd) {
	try{
		executer.executeSQL(Parser::parseString(cmd),output);
	}catch(const ParseError& error){
		output<<"Error when parsing command:\n"<<error.what()<<::std::endl;
	}catch(ExecuteError const& error) {
		output<<"Error when execute command:\n"<<error.what()<<::std::endl;
	}
}
