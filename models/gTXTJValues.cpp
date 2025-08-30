#include <iostream>


using namespace std;

inline bool isSmallCharToo(char character){
	return (((unsigned)character - 65) < (123 - 65)) && !(((unsigned) character - 91) < (97 - 91));
}

inline std::string getTextWithJSONValues(
		const WordsCompare wordsCompare, 
		StoreNames storeNames, /*const std::string*/ 
		const crow::json::rvalue JSONValuesString, 
		const char* text
){
	std::string retn = "";
	retn.reserve(strlen(text) * 3 / 2);
	int szamlal = 0;
	int position = 0;
	bool syntaxtGood = true;
	char currentChar = '\0';
	char lastChar = ';';
	int wordValue = 0;

	int usedStoreNames = -1;
	auto JSONValues = JSONValuesString;//crow::json::load(JSONValuesString);
	int i = 0;
	std::cout << "Még megyen" << endl;
	while(text[i] != '\0' && syntaxtGood == true){
		std::cout << "Meddig1" << endl;
//		std::cout << "Még megyen: " << i << endl;
		currentChar = text[i] > 96 && text[i] < 123 ? text[i] - 32 : text[i];
		std::cout << "Meddig2" << endl;
		std::cout << "Karakter: " << text[i] << " Max: " << (int)wordsCompare.leghosszabbSzo << " Szamlal: " << szamlal << endl;

//		 std::cout << wordsCompare.leghosszabbSzo << endl;
//		 std::cout << wordsCompare.bynarytree[0] << endl;
		std::cout << whatIsChar(currentChar);
		if(whatIsChar(currentChar)){
			std::cout << "Meddig3" << endl;
//		 	std::cout << wordsCompare.leghosszabbSzo << endl;
//		 std::cout << wordsCompare.bynarytree[0] << endl;
//			haved = true;
			position = currentChar - 65;
			std::cout << "Meddig4" << endl;
			
		 	std::cout << (int)wordsCompare.leghosszabbSzo << endl;
			std::cout << "Számlál: " << szamlal << endl;
			syntaxtGood = szamlal < wordsCompare.leghosszabbSzo && ((wordsCompare.bynarytree[szamlal] >> (position)) & 0b1);
			std::cout << "Meddig5" << endl;
			if(/*syntaxtGood*/szamlal < wordsCompare.leghosszabbSzo) std::cout << "Számláló: " << currentChar <<
					" - " << std::bitset<32>(wordsCompare.bynarytree[szamlal]) <<
					" >> " << currentChar - 65 << " 0b " << 
					std::bitset<32>((wordsCompare.bynarytree[szamlal] >> (position)) & 0b1) <<
			endl;
//			std::cout << "Syn.: " << syntaxtGood << endl;

			//if(szamlal > 0) syntaxtGood = wordsCompare.bywoherin[((szamlal - 1) * 26) + position] >> lastChar - 65;
//			std::cout << "Még megyen: " << i << endl;
			wordValue += ((szamlal) * 31) + position;
			szamlal++;
			//std::cout << text[i] << ":" << (int)text[i] << std::endl;
			// Bináris fa összehasonlítás
//			std::cout << "Syn.: " << syntaxtGood << endl;
		}
//		std::cout << "Synitty: " << syntaxtGood << endl;
		
		if((text[i+1] == '\0' && (whatIsChar(lastChar) || whatIsChar(currentChar))) ||
						(!(whatIsChar(currentChar)) && whatIsChar(lastChar))
		){
			syntaxtGood = false;
			int j = 0;
			for(;j < wordsCompare.szakaszSzam && !syntaxtGood; j++){
std::cout << "ehj" << wordsCompare.szakaszOszlopSzam[j] << " szamlal: " << (int)szamlal <<  std::endl;
				syntaxtGood = wordsCompare.szakaszOszlopSzam[j] == szamlal;
			}
			j--;	
		
			for(int k = 0; k < wordsCompare.szavakSzama; k++){
//	std::cout << k << ".: " << wordsCompare.vegsoErtek[k] << " VégsőértékOszlop: " << (unsigned)wordsCompare.vegsoErtekOszlopSzam[k] << std::endl;
			}
			for(int k = 0; k < wordsCompare.szakaszSzam; k++){
//std::cout << k << ".: " << wordsCompare.kezd[k] << " VégsőértékOszlop: " << (unsigned)wordsCompare.szakaszOszlopSzam[k] << std::endl;
			}
			
std::cout << "jkezd: " << j << " számlál: " <<  endl;
std::cout << "WordValue: " << wordValue << endl;
			int vege = j + 1 < wordsCompare.szakaszSzam ? wordsCompare.kezd[j + 1] : wordsCompare.szavakSzama;
			j = syntaxtGood ? wordsCompare.kezd[j] : wordsCompare.szavakSzama;
			syntaxtGood = false;
			
			std::cout << "j: " << j << "Vege: " << vege << std::endl;
			for(/*Kezdőérték*/; j < vege && !syntaxtGood; j++){
				syntaxtGood = wordsCompare.vegsoErtek[j] == wordValue;
				//std::cout << "VegsoErtek: " << wordsCompare.vegsoErtek[j] << endl;
			}
std::cout << "wV:" << wordValue << endl;
std::cout << "szSz:" << wordsCompare.szakaszSzam << endl;
			szamlal = 0;
			wordValue = 0;
			std::cout << "Syne: " << syntaxtGood << endl;
			//if(text[i] != ' ') retn += ' ';
		}
//std::cout << "Synitt: " << syntaxtGood << endl;
//std::cout << "Még megyen: " << i << endl;
std::cout<< text[i] << endl;
		if(text[i] == '#'){
			i++;
std::cout<< text[i] << "ALAAAAA " << syntaxtGood << endl;
			bool addValue = false;
			std::string field = "";
			char ohne = (unsigned)(text[i] - 36);
			usedStoreNames = ohne < 5 && ohne < storeNames.glength ? ohne : -1;
			if(text[i] == '-'){
				std::cout<< text[i] << endl;
				if(!JSONValues){ 
					std::cout << "Szívás a javából:";
					syntaxtGood = false;
				}
//				syntaxtGood = static_cast<bool>(JSONValues); // MemoryErrorSource
				addValue = true;
				i++;
			}
			std::cout<< text[i] << endl;
			if(usedStoreNames!=-1){
				int wheres = -1;
				i++;
std::cout << (int)text[i] << " JELLLLL "<< endl;
				if(!addValue){
					while((unsigned)text[i] - 48 < (58 - 48)){
						field += text[i];
						i++;
					}
std::cout << "Még megyeni: " << i << endl;
					if(field.length() > 0){
std::cout << "ANE: " << usedStoreNames <<  std::endl;
						wheres = std::stoi(field);
std::cout << "Wheres?:SepIndexes : " << wheres << ":" << storeNames.length << endl;
						char ohigen = (storeNames.groupIndexes[usedStoreNames] + (unsigned)wheres);
						syntaxtGood = ohigen < storeNames.length;
						if(syntaxtGood){
							int j = storeNames.sepIndexes[ohigen];
							std::cout << "J?: " << j << endl;
							if(usedStoreNames == 4) retn += "p9_";
							else if(usedStoreNames > 0 && lastChar != '.') retn += '.';
							for(; (unsigned)storeNames.characterChain[j] - 63 > 1 && (unsigned)storeNames.characterChain[j] > 1; j++){
std::cout << storeNames.characterChain[j] << endl;
								retn += storeNames.characterChain[j]; 
							}
						//	retn += text[i];
							i--;
std::cout << retn << endl;
						}
std::cout << "Még megyeni: " << i << std::endl;
					}
std::cout << "Még megyeni: " << i << std::endl;
				}
			}
			else if(addValue && syntaxtGood){
std::cout << (int)text[i] - 65 << " JELLLLL "<< endl;
				while(((unsigned)(text[i] - 65) < (91 - 65) || (unsigned)(text[i] - 97) < (123-97))){
					field += text[i];
					i++;
				}
std::cout << field << endl;
				if(field.length() > 0 && JSONValues.has(field)){
					syntaxtGood = JSONValues.has(field);
					if(JSONValues[field].t() == crow::json::type::String){
						const char* JSONSTR = std::string(JSONValues[field].s()).c_str();
						std::string::size_type pos = 0;
						std::string s = ""; 
						for(int j = 0; JSONSTR[j] != '\0'; j++) {
							s += JSONSTR[j];
							if(JSONSTR[j] == '\'') s += '\'';
							if(j > 1 && isSmallCharToo(JSONSTR[j-1]) && !isSmallCharToo(JSONSTR[j])) s += '$';
						}
//						JSONSTR = s;
						retn += "'"+s+"'";
					}
					else if(syntaxtGood){
std::cout << "OOOOOOOOOOOOOO" << endl;
//						std::string szamStr = (JSONValues[field].nt() == crow::json::num_type::Signed_integer) ? 
//								std::to_string(JSONValues[field].i()) : std::to_string(JSONValues[field].d());

						crow::json::wvalue myOb(JSONValues[field]);
						retn += myOb.dump();
					}
					i--;
				}
			}
			else{
				retn += "#";
			}
		}
		else{
//			std::cout << "Synitt: " << syntaxtGood << endl;
			retn += text[i];
		}
//		std::cout << "Synitt: " << syntaxtGood << endl;
		lastChar = currentChar;
		/*if(text[i] != '\0')*/ i++;
//		std::cout << "Synitt: " << syntaxtGood << endl;
	}
//	std::cout << "Syny.: " << syntaxtGood << endl;
std::cout<<"-------------\n" << retn << std::endl; 
	if(!syntaxtGood) retn = "-";
	return retn;
};

