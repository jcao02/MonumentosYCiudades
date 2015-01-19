#include "Monument.hpp"
#include<algorithm>

#define PLACES 6

char *countries_arr[] = { "Italia", "Egipto", 
						   "Chile", "Mexico", 
						   "Inglaterra", "EstadosUnidos" };

char *cities_arr[] = { "Pisa", "Guiza", 
					   "IslaDePascua", "ChitchenItza", 
					   "Londres", "CiudadDeNuevaYork"};

std::vector<std::string> Monument::countries(countries_arr, countries_arr + 6);
std::vector<std::string> Monument::cities(cities_arr, cities_arr + 6);	

Monument::Monument(void)
{
}


Monument::~Monument(void)
{
}


bool Monument::match(char *place1, char *place2) {
	size_t pos1, pos2;
	if (isCountry(place1)) {
		pos1 = std::distance(countries.begin(), find(countries.begin(), countries.end(), place1)); 
	} else {
		pos1 = std::distance(cities.begin(), find(cities.begin(), cities.end(), place1)); 
	}

	if (isCountry(place2)) {
		pos2 = std::distance(countries.begin(), find(countries.begin(), countries.end(), place2)); 
	} else {
		pos2 = std::distance(cities.begin(), find(cities.begin(), cities.end(), place2)); 
	}
	return pos1 == pos2; 
}

bool Monument::isCountry(std::string place) {
	return std::find(countries.begin(), countries.end(), place) != countries.end();
}
bool Monument::isCity(std::string place) {
	return std::find(cities.begin(), cities.end(), place) != cities.end();
}