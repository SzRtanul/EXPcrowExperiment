#include "test.h"

int main(){
	std::cout << "Élien" << std::endl;
	int i = 0;
	std::string text = "unult=f\x02\x00\x00\x00 0";
	std::cout << "Origin: " << text << std::endl;
//	std::cout << getUpdateSets(i, text) << std::endl;
	std::cout << "Ez működik." << std::endl;
	
	i = 0;
	text = "f\x03\x00\x00\x00nev,\x05\x00\x00\x00 atle,\x05\x00\x00\x00 atla";
	std::cout << "Origin: " << text << std::endl;
//	std::cout << insertValues(i, text) << std::endl;
	return 0;
}
