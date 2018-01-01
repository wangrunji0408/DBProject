#include <iostream>
#include "driver/InteractiveDriver.h"

int main(int argc,char** argv){
	InteractiveDriver(::std::cin,::std::cout,true).start();
	return 0;
}