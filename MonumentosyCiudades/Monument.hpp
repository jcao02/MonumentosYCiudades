#ifndef MONUMENTOS_Y_CIUDADES_HPP
#define MONUMENTOS_Y_CIUDADES_HPP

#include <iostream>
#include <string>
#include<unordered_map>

#include "vrml_object.hpp"

typedef std::unordered_map<std::string, int> Map;
class Monument {
public:
	Monument(void);
	~Monument(void);

	static void loadModels();
	static bool match(char *place1, char *place2);
	static bool isCountry(std::string place);
	static bool isCity(std::string place);
private:
	static std::vector<std::string> countries;
	static std::vector<std::string> cities;
};
#endif
