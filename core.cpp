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
		rownums = sign ? std::string("F") + static_cast<char>(R.columns()) + "2;" : "";
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



inline bool isCsChar(char CharC){
	return ((unsigned)CharC - 65 < 58 && (unsigned)CharC - 91 > 5) || CharC == 95;
}

inline std::string getTextWithJustChars(std::string text){
	std::string out = "";
	for (int i = 0; text[i] != '\0'; i++){
		if(isCsChar(text[i])) out += text[i];
	}
	return out;
}

inline std::string getSetConfigs(int &i, std::string text){
	bool change = true;
	std::string out = text[0] != '\0' ?  "SELECT set_config('custom." : "";
	for(int i = 0; i != '\0';i++){
		if(change){
			for(; text[i] != '='; i++){
				if(isCsChar(text[i])) out += text[i];
			}
		}
		else{
			int limn = i + text[i];
			for(i=i+1; i < limn && text[i] != '\0'; i++){
				out += text[i];
				if(text[i] == '\'') out += '\'';
			}
		}
		out += change ? "', '" : "');\nSELECT set_config('custom.";
		change = !change;
	}
	return out;
}

inline std::string getUpdateSets(int &i, std::string text){
	std::string out = "SET ";
	bool change = true;
	for(; text[i] != '\0'; i++){
		if(change){
			for(;text[i] != '='; i++){
				if(isCsChar(text[i])) out += text[i];
			}
		}
		else{
			int limn = i + text[i]+text[i+1]*256; //For cicla
			for(i=i+1; i < limn && text[i] != '\0'; i++){
				out += text[i];
				if(text[i] == '\'') out += '\'';
			}
		}
		out+=change ? "='" : "'\n";
	}
	return out;
}

inline std::string insertColumns(int &i, std::string text){
	std::string out = "";
	for(; text[i] != '\0'; i++){
		if(isCsChar(text[i]) || text[i] == ',') out += text[i];
	}
	return out;
}

inline std::string insertValues(int &i, std::string text){
	std::string out = "'";
	for(; text[i] != '\0'; i++){
		int limn = i + text[i]+text[i+1]*256;
		for(i=i+2; i < limn && text[i] != '\0'; i++){
			out += text[i];
			if(text[i] == '\'') out += '\'';
		}
	}
	return out;
}

inline bool setSessionValues(std::shared_ptr<pqxx::connection> NC, int i, std::string text){
		std::string hh =  getSetConfigs(i, text);
		std::string qre = getSQLQuery(NC, hh.c_str(), "", "", false, false); // Get check 1.
		return qre.length() > 0 ? qre[0] == 't' : 0;
}

std::string metha(int index, int outi, std::string dbthings, std::string transedschema, std::string transedtablename, int offset, int limit, int row){
	std::array<std::string, 4> queries = {
		//select
		"select * from "+ transedschema + "." + transedtablename + " OFFEST " + std::to_string(offset) + " LIMIT " + std::to_string(limit) + ";",
		//insert
		"insert into " + transedschema + "." + transedtablename + "(" + insertColumns(outi, dbthings) + ") values (" + insertValues(outi, dbthings) + ");",
		//delete
		"delete from "+ transedschema + "." + transedtablename +
			"where " + transedschema + "." + transedtablename+".id = " + std::to_string(row) + ";",
		//update
		"update "+ transedschema + "." + transedtablename + getUpdateSets(outi, dbthings) +
			"\nwhere " + transedschema + "." + transedtablename+".id = " + std::to_string(row) + ";"
	};
	return queries[index];
}

inline crow::response execFormat(PoolDBConnection& poolDB, int caseindex, crow::json::rvalue json, std::string schemaname, std::string tablename, int offset, int limit, int row)
{
	std::shared_ptr<pqxx::connection> NC = poolDB.getDBConn();
	int outi = 0;
	std::string out = "-";
	bool resnum = json ? isJogosult(NC, json["token"].s(), schemaname) : false;
	if(resnum){
		std::string dbthings = json["dbthings"].s();
		setSessionValues(NC, outi, dbthings);
		std::string queryText = metha(caseindex, outi, dbthings, schemaname, tablename, offset, limit, row);
		out = getSQLQuery(NC, queryText.c_str());
		poolDB.giveBackConnect(NC);
	}
	return crow::response(resnum ? 200 : 400, out);
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
		) -> crow::response{
			// json[dbthings][set_configs]
			auto json = crow::json::load(req.body);
			std::string transedschema = getTextWithJustChars(schema);
			return execFormat(poolDB, 0, json, transedschema, getTextWithJustChars(tablename), offset, limit, 0);	
			
		});

		CROW_ROUTE(app, "/insert/<string>/<string>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename
		) -> crow::response{
			auto json = crow::json::load(req.body);
			return execFormat(poolDB, 1, json, getTextWithJustChars(schema), getTextWithJustChars(tablename), 0, 0, 0);
		});

		CROW_ROUTE(app, "/delete/<string>/<string>/<int>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename,
			const int row
		) -> crow::response{
			auto json = crow::json::load(req.body);
			return execFormat(poolDB, 2, json, getTextWithJustChars(schema), getTextWithJustChars(tablename), 0, 0, row);
		});

		CROW_ROUTE(app, "/update/<string>/<string>/<int>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename,
			const int row
		) -> crow::response{
			auto json = crow::json::load(req.body);
			return execFormat(poolDB, 3, json, getTextWithJustChars(schema), getTextWithJustChars(tablename), 0, 0, row);
		});
//      C.disconnect();
    } else {
        cout << "Can't open database" << endl;
        return 1;
    }	
	
	app.port(crowPort).multithreaded().run();
  	return 0;
}
