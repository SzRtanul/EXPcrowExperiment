#include "crow.h"
#include <iostream>
#include <cstring>
#include <mariadb/mysql.h>
#include "crow/middlewares/cors.h"

/*
struct connection_details{
    const char *server, *user, *password, *database;
};struct

MYSQL* mysql_connection_setup(struct connection_details mysql_details){
    MYSQL *connection = mysql_init(NULL);
    if(!mysql_real_connection(connection, mysql_details.server, mysql_details.user, mysql_details.password, 
			    mysql_details.database, 0, NULL, 0)){
    	std::cout << "Connection Failed: " >> mysql_error(connection) << std::endl;
        exit(1);	
    }
    return connection;
}

MYSQL_RES mysql_execute_query(MYSQL *connection, const char *sql_query){
   if(mysql_query(connection, sql_query)){
       std::cout << "Mysql query error: " << mysql_error(*connection) << std::endl;
   }
   return mysql_use_result(*connection);
}
*/

int exa=0;
int main(){
		
/*    MYSQL *con;
    MYSQL_RES *res;
    MYSQL_ROW row;

    struct connection_details mysqlD;
    mysqlD.server="localhost";
    mysqlD.user="admin";
    mysqlD.password="Nincs1";
    mysqlD.database="example";

    con=mysql_connection_setup(mysqlD);
    res= mysql_execute_query(con, "CREATE TABLE \"faszom\"");
    std::cout << "elvileg sikerült" << std::endl;
    mysql_free_result(res);
    mysql_close(con);*/

    crow::App<crow::CORSHandler> app;
	auto& cors = app.get_middleware<crow::CORSHandler>();
    //define your crow application			    
    //define your endpoint at the root directory
	
    CROW_ROUTE(app, "/")([](){return "Hello world";});
    CROW_ROUTE(app, "/exa")([](){return std::to_string(exa);});
    CROW_ROUTE(app, "/exa/<int>").methods("POST"_method)([](int a){ 
		exa=a; 
		return std::to_string(0b0111101); 
	});
    //set the port, set the app to run on multiple threads, and run the app
    app.port(18080).multithreaded().run();
    return 0;
}
