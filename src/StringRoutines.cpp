#include "StringRoutines.h"
#include "Messages.h"
#include <cmath> // log10
#include <ctime> // for TimeString()
#include <sstream> // istringstream, ostringstream
#include <algorithm> //sort, unique

// DigitWidth()
/** \return the number of characters necessary to express the given digit. */
int StringRoutines::DigitWidth(long int numberIn) {
  double numf;
  int minusSign = 0;

  if (numberIn == 0L) return 1;
  if (numberIn < 0L) {
    numf = (double)(-numberIn);
    minusSign = 1;
  } else
    numf = (double) numberIn;

  numf = log10( numf );
  ++numf;
  // The cast back to long int implicitly rounds down
  int numi = (int)numf;
  return (minusSign + numi);
}

// integerToString()
std::string StringRoutines::integerToString(int i) {
  std::ostringstream oss;
  oss << i;
  return oss.str();
}

// integerToString()
std::string StringRoutines::integerToString(int i, int width) {
  std::ostringstream oss;
  oss.fill('0');
  oss.width( width );
  oss << std::right << i;
  return oss.str();
}

// validInteger()
bool StringRoutines::validInteger(std::string const &argument) {
  if (argument.empty()) return false;
  std::string::const_iterator c;
  if (argument[0]=='-' || argument[0]=='+') {
    c = argument.begin()+1;
    if (c == argument.end()) return false;
  } else
    c = argument.begin();
  for (; c != argument.end(); ++c)
    if (!isdigit(*c)) return false;
  return true;
}

// convertToInteger()
/** Convert the input string to an integer. */
int StringRoutines::convertToInteger(std::string const &s) {
  std::istringstream iss(s);
  long int i;
  iss >> i;
  if (iss.fail()) {
    Messages::ErrorMsg("Could not convert '%s' to integer.\n", s.c_str());
    return 0;
  }
    //throw BadConversion("convertToInteger(\"" + s + "\")");
  return (int)i;
}

/** Convert the input string to a double. */
double StringRoutines::convertToDouble(std::string const &s) {
  std::istringstream iss(s);
  double d;
  iss >> d;
  if (iss.fail()) {
    Messages::ErrorMsg("Could not convert '%s' to double.\n", s.c_str());
    return 0;
  }
    //throw BadConversion("convertToDouble(\"" + s + "\")");
  return d;
}


std::string StringRoutines::doubleToString(double d) {
  std::ostringstream oss;
  oss << d;
  return oss.str();
}

// RemoveTrailingWhitespace()
/// Remove any trailing whitespace from string.
void StringRoutines::RemoveTrailingWhitespace(std::string &line) {
  if (line.empty()) return;
  std::locale loc;
  int p = (int)line.size() - 1;
  while (p > -1 && (isspace(line[p],loc) || line[p]=='\n' || line[p]=='\r'))
    --p;
  line.resize(p + 1);
}

std::string StringRoutines::NoTrailingWhitespace(std::string const& line) {
  std::string duplicate(line);
  RemoveTrailingWhitespace(duplicate);
  return duplicate;
}

/// Remove all whitespace from string.
void StringRoutines::RemoveAllWhitespace(std::string& line) {
  if (line.empty()) return;
  std::string tmp( line );
  line.clear();
  for (std::string::const_iterator it = tmp.begin(); it != tmp.end(); ++it) {
    if (isspace(*it) || *it == '\n' || *it == '\r') continue;
    line += *it;
  }
}

/** Given an argument containing strings separated by "," construct a list of strings.
  * Remove any duplicates.
  * \return 0 on success, 1 on error.
  */
std::vector<std::string> StringRoutines::CommaSep(std::string const& ArgIn)
{
  using namespace Messages;

  typedef std::vector<std::string> Sarray;
  Sarray CommaList;
//  Msg("DEBUG: ParseRange(%s)\n", ArgIn.c_str());
  if (ArgIn.empty()) return CommaList;

  // Split range by comma
  std::string token;
  for (std::string::const_iterator it = ArgIn.begin(); it != ArgIn.end(); ++it)
  {
    if (*it == ',') {
      if (!token.empty()) {
        CommaList.push_back( token );
        token.clear();
      }
    } else
      token += *it;
  }
  if (!token.empty()) {
    CommaList.push_back( token );
    token.clear();
  }
  // Remove duplicates by sorting and keeping only unique.
  std::sort(CommaList.begin(), CommaList.end());
  Sarray::const_iterator it = std::unique( CommaList.begin(), CommaList.end() );
  CommaList.resize( it - CommaList.begin() );

  return CommaList;
}

