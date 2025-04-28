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





pqxx::connection C = pqxx::connection(R"(dbname = testdb3 user=postgres password=test123 hostaddr=127.0.0.1 port=5432)");
int exat=0;
// SzóRaktár
char szoRaktarSQLSyntaxt[] =
	"SELECT;FROM;WHERE;"
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
	"AS;"
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
        bynarytree = new int[leghosszabbSzo]();
        bywoherin = new int[leghosszabbSzo * 32]();
        byoszlop = new char[leghosszabbSzo * 32]();
        vegsoErtek = new int[szavakSzama]();
        vegsoErtekOszlopSzam = new char[szavakSzama]();
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
		sepIndexes = new int[this->length]();
		initSepIndexes();
	}

	~StoreNames(){
		delete[] sepIndexes;
	}

	int getSepNumber(){
		int sepNumberSum = 0;
		char lastChar = ';';
		int i = 0;
		for(; characterChain[i] != '\0'; i++){
			if(characterChain[i] > 96 && characterChain[i] < 123) characterChain[i] -= 32;
			if(lastChar == ';' && (characterChain[i] > 64 && characterChain[i] < 91)) sepNumberSum++; 
			if(characterChain[i] == ';' || (characterChain[i] > 64 && characterChain[i] < 91)) lastChar = characterChain[i];
			else if(characterChain[i] != '_') characterChain[i] = '_';
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

inline bool whatIsChar(char character){
	return ((unsigned)(character - 65) < (91- 65)) || character == '_';
}

WordsCompare doSyntaxtCheckPreparation(char* characterChain){	
	// Segéd
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
			szamlal = 0;
		}
		if(characterChain[i] > 96 && characterChain[i] < 123) characterChain[i] -= 32;
		if(whatIsChar(characterChain[i])){
		//	if(szamlal == 127) return WordsCompare(0, 0);
			szamlal++;
		}
		if(whatIsChar(characterChain[i]) && lastChar == ';'){
			lastChar = characterChain[i];
		}
	}
	std::cout<<characterChain << endl;
	std::cout << leghosszabbSzo << " : " << szavakSzama << endl;
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
		if(whatIsChar(characterChain[i])){
			position = characterChain[i] - 65; // zyxwvutsrqponmlkjihgfedcba
			wordValue += (31 * szamlal) + position;
			std::cout << "WordValue(Pre): " << wordValue << " - " << i << endl;
			wordsCompare.bynarytree[szamlal] |= 1 << position;
//			if(lastChar != ';') wordsCompare.bywoherin[(szavakSzama * 26) + position] |= 1 << lastCharPosition; //abcdefghijkmnopqrstuvwqyz
			szamlal++;
		}
		std::cout << "LastChar: " << lastChar << " Aktuel: " << characterChain[i] << endl;
		if((lastChar != ';' && (characterChain[i] == ';') || (lastChar != ';' && characterChain[i+1] == '\0'))){ 
			std::cout << "WordValue(Pret): " << wordValue << endl;
			wordsCompare.vegsoErtek[szavakSzama] = wordValue;
			wordsCompare.vegsoErtekOszlopSzam[szavakSzama] = (char)szamlal;
			szavakSzama++;
			szamlal = 0;
			wordValue = 0;
		}
		if(whatIsChar(characterChain[i]) || characterChain[i] == ';'){ 
			lastChar = characterChain[i];
		}
	}
	return wordsCompare;
}

inline std::string getSQLQuery(pqxx::work &W, const char* querytext, const std::string recordsep, const std::string columnsep){
	std::string textout = "-";
    try
	{
		pqxx::result R = W.exec(querytext);
		// Itt folytatódik a sikeres lekérdezés feldolgozása
		textout = "";
	    for (const auto &row : R) {
	        for(int i = 0; i < row.size(); i++){
	          // textout += "valami";
			  textout += getWithoutSpace(row[i].as<std::string>()) + columnsep;
        	}
        	textout = textout.length() > recordsep.length() ? textout + recordsep : "";
		}
		textout += '\0';
    }
	catch (const pqxx::sql_error &e)
	{
		std::cerr << "SQL hiba: " << e.what() << std::endl;
		std::cerr << "Sikertelen lekérdezés: " << e.query() << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Egyéb hiba: " << e.what() << std::endl;
	}
//	pqxx::result R = W.exec(querytext);
 	return textout;
}

