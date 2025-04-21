#include "crow.h"
#include <iostream>
#include <cstring>
#include "crow/middlewares/cors.h"
#include <csignal>
#include <pqxx/pqxx>
#include <string>
#include <thread>
#include <chrono>
#include <regex>
#include <crow/json.h>

using namespace std;
using namespace pqxx;
//using json = nlohmann::json;





pqxx::connection C = pqxx::connection(R"(dbname = testdb user=postgres password=test123 hostaddr=127.0.0.1 port=5432)");
int exat=0;
// SzóRaktár
char szoRaktarSQLSyntaxt[] =
	"SELECT;FROM;"
	"INNER;"
	"JOIN;"
	"ON;"
	"OUTER;"
	"RIGHT;"
	"LEFT;"
	"EXISTS;"
	"IN;"
	"NOT;"
	"LIKE;"
	//"AS;"
;

struct WordsCompare{
	int leghosszabbSzo;
	int szavakSzama;
//Betűk jelölése oszloponként
	int* bynarytree;
//Honnan
	int* bywoherin; // Inicializálás binarytree*32, Csökkentés: Ahány betű

	char* byoszlop;
//VégsőÉrték
	char* vegsoErtekOszlopSzam; // Inicializálás a szavak számával
	int* vegsoErtek;

	WordsCompare(int leghosszabbSzo, int szavakSzama){
		this->leghosszabbSzo = leghosszabbSzo;
		this->szavakSzama = szavakSzama;
        bynarytree = new int[leghosszabbSzo];
        bywoherin = new int[leghosszabbSzo * 32];
        byoszlop = new char[leghosszabbSzo * 32];
        vegsoErtek = new int[szavakSzama];
        vegsoErtekOszlopSzam = new char[szavakSzama];
    }

	~WordsCompare(){
        delete[] bynarytree;
        delete[] bywoherin;
        delete[] byoszlop;
        delete[] vegsoErtek;
        delete[] vegsoErtekOszlopSzam;
    }
};

struct StoreNames{
	int length;
	int* sepIndexes;
	char* characterChain;

	StoreNames(char* characterChain){
		this->characterChain = characterChain;
		this->length = this->getSepNumber();
		sepIndexes = new int[this->length];
		initSepIndexes();
	}

	~StoreNames(){
		delete[] sepIndexes;
//		delete[] characterChain;
	}

