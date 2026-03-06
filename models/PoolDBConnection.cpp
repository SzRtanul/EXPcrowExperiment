#include <iostream>
#include <chrono>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>

using namespace std;

struct PoolDBConnection{
	std::string conninfo;
	int min_size;
	int max_size;
	int active_connections;
	std::mutex mtx;
	std::queue<std::shared_ptr<pqxx::connection>> pool;
	const std::chrono::milliseconds scale_threshold{2000};

	PoolDBConnection(std::string conninfo, int min_size, int max_size){
		this->active_connections = 0;
		this->conninfo = conninfo;
		this->min_size = min_size;
		this->max_size = max_size;
		this->create_connection();
	}

	inline bool create_connection(){
		auto conn = getDBConn();
		if (conn->is_open()){
			giveBackConnect(conn);
		}
		else{
			std::cout << "Connection to database is failed;" << endl;
		}
		return conn->is_open();
	}

	std::shared_ptr<pqxx::connection> getDBConn(){
		std::lock_guard<std::mutex> lock(mtx);
		std::shared_ptr<pqxx::connection> conn = nullptr;
		if((!pool.empty() && !pool.front()->is_open())){
			std::cout << "ENEN" << endl;
			while(!pool.empty()){
				pool.pop();
				this->active_connections--;
			}
		}
		if(pool.empty()){
			std::cout << "ENEC" << endl;
			for(int i = 0; i < min_size; i++){
				pool.push(std::make_shared<pqxx::connection>(conninfo));
				active_connections++;
			}
			conn = pool.front();
			pool.pop();
			active_connections--;
			std::cout << "ENEC" << endl;
		}
		else{
			std::cout << "ENE" << endl;
			conn = pool.front(); 
			pool.pop();
			active_connections--;
			std::cout << "ENE" << endl;
		}
		return conn;	
	}

	void giveBackConnect(std::shared_ptr<pqxx::connection> conn){
		std::lock_guard<std::mutex> lock(mtx);
		if(active_connections > 0 && conn && conn->is_open()){
			pool.push(conn);
			active_connections++;
		}
		else if(active_connections > 0){
			active_connections++;
		}
	}
};