inline std::string getSQLQuery(pqxx::work &W, const char* querytext){
	return getSQLQuery(W, querytext, ";;;\n", ":::");
}

inline std::string getTextWithJSONValues(
		pqxx::work &W,
		WordsCompare& wordsCompare, 
		StoreNames storeNames[], /*const std::string*/ 
		crow::json::rvalue JSONValuesString, 
		char* text
){
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
	auto JSONValues = JSONValuesString;//crow::json::load(JSONValuesString);
	std::string hh = "select sysadmin.getaccesfullschemasfromgroups(5, '" + std::string(storeNames[0].characterChain) + "')";
	std::string qre = getSQLQuery(W, hh.c_str(), "", "");
	syntaxtGood = qre.compare("t");

	int i = 0;
	std::cout << "Még megyen" << endl;
	while(text[i] != '\0' && syntaxtGood == true){
		std::cout << "Meddig1" << endl;
//		std::cout << "Még megyen: " << i << endl;
		text[i] = text[i] > 96 && text[i] < 123 ? text[i] - 32 : text[i];
		std::cout << "Meddig2" << endl;
		if(whatIsChar(text[i])){
			std::cout << "Meddig3" << endl;
//			haved = true;
			position = text[i] - 65;
			std::cout << "Meddig4" << endl;
			syntaxtGood = szamlal < wordsCompare.leghosszabbSzo && ((wordsCompare.bynarytree[szamlal] >> (position)) & 0b1);
			std::cout << "Meddig5" << endl;
			if(syntaxtGood) std::cout << "Számláló: " << text[i] <<
					" - " << std::bitset<32>(wordsCompare.bynarytree[szamlal]) <<
					" >> " << text[i] - 65 << " 0b " << 
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
		
		if((text[i+1] == '\0' && (whatIsChar(lastChar) || whatIsChar(text[i]))) ||
						(!(whatIsChar(text[i])) && whatIsChar(lastChar))
		){
			syntaxtGood = false;
			int j = 0;
			std::cout << "WordValue: " << wordValue << endl;
			for(/*Kezdőérték*/; j < wordsCompare.szavakSzama && !syntaxtGood; j++){
				syntaxtGood = wordsCompare.vegsoErtek[j] == wordValue;
				std::cout << "VegsoErtek: " << wordsCompare.vegsoErtek[j] << endl;
			}
			szamlal = 0;
			wordValue = 0;
//			std::cout << "Syne: " << syntaxtGood << endl;
			//if(text[i] != ' ') retn += ' ';
		}
//		std::cout << "Synitt: " << syntaxtGood << endl;
//		std::cout << "Még megyen: " << i << endl;
		std::cout<< text[i] << endl;
		if(text[i] == '#'){
			i++;
			std::cout<< text[i] << "ALAAAAA " << syntaxtGood << endl;
			bool addValue = false;
			std::string field = "";
			usedStoreNames = (unsigned)(text[i] - 36) < (40 - 36) ? text[i] - 36 : - 1;
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
				std::cout << (int)text[i] <<" JELLLLL "<< endl;
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
						syntaxtGood = ((unsigned)wheres) < localStoreNames.length;
						if(syntaxtGood){
							int j = localStoreNames.sepIndexes[wheres];
							std::cout << "J?: " << j << endl;
							if(usedStoreNames > 0 && lastChar != '.') retn += '.';
							for(; localStoreNames.characterChain[j] != ';' && localStoreNames.characterChain[j] != '\0'; j++){
								std::cout << localStoreNames.characterChain[j] << endl;
								retn += localStoreNames.characterChain[j]; 
							}
						//	retn += text[i];
							i--;
							std::cout << retn << endl;
						}
						std::cout << "Még megyeni: " << i << endl;
					}
					std::cout << "Még megyeni: " << i << endl;
				}
			}
			else if(addValue && syntaxtGood){
				std::cout << (int)text[i]-65 << " JELLLLL "<< endl;
				while(((unsigned)(text[i] - 65) < (91 - 65) || (unsigned)(text[i] - 97) < (123-97))){
					field += text[i];
					i++;
				}
				std::cout << field << endl;
				if(field.length() > 0 && JSONValues.has(field)){
					syntaxtGood = JSONValues.has(field);
					if(JSONValues[field].t() == crow::json::type::String){
						std::string JSONSTR = std::string(JSONValues[field].s());
						std::string::size_type pos = 0;
						std::string s = JSONSTR; 
						while ((pos = s.find("'", pos)) != std::string::npos) {
						    s.replace(pos, 1, "''");
						    pos += 2;
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
		}
		else{
//			std::cout << "Synitt: " << syntaxtGood << endl;
			retn += text[i];
		}
//		std::cout << "Synitt: " << syntaxtGood << endl;
		lastChar = text[i];
		/*if(text[i] != '\0')*/ i++;
//		std::cout << "Synitt: " << syntaxtGood << endl;
	}
//	std::cout << "Syny.: " << syntaxtGood << endl;
	if(!syntaxtGood) retn = "-";
	return retn;
};

int main(){
	WordsCompare compareWords(0, 0); 
	crow::App<crow::CORSHandler> app;
	auto& cors = app.get_middleware<crow::CORSHandler>();
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
		pqxx::work WG(C);

		std::string query = getSQLQuery(WG, "SELECT word FROM pg_get_keywords()", ";", "");
		char* SQLkeywords = new char[query.size() + 1];
		std::copy(query.begin(), query.end(), SQLkeywords);
		SQLkeywords[query.size()] = '\0';

		compareWords = doSyntaxtCheckPreparation(SQLkeywords);
		std::cout<<SQLkeywords<<endl;
		delete[] SQLkeywords;

        cout << "Opened database successfully: " << C.dbname() << endl;
        std::signal(SIGINT, signal_handler);
		std::cout << "OOOO" << endl;
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

		CROW_ROUTE(app, "/callquery").methods("POST"_method)([&compareWords](const crow::request& req){
			auto json = crow::json::load(req.body);
			std::cout << req.body;
			std::cout << "Elmegy";
			if (!json) return crow::response(400, "Invalid JSON");
			std::cout << "Elmegy";
			auto& DBdataJSON = json["db"];
			std::cout << "Elmegy";
			crow::json::rvalue CAzon = json["CAzon"];
			std::cout << "Elmegy";
			std::cout << "Elmegy1";
//			auto& query = json["query"];
			std::string CAzonStr = "";//CAzon.s();
			std::cout << "Elmegy1";
			std::string schemaNamesStr = DBdataJSON["schemanames"].s();
			std::cout << "Elmegy1";
			std::string tableNamesStr = DBdataJSON["tablenames"].s();
			std::cout << "Elmegy1";
			std::string columnNamesStr = DBdataJSON["columnnames"].s();
			std::cout << "Elmegy2";
			std::string methodNamesStr = DBdataJSON["methodnames"].s();
			std::cout << "Elmegy3";
			std::string queryStr = DBdataJSON["query"].s();
			std::cout << "Elmegy4";

			char* schemaNames = const_cast<char*>(schemaNamesStr.c_str());
			char* tableNames = const_cast<char*>(tableNamesStr.c_str());
			char* columnNames = const_cast<char*>(columnNamesStr.c_str());
			char* methodNames = const_cast<char*>(methodNamesStr.c_str());
			char* queryJ = const_cast<char*>(queryStr.c_str());
			std::cout << "Elmegy";

			StoreNames storeNames[] = {
				StoreNames(schemaNames),
				StoreNames(tableNames),
				StoreNames(columnNames),
				StoreNames(methodNames)
			};
			std::cout << "Elmegy";
			pqxx::work W(C);
			std::string quer = getTextWithJSONValues(W, compareWords, storeNames, CAzon, queryJ);
			std::cout << quer << endl;
        	return crow::response(200, getSQLQuery(W, quer.c_str()));
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
		
		std::cout << "OOOO" << endl;
  //      C.disconnect();
    } else {
        cout << "Can't open database" << endl;
        return 1;
    }
	
		std::cout << "OOOO" << endl;
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
		std::cout << "OOOO" << endl;
    app.port(18080).multithreaded().run();
  	return 0;
}
