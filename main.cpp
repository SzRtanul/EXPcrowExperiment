#include "include/nlohmann/json.hpp"
#include "core.h"
#include <fstream>  // kell az std::ifstreamhez
#include <string>

using json = nlohmann::json;

int main(){
	std::ifstream f(CONFIG_FILE);
//	std::string content((std::istreambuf_iterator<char>(f)),
//	std::istreambuf_iterator<char>());
	
    json x;
    try {
        f >> x;  // közvetlenül betölti a JSON-t
    } catch (json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return 1;
    }

    // 2️⃣ Értékek kinyerése
    try {
        entraceMethod(
            x.at("crowPort").get<int>(),
            x.at("minDBConn").get<int>(),
            x.at("maxDBConn").get<int>(),
            x.at("postgresDBlocation").get<std::string>(),
            x.at("postgresDBusername").get<std::string>(),
            x.at("postgresDBpassword").get<std::string>(),
            x.at("postgresDBport").get<std::string>(),
            x.at("serviceDBName").get<std::string>()
        );
    } catch (json::type_error& e) {
        std::cerr << "JSON type error: " << e.what() << std::endl;
        return 1;
    } catch (json::out_of_range& e) {
        std::cerr << "JSON key not found: " << e.what() << std::endl;
        return 1;
    }	

	return 0;
}
