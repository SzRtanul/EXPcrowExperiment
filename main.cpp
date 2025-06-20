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
#include "models/PoolDBConnection.cpp"
#include "models/WordsCompare.cpp"
#include "models/StoreNames.cpp"
#include "models/gTXTJValues.cpp"

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

PoolDBConnection poolDB("dbname=testdb3 user=postgres password=test123 hostaddr=127.0.0.1 port=5432", 15, 50);

void signal_handler(int signal) {
    if (C.is_open()) {
        std::cout << "Zárjuk az adatbázis kapcsolatot..." << std::endl;
        C.disconnect();
    }
    std::cout << "A program leállt." << std::endl;
    exit(0);  // Kilépés
}

inline std::string getWithoutSpace(string text){
    int i = text.length() - 1;
    for(;i>-1 && text[i] == ' '; i--){};
    return text.erase(i+1);
}

inline std::string getSQLQuery(std::shared_ptr<pqxx::connection> NC, const char* querytext, const std::string recordsep, const std::string columnsep, bool columnnames){
	std::cout << "DBBBBB: " << std::endl;
	pqxx::work W(*NC);
	std::cout << "DBBBBB: " << std::endl;
	std::string textout = "-";
    try
	{
		std::cout << "qText: " << querytext << std::endl;
		pqxx::result R = W.exec(querytext);
		std::cout << "DBBBBB: " << std::endl;
		// Itt folytatódik a sikeres lekérdezés feldolgozása
		textout = "";
		std::cout << "DBBBBB: " << std::endl;
		if(columnnames){
			textout = "-;";
			for (int i = 0; i < R.columns(); ++i) {
				textout += R.column_name(i) + columnsep;
			}
			textout += recordsep;
		}
		std::cout << "DBBBBB: " << std::endl;
	    for (const auto &row : R) {
	        for(int i = 0; i < row.size(); i++){
	          // textout += "valami";
			  textout += !row[i].is_null() ? getWithoutSpace(row[i].as<std::string>()) + columnsep : "null" + columnsep;
        	}
        	textout = textout.length() > recordsep.length() ? textout + recordsep : "";
		}
		std::cout << "DBBBBB: " << std::endl;
//		textout += '\0';
		W.commit();
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
	//W.commit();
 	return textout;
}



inline std::string getSQLQuery(std::shared_ptr<pqxx::connection> NC, const char* querytext){
	return getSQLQuery(NC, querytext, ";;;\n", ":::", true);
}


int main(){
	std::shared_ptr<pqxx::connection> RC = poolDB.getDBConn();
	std::string query = getSQLQuery(RC, "SELECT word FROM pg_get_keywords() ORDER BY LENGTH(word), word", ";", "", false);
	poolDB.giveBackConnect(RC);
	WordsCompare compareWords = doSyntaxtCheckPreparation(query.c_str());
	
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
        cout << "Opened database successfully: " << C.dbname() << endl;
        //std::signal(SIGINT, signal_handler);
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

		CROW_ROUTE(app, "/callquery").methods("POST"_method)([&compareWords](const crow::request& req){
			std::string quer = "-";
			std::shared_ptr<pqxx::connection> NC = poolDB.getDBConn(); 
			try{
				auto json = crow::json::load(req.body);
				std::cout << req.body;
				std::cout << "Elmegy";
				if (!json) {
					poolDB.giveBackConnect(NC);
					return crow::response(400, "Invalid JSON");
				}
				std::cout << "Elmegy";
				std::string DBDataStr = json["db"].s();
				crow::json::rvalue CAzon = json["CAzon"];
				const char* DBDataChr = DBDataStr.c_str();				
				StoreNames keywordNames(DBDataChr);
				/*StoreNames storeNames[] = {
					StoreNames(schemaNamesStr.c_str()),
					StoreNames(tableNamesStr.c_str()),
					StoreNames(columnNamesStr.c_str()),
					StoreNames(methodNamesStr.c_str()),
					StoreNames(aliasesStr.c_str())
				};*/
				crow::json::wvalue gnn = json["token"];
				int qfa = keywordNames.glength > 1 ? (unsigned)keywordNames.sepIndexes[keywordNames.groupIndexes[1]] - 1 : keywordNames.spellNumber;
				std::cout << "QFAi: " << keywordNames.characterChain << std::endl;
				std::cout << "QFA: " << qfa << std::endl;
				std::cout << "MyFast: " << keywordNames.characterChain[keywordNames.spellNumber-1] 
						<< ":" << (int)keywordNames.characterChain[keywordNames.spellNumber-1] << std::endl;
				char qfabsv = '\0'; 
				/*if(keywordNames.characterChain[qfa]){
					qfabsv = keywordNames.characterChain[qfa];
					keywordNames.characterChain[qfa] = '\0';
				}*/
				std::cout << "QFA: " << qfa << std::endl;
				qfabsv = keywordNames.characterChain[qfa];
				keywordNames.characterChain[qfa] = '\0';
				std::cout << "QFA: " << qfa << std::endl;
				std::string hh = 
						"SELECT set_config('app.current_user_id', '"+ gnn.dump() +"', false);" +
						"" +
						"" +
						"" + 
						"select sysadmin.getaccesfullschemasfromgroups(" + 
						gnn.dump() + std::string(", '\?',  '") + std::string(keywordNames.characterChain) + "')";
				std::cout << "QFA: " << qfa << std::endl;
				keywordNames.characterChain[qfa] = qfabsv;
				std::cout << "QFA: " << qfa << std::endl;
				std::string qre = getSQLQuery(NC, hh.c_str(), "", "", false);
				std::cout << "QFA: " << qfa << std::endl;
				bool syntaxtGood = qre.length() > 0 ? qre[0] == 't' : 0;
				const char* jk = &DBDataChr[
					DBDataChr[keywordNames.spellNumber] ? keywordNames.spellNumber + 1 : keywordNames.spellNumber
				];
				quer = syntaxtGood ? getTextWithJSONValues(compareWords, keywordNames, CAzon, jk) : "-";
				std::cout << "ENLY: " << (unsigned)keywordNames.characterChain[keywordNames.spellNumber] << ":" 
						<< (unsigned)keywordNames.characterChain[keywordNames.spellNumber + 1] << std::endl;
			}
			catch(const std::exception &e){				
				std::cerr << "Egyéb hiba: " << e.what() << std::endl;
			}
			std::cout << quer << endl;
			std::string resdb = "err:Hiba történt!";
			if(quer.compare("-")) resdb = getSQLQuery(NC, quer.c_str());
			poolDB.giveBackConnect(NC);
			return crow::response(200, resdb);
    	});
	  
		std::cout << "OOOO" << endl;
  //      C.disconnect();
		std::cout << compareWords.leghosszabbSzo << endl;
    } else {
        cout << "Can't open database" << endl;
        return 1;
    }	
	std::cout << compareWords.leghosszabbSzo << endl;
	std::cout << "OOOO" << endl;
	
	CROW_ROUTE(app, "/")([](){
		return "Hello world";
	});

	CROW_ROUTE(app, "/pelda/<int>").methods("POST"_method)([](const crow::request& req, const int ye){
		
		std::cout << "Fejlec:" << ye << std::endl;
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
