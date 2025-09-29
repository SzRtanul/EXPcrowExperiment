#include "core.h"
#include "crow.h"
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

pqxx::connection C = pqxx::connection(R"(dbname=testdb3 user=postgres password=test123 hostaddr=127.0.0.1 port=5432)");
int exat=0;

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

inline std::string getSQLQuery(
	std::shared_ptr<pqxx::connection> NC,
	const char* querytext,
	const std::string recordsep,
	const std::string columnsep,
	bool columnnames, bool sign
){
	std::cout << "DBBBBB: " << std::endl;
	pqxx::work W(*NC);
	std::cout << "DBBBBB: " << std::endl;
	std::string textout = "-";
	std::string rownums = "";
    try
	{
		std::string item = "";
		std::cout << "qText: " << querytext << std::endl;
		pqxx::result R = W.exec(querytext);
		std::cout << "DBBBBB: " << std::endl;
		// Itt folytatódik a sikeres lekérdezés feldolgozása
		rownums = sign ? std::string("F") + static_cast<char>(R.columns())+"2;" : "";
		std::cout << "DBBBBB: " << std::endl;
		if(columnnames){
			rownums = sign ? std::string("T") + static_cast<char>(R.columns()) + "2;": "";
			for (int i = 0; i < R.columns(); ++i) {
				textout += R.column_name(i);// + columnsep;
				rownums += (textout.length() - 1) + ";";
			}
//			textout += recordsep;
		}
		std::cout << "DBBBBB: " << std::endl;
	    for (const auto &row : R) {
	        for(int i = 0; i < row.size(); i++){
	          	// textout += "valami";
			  	textout += !row[i].is_null() ? getWithoutSpace(row[i].as<std::string>()) + columnsep : "null" + columnsep;
				rownums += (textout.length() - 1) + ";";
        	}
        	//textout = textout.length() > recordsep.length() ? textout + recordsep : "";
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
	std::cout << "ADAT KIÍRÁS!" << std::endl;
//std::cout << textout << std::endl;
 	return sign ? rownums + "|||" + textout : textout;
}

inline std::string getSQLQuery(std::shared_ptr<pqxx::connection> NC, const char* querytext){
	return getSQLQuery(NC, querytext, "", ":::", true, true);
}

inline bool isJogosult(std::shared_ptr<pqxx::connection> NC, std::string gnndump, std::string keynames){
		std::string hh ="SELECT set_config('app.token', '"+ gnndump +"', false);" +
		"" +
		"" +
		"" + 
		"select sysadmin.getaccesfullschemasfromgroups(" + gnndump + std::string(", '\?',  '") + keynames + "')";

		std::string qre = getSQLQuery(NC, hh.c_str(), "", "", false, false); // Get check 1.
		return qre.length() > 0 ? qre[0] == 't' : 0;
}

inline bool setSessionValues(std::shared_ptr<pqxx::connection> NC, std::string gnndump, std::string keynames){
		std::string hh ="SELECT set_config('app.token', '"+ gnndump +"', false);" +
		"" +
		"" +
		"" + 
		"select sysadmin.getaccesfullschemasfromgroups(" + gnndump + std::string(", '\?',  '") + keynames + "')";

		std::string qre = getSQLQuery(NC, hh.c_str(), "", "", false, false); // Get check 1.
		return qre.length() > 0 ? qre[0] == 't' : 0;
}

inline std::string getTextWithJustChars(std::string text){
	std::string out = "";
	for (int i = 0; text[i] != '\0'; i++){
		if(((unsigned)text[i] - 65 < 58 && (unsigned)text[i] - 91 > 5) || text[i] == 95) out += text[i];
	}
	return out;
}

int entraceMethod(
	int crowPort,
	int minDBConn,
	int maxDBConn,
	std::string postgresDBlocation, 
	std::string postgresDBusername, 
	std::string postgresDBpassword, 
	std::string postgresDBport, 
	std::string serviceDBName

){
//	static PoolDBConnection poolOldDB("dbname=testdb3 user=postgres password=test123 hostaddr=127.0.0.1 port=5432", 15, 50);
	static PoolDBConnection poolDB(
		"dbname=" + serviceDBName +
		" user=" + postgresDBusername +
		" password=" + postgresDBpassword +
		" hostaddr=" + postgresDBlocation +
		" port=" + postgresDBport,
		minDBConn, maxDBConn
	);

	crow::App<crow::CORSHandler> app;
    if (C.is_open()) {
        cout << "Opened database successfully: " << C.dbname() << endl;
		
		CROW_ROUTE(app, "/gettable/<string>/<string>/<int>/<int>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename,
			const int offset,
			const int limit
		){
			// json[dbthings][set_configs]
			auto json = crow::json::load(req.body);
			std::string out = "-";
			std::string transedschema = getTextWithJustChars(schema);
			bool resnum = json ? isJogosult(NC, json["token"].dump(), transedschema) : false;
			if(resnum){
				std::shared_ptr<pqxx::connection> NC = poolDB.getDBConn();
				std::string hjut = "select * from "+ transedschema + "." + getTextWithJustChars(tablename) + ";";
				const char* hja = hjut.c_str();
				out = getSQLQuery(NC, hja);
				poolDB.giveBackConnect(NC);
			}
			return crow::response(resnum ? 200 : 400, out);
		});

		CROW_ROUTE(app, "/insert/<string>/<string>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename,
		){
			auto json = crow::json::load(req.body);
			std::string out = "-";
			std::string transedschema = getTextWithJustChars(schema);
			bool resnum = json ? isJogosult(NC, json["token"].dump(), transedschema) : false;
			// json[dbthings][columns]
			// json[dbthings][values]
			// json[dbthings][set_configs]
			// 
			if(resnum){
				std::shared_ptr<pqxx::connection> NC = poolDB.getDBConn();
				std::string hjut = "insert into "+ transedschema + "." + getTextWithJustChars(tablename) +
				"" + ";";
				const char* hja = hjut.c_str();
				out = getSQLQuery(NC, hja);
				poolDB.giveBackConnect(NC);
			}
			return crow::response(resnum ? 200 : 400, out);
		});

		CROW_ROUTE(app, "/delete/<string>/<string>/<int>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename,
			const int row,
		){
			// json[dbthings][set_configs]
			auto json = crow::json::load(req.body);
			std::string out = "-";
			std::string transedschema = getTextWithJustChars(schema);
			std::string transedtable = getTextWithJustChars(tablename);
			bool resnum = json ? isJogosult(NC, json["token"].dump(), transedschema) : false;
			if(resnum){
				std::shared_ptr<pqxx::connection> NC = poolDB.getDBConn();
				std::string hjut = "delete from "+ transedschema + "." + transedtablename +
				"where " + transedschema + "." + transedtablename+".id = " + row + ";";
				const char* hja = hjut.c_str();
				out = getSQLQuery(NC, hja);
				poolDB.giveBackConnect(NC);
			}
			return crow::response(resnum ? 200 : 400, out);
		});

		CROW_ROUTE(app, "/update/<string>/<string>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename,
			// json[dbthings][col&values]
			// json[dbthings][set_configs]
			auto json = crow::json::load(req.body);
			std::string out = "-";
			std::string transedschema = getTextWithJustChars(schema);
			std::string transedtable = getTextWithJustChars(tablename);
			bool resnum = json ? isJogosult(NC, json["token"].dump(), transedschema) : false;
			if(resnum){
				std::shared_ptr<pqxx::connection> NC = poolDB.getDBConn();
				std::string hjut = "update "+ transedschema + "." + transedtablename +
				"set column='ye' "
				"where " + transedschema + "." + transedtablename+".id = " + row + ";";
				const char* hja = hjut.c_str();
				out = getSQLQuery(NC, hja);
				poolDB.giveBackConnect(NC);
			}
			return crow::response(resnum ? 200 : 400, out);
		});
  //      C.disconnect();
    } else {
        cout << "Can't open database" << endl;
        return 1;
    }	
	
	app.port(crowPort).multithreaded().run();
  	return 0;
}
