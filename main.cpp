#include "crow.h"
#include <iostream>
#include <cstring>
#include "crow/middlewares/cors.h"
#include <csignal>
#include <pqxx/pqxx>
#include <string>
#include <thread>
#include <chrono>

using namespace std;
using namespace pqxx;

pqxx::connection C = pqxx::connection(R"(dbname = testdb user=postgres password=test123 hostaddr=127.0.0.1 port=5432)");
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

inline std::string getSQLQuery(pqxx::work &W, std::string querytext){
    std::string textout = "";
    pqxx::result R = W.exec(querytext);
    for (const auto &row : R) {
        for(int i = 0; i < row.size(); i++){
            textout += getWithoutSpace(row[i].as<std::string>()) + ":::";
        }
        textout = textout.length() > 3 ? textout + ";;;\n" : "";
    }
	return textout;
}

int main(){		
    crow::App<crow::CORSHandler> app;
	auto& cors = app.get_middleware<crow::CORSHandler>();
    //pqxx::work W(C);

    if (C.is_open()) {
        cout << "Opened database successfully: " << C.dbname() << endl;
        std::signal(SIGINT, signal_handler);
        /*W.commit();*/
  //      std::cout << getSQLQuery(W, "SELECT id, name FROM users");
  //      std::cout << getSQLQuery(W, "create table if not exists users(id int, name char(30))");
  //      std::cout << getSQLQuery(W, "SELECT public.helloworld('Szabó Roland')");
		CROW_ROUTE(app, "/ujvacsora")([](){
			pqxx::work W(C);
        	return getSQLQuery(W, "SELECT public.helloworld('Szabó Roland')");
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
    app.port(18080).multithreaded().run();
    return 0;
}
