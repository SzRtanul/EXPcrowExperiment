#include <iostream>

using namespace std;

struct WordsCompare{
	int szakaszSzam;
	char leghosszabbSzo;
	int szavakSzama;
//Betűk jelölése oszloponként
	int* bynarytree;
//Honnan
//	int* bywoherin; // Inicializálás binarytree*32, Csökkentés: Ahány betű
//	char* byoszlop;
//VégsőÉrték
	char* vegsoErtekOszlopSzam; // Inicializálás a szavak számával
	int* vegsoErtek; //3000
// Oszlopszamszakasz
	char* szakaszOszlopSzam;
	int* kezd;



	WordsCompare(char leghosszabbSzo, int szavakSzama, int szakaszSzam){
		std::cout << "DEFIN: " << leghosszabbSzo << ":" << szavakSzama << endl;
		this->szakaszSzam = szakaszSzam;
		this->leghosszabbSzo = leghosszabbSzo;
		this->szavakSzama = szavakSzama;
        bynarytree = new int[leghosszabbSzo]();
//        bywoherin = new int[leghosszabbSzo * 32]();
//        byoszlop = new char[leghosszabbSzo * 32]();
        vegsoErtek = new int[szavakSzama]();
        vegsoErtekOszlopSzam = new char[szavakSzama]();
		szakaszOszlopSzam = new char[szakaszSzam]();
		kezd = new int[szakaszSzam]();
    }

	~WordsCompare(){
        /*
		delete[] bynarytree;
        delete[] bywoherin;
        delete[] byoszlop;
        delete[] vegsoErtek;
        delete[] vegsoErtekOszlopSzam;
		*/
    }
};


inline bool whatIsChar(char character){
	bool vmia = ((unsigned)(character - 65) < (91- 65)) || character == '_';
	return vmia;
}

WordsCompare doSyntaxtCheckPreparation(const char* characterChain){	
	// Segéd
	int szakaszSzam = 0;
	char currentChar = '\0';

	char lastszamlal = 0;
	char szamlal = 0;
	char lastChar = ';';
	// Utófeltétel
	int szavakSzama = 0; 
	char leghosszabbSzo = 0;

	int i = 0;
	for(; characterChain[i] != '\0'; i++){
		if((lastChar != ';' && (characterChain[i] == ';') || (lastChar != ';' && characterChain[i+1] == '\0'))){
			szavakSzama++;
			leghosszabbSzo = szamlal > leghosszabbSzo ? szamlal : leghosszabbSzo;
			if(lastszamlal != szamlal){
				szakaszSzam++;
				lastszamlal = szamlal;
			}
			szamlal = 0;
		}
		currentChar = characterChain[i] > 96 && characterChain[i] < 123 ? characterChain[i] - 32 : characterChain[i];
		if(whatIsChar(currentChar)){
			if(szamlal == 127) return WordsCompare(0, 0, 0);
			szamlal++;
		}
		if(whatIsChar(currentChar) && lastChar == ';'){
			lastChar = currentChar;
		}
	}
	std::cout<<characterChain << endl;
	std::cout << leghosszabbSzo << " : " << szavakSzama << endl;
	std::cout << "SzakaszSzám: " << szakaszSzam << endl;
	WordsCompare wordsCompare(leghosszabbSzo, szavakSzama, szakaszSzam);
	
	//Reinit
	szamlal = 0;
	szavakSzama = 0;
	lastChar = ';';
	int position = 0;
	int lastCharPosition = 0;
	int wordValue = 0;
	szakaszSzam = 0;
	i = 0;
	for(; characterChain[i] != '\0'; i++){
		currentChar = characterChain[i] > 96 && characterChain[i] < 123 ? characterChain[i] - 32 : characterChain[i];
		if(whatIsChar(currentChar)){
			position = currentChar - 65; // zyxwvutsrqponmlkjihgfedcba
			wordValue += (31 * szamlal) + position;
			std::cout << "WordValue(Pre): " << wordValue << " - " << i << endl;
			wordsCompare.bynarytree[szamlal] |= 1 << position;
//			if(lastChar != ';') wordsCompare.bywoherin[(szavakSzama * 31) + position] |= 1 << lastCharPosition; //abcdefghijkmnopqrstuvwqyz
			szamlal++;
		}
		std::cout << "LastChar: " << lastChar << " Aktuel: " << currentChar << endl;
		if((lastChar != ';' && (currentChar == ';') || (lastChar != ';' && characterChain[i+1] == '\0'))){ 
			std::cout << "WordValue(Pret): " << wordValue << endl;
			wordsCompare.vegsoErtek[szavakSzama] = wordValue;
			std::cout << "Szavak száma: " << szavakSzama << endl; 
			wordsCompare.vegsoErtekOszlopSzam[szavakSzama] = (char)szamlal;

			if(lastszamlal != szamlal){
				wordsCompare.kezd[szakaszSzam] = szavakSzama;
				wordsCompare.szakaszOszlopSzam[szakaszSzam] = szamlal;
				lastszamlal = szamlal;
				szakaszSzam++;
			}
			szavakSzama++;
			szamlal = 0;
			wordValue = 0;
		}
		if(whatIsChar(currentChar) || currentChar == ';'){ 
			lastChar = currentChar;
		}
	}
	for(int k = 0; k < wordsCompare.szavakSzama; k++){
		std::cout << k << ".: " << wordsCompare.vegsoErtek[k] << " VégsőértékOszlop: " << (unsigned)wordsCompare.vegsoErtekOszlopSzam[k] << std::endl;
	}
	return wordsCompare;
}
