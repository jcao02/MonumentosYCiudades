#include "Monument.hpp"

#define PLACES 6
std::unordered_map<std::string, int> Monument::places;
char *Monument::countries[] = { "Italia", "Egipto", 
								"Chile", "Mexico", 
								"Inglaterra", "EstadosUnidos" };
char *Monument::cities[]    = { "Pisa", "Guiza", 
						        "IslaDePascua", "ChitchenItza", 
							    "Londres", "CiudadDeNuevaYork"};	

Monument::Monument(void)
{
	int id, i;

	for (id = 0, i = 0; i < PLACES; ++i, ++id) {
		places[countries[i]] = id;
		places[cities[i]] = id;
	}
}


Monument::~Monument(void)
{
}


bool Monument::match(std::string place1, std::string place2) {
	return places[place1] == places[place2]; 
}

bool Monument::isCountry(std::string place) {
	int i; 

	for (i = 0; i < PLACES; ++i) {
		if (strcmp(place.c_str(), countries[i]) == 0) 
			return true;
	}
	return false; 
}
bool Monument::isCity(std::string place) {
	int i; 

	for (i = 0; i < PLACES; ++i) {
		if (strcmp(place.c_str(), cities[i]) == 0) 
			return true;
	}
	return false; 
}