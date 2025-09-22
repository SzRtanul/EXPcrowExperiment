#include <crow/json.h>
#include "core.h"
#include <fstream>  // kell az std::ifstreamhez
#include <string>

int main(){
	std::ifstream f(CONFIG_FILE);
	std::string content((std::istreambuf_iterator<char>(f)),
	std::istreambuf_iterator<char>());
	auto x = crow::json::load(content);

	entraceMethod(
		x["crowPort"].i(),
		x["minDBConn"].i(),
		x["maxDBConn"].i(),
		x["postgresDBlocation"].s(),
		x["postgresDBusername"].s(),
		x["postgresDBpassword"].s(),
		x["postgresDBport"].s(),
		x["serviceDBName"].s()
	);
	return 0;
}