	int getSepNumber(){
		int sepNumberSum = 0;
		char lastChar = ';';
		int i = 0;
		for(; characterChain[i] != '\0'; i++){
			if(characterChain[i] > 96 && characterChain[i] < 123) characterChain[i] -= 32;
			if(lastChar == ';' && (characterChain[i] > 64 && characterChain[i] < 91)) sepNumberSum++; 
			if(characterChain[i] == ';' || (characterChain[i] > 64 && characterChain[i] < 91)) lastChar = characterChain[i]; 
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

inline bool isBenneVanRendezett(const char array[][50], const char values[][50], int lengtharray, int lengthvalues){
	bool bennevan = 1; // Minden benne van
	for(int i = 0; i < lengthvalues && bennevan; i++){ // lista
		std::cout<<"Ide még belép"<<endl;
		bennevan = 0;
		for(int j = 0; j < lengtharray && !bennevan; j++){
			std::cout << "Ide is" << endl;
			bool nemugyanolyan = false;
			for(int k = 0; k < 50 && !nemugyanolyan; k++){
				nemugyanolyan = (array[j][k] ^ values[i][k]) & 0b1;
			}
			bennevan = !nemugyanolyan;
		}
	}
	;
	return !bennevan;
}


void signal_handler(int signal) {
    if (C.is_open()) {
        std::cout << "Zárjuk az adatbázis kapcsolatot..." << std::endl;
        C.disconnect();
    }
    std::cout << "A program leállt." << std::endl;
    exit(0);  // Kilépés
}



/*inline bool isBenneVanRendezett(std::string[] array, std::string[] values){
    /*for(int i = 0; i < arr){
        for(){

        }
    }
    ;
    return false;
}

inline bool isBenneVannak(std::string[] array, std::string[] values){
    bool both = false;
    for(int i = 0; i<values.length && !both; i++){
        both = isBenneVan(array, values[i]);
    }
    return both;
}

inline bool isBenneVan(std::string[] array, std::string value){
    bool both = false;
    for(int i = 0; i < array.length && !both; i++){
        both = array[i].equals(value);
    }
    return both;
}
*/
inline std::string getWithoutSpace(string text){
    int i = text.length() - 1;
    for(;i>-1 && text[i] == ' '; i--){};
    return text.erase(i+1);
}

/*inline bool hasSubQuery(const char querytext[]){
	std::regex pattern(R"(\s*SELECT)", std::regex_constants::icase);
	return std::regex_search(querytext, pattern);
}*/

WordsCompare doSyntaxtCheckPreparation(char* characterChain){	
	// Segéd
	char szamlal = 0;
	char lastChar = ';';
	// Utófeltétel
	int szavakSzama = 0; 
	char leghosszabbSzo = 0;


	int i = 0;
	for(; characterChain[i] != '\0'; i++){
		if(lastChar != ';' && (characterChain[i] == ';' || characterChain[i+1] == '\0')){
			szavakSzama++;
			leghosszabbSzo = szamlal > leghosszabbSzo ? szamlal : leghosszabbSzo;
			szamlal = 0;
		}
		if(characterChain[i] > 96 && characterChain[i] < 123) characterChain[i] -= 32;
		if(characterChain[i] > 64 && characterChain[i] < 91){
			szamlal++;
		}
		if(characterChain[i] > 64 && characterChain[i] < 91 || lastChar == ';'){
			lastChar = characterChain[i];
		}
	}
	WordsCompare wordsCompare(leghosszabbSzo, szavakSzama);

	//Reinit
	szamlal = 0;
	szavakSzama = 0;
	lastChar = ';';
	int position = 0;
	int lastCharPosition = 0;
	int wordValue = 0;
	i = 0;
	for(; characterChain[i] != '\0'; i++){
		if(characterChain[i] > 64 && characterChain[i] < 91){
			position = characterChain[i] - 65; // zyxwvutsrqponmlkjihgfedcba
			wordValue += (26 * szamlal) + position;
		//	std::cout << "WordValue(Pre): " << wordValue << " - " << i << endl;
			wordsCompare.bynarytree[szamlal] |= 1 << position;
			if(lastChar != ';') wordsCompare.bywoherin[(szavakSzama * 26) + position] |= 1 << lastCharPosition; //abcdefghijkmnopqrstuvwqyz
			szamlal++;
		}
//		std::cout << "LastChar: " << lastChar << " Aktuel: " << characterChain[i+1] << endl;
		if(lastChar != ';' && (characterChain[i] == ';' || characterChain[i] == '\n')){ 
		//	std::cout << "WordValue(Pre): " << wordValue << endl;
			wordsCompare.vegsoErtek[szavakSzama] = wordValue;
			wordsCompare.vegsoErtekOszlopSzam[szavakSzama] = (char)szamlal;
			szavakSzama++;
			szamlal = 0;
			wordValue = 0;
		}
		if(characterChain[i] > 64 && characterChain[i] < 91 || characterChain[i] == ';'){ 
			lastChar = characterChain[i];
		}
	}

	return wordsCompare;
}

inline std::string getTextWithJSONValues(WordsCompare& wordsCompare, StoreNames storeNames[], const std::string JSONValuesString, char* text){
	bool haved = false;
	std::string retn = "";
	retn.reserve(strlen(text) * 3 / 2);
	int szamlal = 0;
	int position = 0;
	bool syntaxtGood = true;
	char currentChar = '\0';
	char lastChar = ';';
	int wordValue = 0;

	int usedStoreNames = -1;
	auto JSONValues = crow::json::load(JSONValuesString);

	int i = 0;
	std::cout << "Még megyen" << endl;
	while(text[i] != '\0' && syntaxtGood){
		std::cout << "Még megyen: " << i << endl;
		text[i] = text[i] > 96 && text[i] < 123 ? text[i] - 32 : text[i];
		if(text[i] > 64 && text[i] < 91){
//			haved = true;
			position = text[i] - 65;
			syntaxtGood = true;//(wordsCompare.bynarytree[szamlal] >> (position)) & 0b1;
			std::cout << "Számláló: " << text[i] <<
					" - " << std::bitset<32>(wordsCompare.bynarytree[szamlal]) <<
					" >> " << text[i] - 65 << " 0b " << 
					std::bitset<32>((wordsCompare.bynarytree[szamlal] >> (position)) & 0b1) <<
			endl;
			std::cout << "Syn.: " << syntaxtGood << endl;

			if(szamlal > 0) syntaxtGood = wordsCompare.bywoherin[((szamlal - 1) * 26) + position] >> lastChar - 65;
			std::cout << "Még megyen: " << i << endl;
			wordValue += ((szamlal) * 26) + position;
			szamlal++;
			//std::cout << text[i] << ":" << (int)text[i] << std::endl;
			// Bináris fa összehasonlítás
		}
		
		if((text[i+1] == '\0' && (lastChar > 64 && lastChar < 91 || text[i] > 64 && text[i] < 91)) ||
						(!(text[i] > 64 && text[i] < 91) && lastChar > 64 && lastChar < 91)){
			//syntaxtGood = false;
			int j = 0;
			std::cout << "WordValue: " << wordValue << endl;
			for(/*Kezdőérték*/; j < wordsCompare.szavakSzama && !syntaxtGood; j++){
				syntaxtGood = wordsCompare.vegsoErtek[j] == wordValue;
				std::cout << "VegsoErtek: " << wordsCompare.vegsoErtek[j] << endl;
			}
			szamlal = 0;
			wordValue = 0;
			std::cout << "Syn.: " << syntaxtGood << endl;
			//if(text[i] != ' ') retn += ' ';
		}
		std::cout << "Még megyen: " << i << endl;
		if(text[i] == '#'){
			i++;
			usedStoreNames = (unsigned)(text[i] - 36) < (39 - 36) ? text[i] - 36 : - 1;
			if(usedStoreNames!=-1){
				std::string field = "";
				int wheres = -1;
				bool addValue = false;
				i++;
				if(text[i] == '-'){
					addValue = true;
					i++;
				}
				if(!addValue){
					field += text[i];
					while((unsigned)(text[i] - 48) < (58 - 48)){
						i++;
					}
					std::cout << "Még megyeni: " << i << endl;
					if(field.length() > 0){
						StoreNames& localStoreNames = storeNames[usedStoreNames];
						wheres = std::stoi(field);
//						std::cout << "Wheres?:SepIndexes : " << wheres << ":" << localStoreNames.length << endl;
						int j = localStoreNames.sepIndexes[wheres] ? localStoreNames.sepIndexes[wheres] : -1;
						std::cout << "J?: " << j << endl;
						if(j > 0){
							for(; localStoreNames.characterChain[j] != ';' && localStoreNames.characterChain[j] != '\0'; j++){
								std::cout << localStoreNames.characterChain[j] << endl;
								retn += localStoreNames.characterChain[j]; 
							}
							std::cout << retn << endl;
						}
						std::cout << "Még megyeni: " << i << endl;
					}
					std::cout << "Még megyeni: " << i << endl;
				}
				else{
					while((unsigned)(text[i] - 65) < (91 - 65)){
						field += text[i];
					}
					if(field.length() > 0 && JSONValues[field]){
						if(JSONValues[field].t() == crow::json::type::String){
							retn += "'"+std::string(JSONValues[field].s())+"'";
						}
						else{
							crow::json::wvalue myOb(JSONValues[field]);
							retn += myOb.dump();
						}
					}
				}
				std::cout << "Még megyeni: " << i << endl;
			}
			std::cout << "Még mindig megyen" << endl;	
		}
		else{
			retn += text[i];
		}
		lastChar = text[i];
		if(text[i] != '\0') i++;
	}
	if(!syntaxtGood) retn = "-";
	return retn;
};

inline std::string getSQLQuery(pqxx::work &W, const char* querytext){
	std::string textout = "";
    pqxx::result R = W.exec(querytext);
    for (const auto &row : R) {
        for(int i = 0; i < row.size(); i++){
         // textout += "valami";
		  textout += getWithoutSpace(row[i].as<std::string>()) + ":::";
        }
        textout = textout.length() > 3 ? textout + ";;;\n" : "";
    }
	return textout;
}

inline std::string getCallMethod(pqxx::work &W, std::string usertoken, std::string methodname){
	//Jogosultság ellenörzés
	return "";//getSQLQuery(W, "Select " + methodname);
}

inline std::string getCallSimpleQuery(pqxx::work &W, std::string tablanev, std::string usertoken, std::string oszlopnevek, std::string wheretext, std::string innerjoins){
//	bool both = 
	// táblanév jogosultságok lekérdezése
	std::string hozzaferhetoek = nullptr;
	/*std::string[] oszlopnevekArray = oszlopnevek.split(";");
//	isBenneVanRendezett
	for(int i = 0; i < oszlopnevekArray.size(); i++){
		
	}*/
	return "";//getSQLQuery(W, "SELECT " + oszlopnevek + " FROM " + tablanev + " ");
}

inline std::string getDBQueryUnit(std::string unit, std::string value){
	return value.length() > 0 ? " " + unit + " " + value : "";
}

/*inline std::string getDBJoin(std::string type, std::string keypairs){
	char keppairsch[][50] = keypairs.split(';');
	return "";
}*/

int main(){
	std::cout << "Elindul" << endl;
	doSyntaxtCheckPreparation(szoRaktarSQLSyntaxt);
	std::cout << "Még megy" << endl;
	WordsCompare compareWords = doSyntaxtCheckPreparation(szoRaktarSQLSyntaxt); 
	std::cout << "Még megy" << endl;
//	CompareWords compareWords = doSyntaxtCheckPreparation();
	char jj[] = "dd;DD;DD;EE";
	char jj1[] = "ABCDZabcz&$ -";
	char jj2[] = "SILECT";
	char jj3[] = "SELECT #$3";

	StoreNames storeNames[] = {
		StoreNames(jj)
	};
	std::cout << "Még megy" << endl;
	std::cout << getTextWithJSONValues(compareWords, storeNames, "", jj1) << endl;
	std::cout << "Még megy" << endl;
	std::cout << getTextWithJSONValues(compareWords, storeNames, "", jj2) << endl;
	std::cout << getTextWithJSONValues(compareWords, storeNames, "", jj3) << endl;
	std::cout << "Még mindig megy" << endl;

	return 0;
	crow::App<crow::CORSHandler> app;
	auto& cors = app.get_middleware<crow::CORSHandler>();
    //pqxx::work W(C);

/*	cors
		.global()
			.headers("Content-Type", "X-Custom-Header", "Upgrade-Insecure-Requests", "Cache")
			.methods("GET"_method, "POST"_method, "PUT"_method, "DELETE"_method, "OPTIONS"_method)
	        //.origin("http://experimental.local:18080")
	        .origin("http://localhost")
			.allow_credentials()
		/*.on_preflight([](crow::response& res, crow::request& req) {
        	std::cout << "Preflight request received for: " << req.url << std::endl;
	        std::cout << "Origin: " << req.get_header_value("Origin") << std::endl;
    	    res.set_header("Access-Control-Allow-Origin", req.get_header_value("Origin"));
       		res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
	        res.set_header("Access-Control-Allow-Headers", "X-Custom-Header, Upgrade-Insecure-Requests, Content-Type");
    	    res.set_header("Access-Control-Allow-Credentials", "true");
	        res.end();
    	});*/;
				            //.methods("GET"_method, "POST"_method, "OPTIONS"_method);
	/*	.prefix("/nocors")
		    .ignore();*/

    if (C.is_open()) {
        cout << "Opened database successfully: " << C.dbname() << endl;
        std::signal(SIGINT, signal_handler);
        /*W.commit();*/
  //      std::cout << getSQLQuery(W, "SELECT id, name FROM users");
  //      std::cout << getSQLQuery(W, "create table if not exists users(id int, name char(30))");
  //      std::cout << getSQLQuery(W, "SELECT public.helloworld('Szabó Roland')");
		CROW_ROUTE(app, "/login")([](const crow::request& req) {
	        crow::response res;
			std::string token = "YOUR_SECURE_TOKEN";
		/*	res.set_header("Access-Control-Allow-Origin", "http://localhost");
			res.set_header("Access-Control-Allow-Credentials", "true");
			res.set_header("Access-Control-Allow-Headers", "Content-Type");
			res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
	      */ 	res.set_header("Set-Cookie", "auth_token=" + token + "; HttpOnly; SameSite=Lax"); // Secure;
			res.code = 200;
		   	res.body = "Bejelentkezve!";
			//res.end();
		    return res;
		});

		CROW_ROUTE(app, "/ujvacsora")([](){
			pqxx::work W(C);
        	return getSQLQuery(W, "SELECT public.helloworld('Szabó Roland')");
    	});
		
		CROW_ROUTE(app, "/addrecord/<string>").methods("POST"_method)([](const crow::request& req, std::string tablename){
            pqxx::work W(C); 
			return getSQLQuery(W, ("INSERT INTO " + tablename + "values ()").c_str());
        });

		CROW_ROUTE(app, "/callquery").methods("POST"_method)([](const crow::request& req){
			auto json = crow::json::load(req.body);
			if (!json) return crow::response(400, "Invalid JSON");
			crow::json::wvalue tablenames = json["db-tablenames"];
			crow::json::wvalue columnames = json["db-columnames"];
			crow::json::wvalue methodnames = json["db-methodnames"];
			
			crow::json::wvalue queryfrom = json["db-queryfrom"];
			crow::json::wvalue querystatements = json["db-querystatements"];

			pqxx::work W(C);
        	return /*getSQLQuery(W,  
				("SELECT " + columnames + 
				" FROM " + tablenames + 
				" INNER JOIN " +
				" WHERE " + whereclause + 
				" GROUP BY " + groupby +
				" HAVING " + having).c_str())*/crow::response(200, "Megkaptam!");
    	});

		CROW_ROUTE(app, "/deletefrom/<string>/<int>").methods("DELETE"_method)([](const crow::request& req, const std::string tablename, const int id){ // Egyszerű kulcsos táblák
            pqxx::work W(C);
			std::ostringstream sqlStream;
			sqlStream << "DELETE FROM " << tablename << " WHERE " << tablename << ".id = " << id;
			return getSQLQuery(W, sqlStream.str().c_str()); // Feladat: User check hozzáadása
        });

/*		CROW_ROUTE(app, "/deletefrom/<string>").methods("POST"_method)([](const crow::request& req, const std::string tablename, const int id){ // Összetett kulcsos táblák
            pqxx::work W(C); 
			return getSQLQuery(W, "INSERT INTO " + tablename + "values ()");
        });
*/
		CROW_ROUTE(app, "/update/<string>/<int>").methods("PUT"_method)([](const crow::request& req, const std::string tablename, const int id){ // Egyszerű kulcsos táblák
            // Miken
			// Mikre
//			crow::json::rvalue rval = 
			std::string keyvaluepairs = "";
			pqxx::work W(C);
			std::ostringstream sqlStream;
			sqlStream << "UPDATE " << tablename << " SET " << keyvaluepairs << " WHERE " << tablename << ".id = " << id;
			return getSQLQuery(W, sqlStream.str().c_str());
        });

/*		CROW_ROUTE(app, "/update/<string>").methods("POST"_method)([](const crow::request& req, const std::string tablename, const int id){ // Összetett kulcsos táblák
            pqxx::work W(C); 
			return getSQLQuery(W, "INSERT INTO " + tablename + "values ()");
        });
*/
	    CROW_ROUTE(app, "/callmethod/<string>").methods("GET"_method)([](const crow::request& req, const std::string methodname){
        	return "";
    	});
  //      C.disconnect();
    } else {
        cout << "Can't open database" << endl;
        return 1;
    }
	
	CROW_ROUTE(app, "/")([](){
		return "Hello world";
	});

    CROW_ROUTE(app, "/exat")([](){
		return std::to_string(exat);
	});

    CROW_ROUTE(app, "/exat/<int>").methods("POST"_method)([](int a){ 
		exat=a; 
		return std::to_string(0b0111101); 
	});

	CROW_ROUTE(app, "/pelda/<int>").methods("POST"_method)([](const crow::request& req, const int ye){
		
		std::cout << "Fejlec:" << ye << endl;
		for (auto& header : req.headers)
		{
			std::cout << header.first << ": " << header.second << "\n";
		}
		std::cout << "\nBody:\n" << req.body << "\n";
		auto parsed = crow::json::load(req.body);
	//	std::cout << "\nBody:\n" << parsed["datum"].dump() << "\n";
		crow::json::wvalue temp(parsed);
		std::cout << "\nBody:\n" << temp.dump() << "\n";

		return crow::response(200, "Megkaptam!");
	});
    app.port(18080).multithreaded().run();
  	return 0;
}
