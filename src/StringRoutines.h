#ifndef INC_STRINGROUTINES_H
#define INC_STRINGROUTINES_H
#include <string>
#include <vector>
namespace StringRoutines {
// Functions for creating fixed-width digit strings.
int DigitWidth(long int);
std::string integerToString(int);
std::string integerToString(int,int);
bool validInteger(std::string const&);
int convertToInteger(std::string const&);
double convertToDouble(std::string const&);
std::string doubleToString(double);
/// Remove any trailing whitespace from string.
void RemoveTrailingWhitespace(std::string &);
/// \return string stripped of trailing whitespace.
std::string NoTrailingWhitespace(std::string const&);
/// Remove all whitespace from a string
void RemoveAllWhitespace(std::string&);
/// \return Array of strings from a comma-separate list
std::vector<std::string> CommaSep(std::string const&);
/// \return List of integers corresponding to a string with a range
std::vector<int> ParseRange(std::string const&);
/// \return String containing time/date
std::string TimeString();
}
#endif