/** Given an argument containing numbers separated by "," (concatentation), and 
  * "-" (number range), construct an ordered list of numbers corresponding to 
  * the argument. Remove any duplicate numbers.
  * \return 0 on success, 1 on error.
  */
std::vector<int> StringRoutines::ParseRange(std::string const& ArgIn)
{
  using namespace Messages;
  //std::string arg;
  //int R[2], upper;

//  Msg("DEBUG: ParseRange(%s)\n", ArgIn.c_str());
  std::vector<int> rangeList;
  if (ArgIn.empty()) return rangeList;

  typedef std::vector<std::string> Sarray;
  // Split range by comma
  Sarray CommaList;
  std::string token;
  for (std::string::const_iterator it = ArgIn.begin(); it != ArgIn.end(); ++it)
  {
    if (*it == ',') {
      if (!token.empty()) {
        CommaList.push_back( token );
        token.clear();
      }
    } else
      token += *it;
  }
  if (!token.empty()) {
    CommaList.push_back( token );
    token.clear();
  }

  for (Sarray::const_iterator tkn = CommaList.begin(); tkn != CommaList.end(); ++tkn) {
//    Msg("DEBUG: Token= %s\n", tkn->c_str());
    // Split token by dash if possible
    std::size_t pos = tkn->find_first_of("-");
    if (pos != std::string::npos) {
      std::string lower = tkn->substr(0,pos);
      if (!validInteger(lower)) {
        ErrorMsg("Invalid number starting range: %s\n", lower.c_str());
        return std::vector<int>();
      }
      std::string upper = tkn->substr(pos+1,tkn->size());
      if (!validInteger(upper)) {
        ErrorMsg("Invalid number ending range: %s\n", upper.c_str());
        return std::vector<int>();
      }
//      Msg("DEBUG:\tfrom %s to %s\n", lower.c_str(), upper.c_str());
      int beg = convertToInteger(lower);
      int end = convertToInteger(upper);
      for (int ii = beg; ii <= end; ii++)
        rangeList.push_back( ii );
    } else {
      if (!validInteger(*tkn)) {
        ErrorMsg("Invalid number in range: %s\n", tkn->c_str());
        return std::vector<int>();
      }
      rangeList.push_back( convertToInteger(*tkn) );
    }
  }
  // Remove duplicates by sorting and keeping only unique.
  std::sort(rangeList.begin(), rangeList.end());
  std::vector<int>::const_iterator it = std::unique( rangeList.begin(), rangeList.end() );
  rangeList.resize( it - rangeList.begin() );
//  Msg("DEBUG: Range:");
//  for (std::vector<int>::const_iterator it = rangeList.begin(); it != rangeList.end(); ++it)
//    Msg(" %i", *it);
//  Msg("\n");

  return rangeList;
}

// NOTE: I think this serves as a great example of how printf syntax is way
//       easier than iostream stuff (same printf command is only 3 lines). -DRR
std::string StringRoutines::TimeString() {
  time_t rawtime;
  time( &rawtime );
  struct tm timeinfo;
# ifdef _WIN32
  localtime_s( &timeinfo, &rawtime );
# else
  localtime_r( &rawtime, &timeinfo );
# endif
  std::ostringstream oss;
  oss.fill('0');
  oss.width(2);
  oss << std::right << timeinfo.tm_mon+1;
  oss.put('/');
  oss.width(2);
  oss << std::right << timeinfo.tm_mday;
  oss.put('/');
  oss.width(2);
  oss << std::right << timeinfo.tm_year%100;
  oss.put(' ');
  oss.width(2);
  oss << std::right << timeinfo.tm_hour;
  oss.put(':');
  oss.width(2);
  oss << std::right << timeinfo.tm_min;
  oss.put(':');
  oss.width(2);
  oss << std::right << timeinfo.tm_sec;
  return oss.str();
}
