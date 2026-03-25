#include "core.h"
#include "test.h"
#include "include/crow_all.h"
#include <cstring>
#include <csignal>
#include <pqxx/pqxx>
#include <string>
#include <thread>
#include <chrono>
#include <regex>
#include "models/PoolDBConnection.cpp"
#include "models/WordsCompare.cpp"
#include "models/StoreNames.cpp"
#include "models/gTXTJValues.cpp"

using namespace std;
using namespace pqxx;
//using json = nlohmann::json;

int exat=0;

void signal_handler(int signal) {
/*    if (C.is_open()) {
        std::cout << "Zárjuk az adatbázis kapcsolatot..." << std::endl;
        C.disconnect();
    }
    std::cout << "A program leállt." << std::endl;
    exit(0);  // Kilépés*/
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
		textout = sign ? std::string("F") + static_cast<char>(R.columns()) + "" : "";
		std::cout << "DBBBBB: " << std::endl;
		if(columnnames){
			textout = sign ? std::string("T") + static_cast<char>(R.columns()) + "" : "";
			for (int i = 0; i < R.columns(); ++i) {
				textout += R.column_name(i) + columnsep;
//				rownums += (textout.length() - 1) + ";";
			}
			textout += recordsep;
		}
		std::cout << "DBBBBB: " << std::endl;
	    for (const auto &row : R) {
	        for(int i = 0; i < row.size(); i++){
	          	// textout += "valami";
			  	textout += !row[i].is_null() ? getWithoutSpace(row[i].as<std::string>()) + columnsep : columnsep;
				//rownums += (textout.length() - 1) + ";";
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
	std::cout << "ADAT KIÍRÁS!" << std::endl;
//std::cout << textout << std::endl;
 	return sign ? textout : textout;
}

inline std::string getSQLQuery(std::shared_ptr<pqxx::connection> NC, const char* querytext){
	return getSQLQuery(NC, querytext, "", std::string(1, '\0'), true, true);
}

inline bool isJogosult(std::shared_ptr<pqxx::connection> NC, std::string gnndump, std::string keynames){
		std::string hh =
			"SELECT set_config('app.token', '"+ gnndump +"', false);\n" +
//			"SELECT set_config('app.service, '" + "" + "', false);\n')" +
			"" +
			""  
//			"select * from sysadmin.hasaccesstogroupview(" + gnndump + std::string(", '\?',  '") + keynames + "')"
		;

		std::string qre = getSQLQuery(NC, hh.c_str(), "", "", false, false); // Get check 1.
		return true;//qre.length() > 0 ? qre[0] == 't' : 0;
}

inline bool isCsChar(char CharC){
	return ((((unsigned)CharC - 65)) < 58 && (((unsigned)CharC - 91) > 4)) || CharC == 95;
}

inline std::string getTextWithJustChars(std::string text){
	std::string out = "";
	for (int i = 0; text[i] != '\0'; i++){
		if(isCsChar(text[i])) out += text[i];
	}
	return out;
}

inline std::string insertColumns(int &i, std::string &text){
	std::cout << "PlatonC: " << i << std::endl;
	std::cout << text << std::endl;
	std::string out = ""; //isJogosult
	for(; text[i] != '\0'; i++){
		if(isCsChar(text[i]) || text[i] == ',') out += text[i];
	}
	return out;
}

/*union strLength{
	int len;
	char lenc[4];
}
*/

inline bool setLimn(std::string_view &out, int &boole, uint32_t &limn, int &i, std::string &text){
	int lkn = i + 4;
	//i-=4;
	std::cout << text << std::endl;
	std::cout << "i: " << i << " Limn: " << limn << " Text.Length: " << text.length() << std::endl;
	if(lkn < text.length()){
		int novel = 0;
		// memcpy(&limn, &text[i], 4);
		for(; i < lkn; i++){
			unsigned char b = text[i];
			std::cout << +b << "|";
			limn |= uint32_t(b) << novel;
			novel += 8;
		}
		std::cout << std::endl;
		boole |= 1 << 2;
		std::cout << "copy is succesful" << std::endl;
	}
//	i += 4;	
	//i2 = i;
	std::bitset<32> z0(limn);
	std::cout << z0 << std::endl;
	std::bitset<32> z1(boole);
	std::cout << z1 << std::endl;
	
	std::cout << "i: " << i << " Limn: " << limn << " Text.Length: " << text.length() << std::endl;
	if(limn + i > text.length()){
		boole &= 0xFFFFFFFB;
		std::cout << "limn-problem" << std::endl;
	}
	else{
		out = string_view(&text[i], limn);
		limn += i;
	}
//	std::string out = "-";//"(";
	std::cout << "Itt jársz" << std::endl;
	std::bitset<32> z(boole);
	std::cout << z << std::endl;
	return true;
}

inline bool isInNumber(char &ch){
	return ((unsigned)ch - 42) < 16 || ch == 'e' || ch == 'E';
}

inline bool checkEqTxT(const char* nll, int &i, uint32_t &limn, std::string &text){
	bool both = true;
	int j = 1;
	if(text[i] == 'n'){
		i++;
		for(; j < 4 && i < limn && both; j++, i++){
			if(text[i] != nll[j]) both = false;
		}
		i--;
	} 
	return j > 3;
}

inline bool getUpdateSets(std::string_view &out, int &i, std::string &text){
	std::cout << "YEE: " << text << std::endl;
	//std::string out = "SET ";
	bool change = true;
	int boole = 0;
	uint32_t limn = 0;
	setLimn(out, boole, limn, i, text);
	for(; i < limn && ((boole >> 2) & 1); i++){
		std::cout << "YEin" << std::endl;
		if(!((boole >> 1) & 1)){
			std::cout << "YEcin: " << i << " Ch: " << text[i] << " - " << (unsigned)text[i] << std::endl;
			std::cout << "YEout-: " << i << std::endl;
/*			for(;text[i] > 1; i++){
				std::cout << "YEcine: " << i << " Ch: " << text[i] << " - " << (unsigned)text[i] << std::endl;
*/			if(text[i] == '=') boole |= (1 << 1);
			else if(!isCsChar(text[i])) boole &= 0xFFFFFFFB;
//			}
		}
		else{
			if(boole & 1){
				std::cout << "YEoutr: " << i << std::endl;
				for(; i < limn && (boole & 1); i++) 
					if(text[i] == '\''){ 
						boole &= 0xFFFFFFFE; // inline for
				//		i--;
					}
			}
			std::cout << "YEgin: " << i << " Ch: " << text[i] << " - " << (unsigned)text[i] << std::endl;
			if (!(boole & 1) && i < limn){
				std::cout << "YEoutj: " << i << std::endl;
				if(text[i] == '\'') boole |= 1;
				else if (text[i] == ',') boole &= 0xFFFFFFFD;
				else if (!isInNumber(text[i]) && !checkEqTxT("null", i, limn, text)) boole &= 0xFFFFFFFB;
			}
			//std::cout << "YEout: " << i << std::endl;
			//std::cout << i << std::endl;
		}
		std::cout << "YEt: " << i << std::endl;
	}
	std::cout << "YEr: " << i << ": " << out << std::endl;
	return ((boole >> 2) & 1);
}

inline bool methValues(std::string_view &insval, int &i, std::string &text){
	int boole=0;
	uint32_t limn = 0;
	setLimn(insval, boole, limn, i, text);
	for(; i < limn && (boole >> 2) & 1; i++){
		if(boole & 1){
			for(; i < limn && (boole & 1); i++) 
				if(text[i] == '\''){ 
					boole &= 0xFFFFFFFE; // inline for
				}
		}
		if(!(boole & 1) && i < limn){
			if(text[i] == '\'') boole |= 1;
			else if (!isInNumber(text[i]) && !checkEqTxT("null", i, limn, text)) boole &= 0xFFFFFFFB;
		}
		std::cout << "YEout: " << i << std::endl;
		std::cout << i << std::endl;
	}
	return (boole >> 2) & 1;
}

bool metha(std::string &out, int index, int outi, std::string dbthings, std::string transedschema, std::string transedtablename, int offset, int limit, int row){
	bool both = 1;
	std::string inscol = "";
	std::string_view insval = "";
	std::string_view upsets = "";
	std::cout << "Zsindex: " << index << std::endl;
	if((unsigned)index - 5 < 2){
		int szamlal = 0;
		 szamlal += methValues(insval, outi, dbthings);
		 std::cout << szamlal << std::endl;
		 both = szamlal == 1;
	}
	else if(index == 2){
		int szamlal = 0;
		std::cout << "DBThings: " << dbthings << "a\0\0a" << std::endl;
		inscol = insertColumns(outi, dbthings);
		outi += 1;
		szamlal += methValues(insval, outi, dbthings);
		std::cout << szamlal << std::endl;
		both = szamlal == 1;
	}
	else if(index == 4){
		int szamlal = 0;
//		outi++;
		szamlal += getUpdateSets(upsets, outi, dbthings);
		both = szamlal == 1;
	}
	if(both){
		std::array<std::string, 7> queries = {
			//select
			"select * from "+ transedschema + "." + transedtablename + ";",
			"select * from "+ transedschema + "." + transedtablename + " OFFSET " + std::to_string(offset) + " LIMIT " + std::to_string(limit) + ";",
			//insert
			"insert into " + transedschema + "." + transedtablename + " (" + inscol + ") values (" + std::string(insval) + ") returning *;",
			//delete
			"delete from "+ transedschema + "." + transedtablename +
				"\nwhere " + transedschema + "." + transedtablename+".id = " + std::to_string(row) + ";",
			//update
			"update " + transedschema + "." + transedtablename + "\nSET " + std::string(upsets) +
				"\nwhere " + transedschema + "." + transedtablename+".id = " + std::to_string(row) + " returning *;",
			"select " + transedschema + "." + transedtablename + " (" + std::string(insval) + ");", // Method call 1
			"select * from " + transedschema + "." + transedtablename + " (" + std::string(insval) + ");", // Method call 2
		};
		out = queries[index];
	}
	return both;
}

inline crow::response execFormat(
	PoolDBConnection& poolDB, 
	int caseindex, 
	std::string reqb, 
	std::string schemaname, 
	std::string tablename, 
	int offset, 
	int limit, 
	int row
){
	std::shared_ptr<pqxx::connection> NC = poolDB.getDBConn();
	int outi = 0;
	std::string out = "-";
	bool resnum = isJogosult(NC, "5", schemaname);
	if(resnum){
		std::string dbthings = reqb;
		std::cout << "DBThings: " << dbthings << std::endl;
//		setSessionValues(NC, outi, dbthings);
		std::string queryText = "";
		out = metha(queryText, caseindex, outi, dbthings, schemaname, tablename, offset, limit, row) ? 
		 	getSQLQuery(NC, queryText.c_str()) : "-";
/*		std::string ntext = "";
		for(int i = 0; i < out.size(); i++){
			ntext += std::to_string((unsigned)out[i]);
			ntext += ";";
		}
		std::cout << ntext << std::endl;
*/
		std::cout << "Out:\n" << out << std::endl;
		poolDB.giveBackConnect(NC);
	}
	return crow::response(out != "-" ? 200 : 500, out);
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
	crow::SimpleApp app;
    if (poolDB.active_connections > 0) {
        cout << "Opened database successfully: " << serviceDBName << endl;
		
		CROW_ROUTE(app, "/gettable/<string>/<string>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename
		) -> crow::response{
			return execFormat(poolDB, 0, req.body, getTextWithJustChars(schema), getTextWithJustChars(tablename), 0, 0, 0);	
			
		});
		
		CROW_ROUTE(app, "/gettable/<string>/<string>/<int>/<int>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename,
			const int offset,
			const int limit
		) -> crow::response{
			return execFormat(poolDB, 1, req.body, getTextWithJustChars(schema), getTextWithJustChars(tablename), offset, limit, 0);	
		});

		CROW_ROUTE(app, "/insert/<string>/<string>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename
		) -> crow::response{
			std::cout << req.body << std::endl;
			return execFormat(poolDB, 2, req.body, getTextWithJustChars(schema), getTextWithJustChars(tablename), 0, 0, 0);
		});

		CROW_ROUTE(app, "/delete/<string>/<string>/<int>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename,
			const int row
		) -> crow::response{
			return execFormat(poolDB, 3, req.body, getTextWithJustChars(schema), getTextWithJustChars(tablename), 0, 0, row);
		});

		CROW_ROUTE(app, "/update/<string>/<string>/<int>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename,
			const int row
		) -> crow::response{
			return execFormat(poolDB, 4, req.body, getTextWithJustChars(schema), getTextWithJustChars(tablename), 0, 0, row);
		});
		
		CROW_ROUTE(app, "/methscal/<string>/<string>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename
		) -> crow::response{
			return execFormat(poolDB, 5, req.body, getTextWithJustChars(schema), getTextWithJustChars(tablename), 0, 0, 0);
		});
		
		CROW_ROUTE(app, "/meth/<string>/<string>").methods("POST"_method)([](
			const crow::request& req,
			const std::string schema,
			const std::string tablename
		) -> crow::response{
			return execFormat(poolDB, 6, req.body, getTextWithJustChars(schema), getTextWithJustChars(tablename), 0, 0, 0);
		});
//      C.disconnect();
    } else {
        cout << "Can't open database" << endl;
        return 1;
    }	
	
	app.port(crowPort).multithreaded().run();
  	return 0;
}
