

struct StoreNames{
	int length;
	int* sepIndexes;
	char* characterChain;

	StoreNames(const char* characterChain){
		this->characterChain = strdup(characterChain);
		this->length = this->getSepNumber();
		sepIndexes = new int[this->length]();
		initSepIndexes();
	}

	~StoreNames(){
		delete[] sepIndexes;
	}

	int getSepNumber(){
		int sepNumberSum = 0;
		char lastChar = ';';
		char currentchar = '\0';
		int i = 0;
		for(; characterChain[i] != '\0'; i++){
			characterChain[i] = characterChain[i] > 96 && characterChain[i] < 123 ? characterChain[i] - 32 : characterChain[i];
			if(lastChar == ';' && (currentchar > 64 && currentchar < 91)) sepNumberSum++; 
			if(currentchar == ';' || (currentchar > 64 && currentchar < 91)) lastChar = currentchar;
			else if(currentchar != '_') characterChain[i] = '_';
		}
		return sepNumberSum;
	}

	void initSepIndexes(){
		int sepPosition = 0;
		char lastChar = ';';
		int i = 0;
		for(; characterChain[i] != '\0'; i++){
			 if(lastChar == ';' && (characterChain[i] > 64 && characterChain[i] < 91)){
				sepIndexes[sepPosition] = i;
				sepPosition++;
			 }
			 if(characterChain[i] == ';' || (characterChain[i] > 64 && characterChain[i] < 91)) lastChar = characterChain[i]; 
		}
	}
};


