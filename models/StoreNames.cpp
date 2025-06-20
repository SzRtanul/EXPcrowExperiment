

struct StoreNames{
	int spellNumber;
	int length;
	int glength;
	int* sepIndexes;
	int* groupIndexes;
	char* characterChain;

	StoreNames(const char* characterChain){
		std::cout << "CARA: " <<  characterChain << std::endl;
		this->getSepNumber(characterChain);
	}

	~StoreNames(){
//		delete[] sepIndexes;
	}

	void getSepNumber(const char* characterChain){
		int gNumberSum = 0;
		int sepNumberSum = 0;
		char lastChar = '@';
		char currentchar = '\0';
		std::cout << "Null:: " << (int)currentchar << std::endl;
		char vizsLastChar = 0;

		int i = 0;
		for(; (unsigned)characterChain[i] > 1; i++){
			std::cout<< "OfFar(" << i << "): " << (unsigned)characterChain[i] << ":" << characterChain[i] << std::endl;
			vizsLastChar = (unsigned)lastChar - 63;
			currentchar = (unsigned)characterChain[i] - 97 < 26 ? characterChain[i] - 32 : characterChain[i];
			std::cout << "Vizsla: " << vizsLastChar << ":" << (unsigned)vizsLastChar << std::endl;
			if(vizsLastChar < 2 && ((unsigned)currentchar - 64 < 27)){
				sepNumberSum++;
				if(vizsLastChar) gNumberSum++;
			}
			if((unsigned)currentchar - 63 < 28) lastChar = currentchar;
		}
		this->spellNumber = i;
		std::cout << "SPELLNUMBER: " << this->spellNumber << std::endl;
		std::cout << "SepNumberSum: " << sepNumberSum << std::endl; 
		sepIndexes = new int[sepNumberSum];
		std::cout << "ohh" << std::endl; 
		groupIndexes = new int[gNumberSum];
		std::cout << "ohh" << std::endl; 
		this->length = sepNumberSum;
		std::cout << "ohh(l): " << this->length << std::endl; 
		this->glength = gNumberSum;
		std::cout << "ohh(g): " << this->glength << std::endl; 

		// Helyek beszámozása
		this->characterChain = new char[i+1];
		gNumberSum = 0;
		sepNumberSum = 0;

		lastChar = '@';
		currentchar = '\0';
		vizsLastChar = 0;

		i = 0;
		for(; (unsigned)characterChain[i] > 1; i++){
			 vizsLastChar = (unsigned)lastChar - 63;
//			 std::cout << "Craq: " <<  characterChain[i] << ":" << (unsigned)currentchar << std::endl;
			 currentchar = (unsigned)characterChain[i] - 97 < 26 ? characterChain[i] - 32  : characterChain[i];

			 
			 if(vizsLastChar < 2 && ((unsigned)currentchar - 64 < 27)){
				if(vizsLastChar){
					this->groupIndexes[gNumberSum] = sepNumberSum;
					gNumberSum++;
				}
				this->sepIndexes[sepNumberSum] = i;
				sepNumberSum++;
			 }
//			 std::cout << "Ellőtt: " <<  currentchar << ":" << (unsigned)currentchar << std::endl;
			 if((unsigned)currentchar - 63 < 28) lastChar = currentchar;
			 else if(currentchar != '_') currentchar = '_';
//			 std::cout << "Után: " <<  currentchar << ":" << (unsigned)currentchar << std::endl;
			 this->characterChain[i] = currentchar;
		}
		this->characterChain[i] = '\0';
		std::cout << "CARAn: " <<  this->characterChain << std::endl;
		std::cout << "ohha" << std::endl; 
	}
};
