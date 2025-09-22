#pragma once
#include <iostream>

int entraceMethod(
	int crowPort,
	int minDBConn,
	int maxDBConn,
	std::string postgresDBlocation, 
	std::string postgresDBusername, 
	std::string postgresDBpassword, 
	std::string postgresDBport, 
	std::string serviceDBName
);
