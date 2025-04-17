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
const char sqlsyntaxt[][20]{
	"SELECT",
	"FROM",
	"INNER",
	"JOIN",
	"ON",
	"OUTER",
	"RIGHT",
	"LEFT",
	"EXISTS",
	"IN",
	"NOT",
	"LIKE",
};

char bynarytree[][32]{
	"SFIJORLENL",
	"ERNOUIXN",
	"LONITGFK"
}

char bynwoher[][300][32]{
	{{1,5},{2,3}},
	{"", ""}
}


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

inline char* getTextWithJSONValues(const char text[]){
	int i = 0;
	std::string retn = "";
	bool haved = false;
	while(text[i] != '\0'){
		if(text[i] > 96 && text[i] < 123) text[i] -= 32;
		if(text[i] > 64 && text[i] < 91){
			haved = true;

			//std::cout << text[i] << ":" << (int)text[i] << std::endl;
			// Bináris fa összehasonlítás
		}
		for(; text[i] != '\0'; i++){
	
		}
	}

	return nullptr;
}

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
	getTextWithJSONValues("ABCDZabcz&$ -");
	return 0;
/*	
 *	const char array[][50] = {
		"ohIgen",
		"OHIGEN",
		"OHYea",
		"n1",
		"n2",
		"n3"
	};
	const char values[][50] = {
		//"oh",
		"n3"
	};
	std::cout << sizeof(values)/50 << endl;
	std::cout << "Működik?: " << isBenneVanRendezett(array, values, sizeof(array)/50, sizeof(values)/50) << endl;
	return 0;
*/
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

		CROW_ROUTE(app, "/callquery").methods("GET"_method)([](const crow::request& req){
			std::string columnames = "";
			std::string tablenames = "";
			std::string whereclause = "";
			std::string groupby = "";
			std::string having = "";

			pqxx::work W(C);
        	return getSQLQuery(W,  
				("SELECT " + columnames + 
				" FROM " + tablenames + 
				" INNER JOIN " +
				" WHERE " + whereclause + 
				" GROUP BY " + groupby +
				" HAVING " + having).c_str());
    	});

		CROW_ROUTE(app, "/deletefrom/<string>/<int>").methods("POST"_method)([](const crow::request& req, const std::string tablename, const int id){ // Egyszerű kulcsos táblák
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
		CROW_ROUTE(app, "/update/<string>/<int>").methods("POST"_method)([](const crow::request& req, const std::string tablename, const int id){ // Egyszerű kulcsos táblák
            // Miken
			// Mikre
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


		return "Megkaptam!";
	});
    app.port(18080).multithreaded().run();
  	return 0;
}
