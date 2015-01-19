#ifndef MONUMENTOS_Y_CIUDADES_HPP
#define MONUMENTOS_Y_CIUDADES_HPP

#include <iostream>
#include <string>
#include<unordered_map>

#include "vrml_object.hpp"

class Monument {
public:
	Monument(void);
	~Monument(void);

	static void loadModels();
	static void renderSuccess(ObjectVRML_T);
	static void renderFailure(ObjectVRML_T);
	static bool match(std::string place1, std::string place2);
	static bool isCountry(std::string place);
	static bool isCity(std::string place);
private:
	static std::unordered_map<std::string, int> places;
	static char *countries[];
	static char *cities[];
	static int success;
	static int failure; 
};
#endif
