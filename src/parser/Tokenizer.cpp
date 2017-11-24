#include <string>
#include <iostream>
#include "Tokenizer.h"
Token Tokenizer::next(){
	while(true){
		if(pos>=text.length()){
			return {TokenType::EOS};
		}
		if(::std::isspace(text[pos])){
			pos++;
			continue;
		}
		if(text[pos]=='\''){
			size_t pos2=pos+1;
			::std::string content="";
			while(pos2<text.length()){
				if(text[pos2]=='\''){
					pos2++;
					if(pos2>=text.length()||text[pos2]!='\''){
						pos=pos2;
						return {TokenType::STRING,0,0,content};
					}
				}
				content+=text[pos2];
				pos2++;
			}
			throw std::runtime_error("Unexpected terminal of string literal");
		}
		if(text[pos]=='-'){
			if(pos+1<text.length()&&text[pos+1]=='-'){
				pos+=2;
				while(pos<text.length()){
					if(text[pos]=='\n'){
						pos++;
						break;
					}
					pos++;
				}
				continue;
			}
		}
		if(text[pos]=='/'){
			if(pos+1<text.length()&&text[pos+1]=='*'){
				pos+=2;
				while(pos<text.length()){
					if(text[pos]=='*'){
						pos++;
						if(pos>=text.length()||text[pos]=='/'){
							pos++;
							break;
						}
					}
					pos++;
				}
				continue;
			}
		}
		if(::std::isdigit(text[pos])||text[pos]=='.'){
			size_t pos2=pos;
			::std::string content="";
			bool isFloat=false;
			while(pos2<text.length()){
				if(::std::isdigit(text[pos2])){
					content+=text[pos2];
					pos2++;
					continue;
				}
				if(text[pos2]=='.'){
					if(isFloat){
						throw std::runtime_error("Illegal numeric literal");
					}else{
						isFloat=true;
					}
					content+=text[pos2];
					pos2++;
					continue;
				}
				break;
			}
			pos=pos2;
			if(isFloat){
				return {TokenType::FLOAT,0,::std::stod(content)};
			}else{
				return {TokenType::INT,::std::stoi(content)};
			}
		}
		if(::std::isalpha(text[pos])||text[pos]=='_'){
			size_t pos2=pos;
			::std::string content="";
			while(pos2<text.length()){
				if(::std::isdigit(text[pos2])||::std::isalpha(text[pos2])||text[pos2]=='_'){
					content+=::std::tolower(text[pos2]);
					pos2++;
					continue;
				}
				break;
			}
			pos=pos2;
			if(content=="and"){
				return{TokenType::K_AND};
			}else if(content=="create"){
				return{TokenType::K_CREATE};
			}else if(content=="database"){
				return{TokenType::K_DATABASE};
			}else if(content=="databases"){
				return{TokenType::K_DATABASES};
			}else if(content=="date"){
				return{TokenType::K_DATE};
			}else if(content=="delete"){
				return{TokenType::K_DELETE};
			}else if(content=="desc"){
				return{TokenType::K_DESC};
			}else if(content=="drop"){
				return{TokenType::K_DROP};
			}else if(content=="float"){
				return{TokenType::K_FLOAT};
			}else if(content=="foreign"){
				return{TokenType::K_FOREIGN};
			}else if(content=="from"){
				return{TokenType::K_FROM};
			}else if(content=="index"){
				return{TokenType::K_INDEX};
			}else if(content=="insert"){
				return{TokenType::K_INSERT};
			}else if(content=="int"){
				return{TokenType::K_INT};
			}else if(content=="into"){
				return{TokenType::K_INTO};
			}else if(content=="is"){
				return{TokenType::K_IS};
			}else if(content=="key"){
				return{TokenType::K_KEY};
			}else if(content=="not"){
				return{TokenType::K_NOT};
			}else if(content=="null"){
				return{TokenType::K_NULL};
			}else if(content=="primary"){
				return{TokenType::K_PRIMARY};
			}else if(content=="references"){
				return{TokenType::K_REFERENCES};
			}else if(content=="select"){
				return{TokenType::K_SELECT};
			}else if(content=="set"){
				return{TokenType::K_SET};
			}else if(content=="show"){
				return{TokenType::K_SHOW};
			}else if(content=="table"){
				return{TokenType::K_TABLE};
			}else if(content=="tables"){
				return{TokenType::K_TABLES};
			}else if(content=="update"){
				return{TokenType::K_UPDATE};
			}else if(content=="use"){
				return{TokenType::K_USE};
			}else if(content=="values"){
				return{TokenType::K_VALUES};
			}else if(content=="varchar"){
				return{TokenType::K_VARCHAR};
			}else if(content=="where"){
				return{TokenType::K_WHERE};
			}else{
				return{TokenType::IDENTIFIER,0,0,content};
			}
		}
		if(text[pos]=='('){
			pos++;
			return{TokenType::P_LPARENT};
		}
		if(text[pos]==')'){
			pos++;
			return{TokenType::P_RPARENT};
		}
		if(text[pos]==';'){
			pos++;
			return{TokenType::P_SEMICOLON};
		}
		if(text[pos]==','){
			pos++;
			return{TokenType::P_COMMA};
		}
		if(text[pos]=='='){
			pos++;
			return{TokenType::P_EQ};
		}
		if(text[pos]=='>'){
			pos++;
			if(pos<text.length()){
				if(text[pos]=='='){
					pos++;
					return{TokenType::P_GE};
				}
			}
			return{TokenType::P_G};
		}
		if(text[pos]=='<'){
			pos++;
			if(pos<text.length()){
				if(text[pos]=='='){
					pos++;
					return{TokenType::P_LE};
				}
				if(text[pos]=='>'){
					pos++;
					return{TokenType::P_NE};
				}
			}
			return{TokenType::P_L};
		}
		throw std::runtime_error(::std::string("Unexpected character:")+text[pos]);
	}
}