#include "User.hpp"


char* User::monument_arr[] =  { "maya", "pyramid", "pisa", "bigben", "maui", "liberty"}; 
std::set<std::string> User::monuments(monument_arr, monument_arr + 6);

User::User(int n /*= 3*/)
{
	_lifes = n;
}


User::~User(void)
{
}
