#ifndef MONUMENTOS_Y_CIUDADES_USER_HPP_
#define MONUMENTOS_Y_CIUDADES_USER_HPP_

#include<iostream>
#include<set>
#include<string>
#include<vector>

class User {
public:
	User(int n = 3);
	~User(void);
	bool won() { return _collection == monuments; };
	bool lost() { return _lifes == 0; };
	int lifes() { return _lifes; };
	std::set<std::string> collection() { return _collection; };
	void lifes(int n) { _lifes = n; };
	void collection(std::string monument) { _collection.insert(monument); };

private:
	int _lifes;
	std::set<std::string> _collection;
	static char* monument_arr[];
	static std::set<std::string> monuments;
};
#endif