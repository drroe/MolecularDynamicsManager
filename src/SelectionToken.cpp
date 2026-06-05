#include "SelectionToken.h"
#include "Messages.h"
#include "StringRoutines.h"
#include <cctype>

using namespace MdManager;
using namespace Messages;

/** CONSTRUCTOR */
SelectionToken::SelectionToken()
{}

static inline bool advance(std::string::const_iterator& it, std::string::const_iterator const& end)
{
  ++it;
  if (it == end) return true;
  if (!isdigit(*it)) return true;
  return false;
}

static inline bool validChar(char cIn)
{
  if (cIn == ',' || cIn == '-' || isdigit( cIn )) return true;
  ErrorMsg("Invalid character: %c\n", cIn);
  return false;
}

/** Set up token from selection string. */
int SelectionToken::SetFromStr(std::string const& strIn)
{
  using namespace StringRoutines;

  projectNumbers_.clear();
  systemNumbers_.clear();
  runNumbers_.clear();
  statuses_.clear();

  //enum TknType { NONE=0, PROJECT };
  //TknType iToken = NONE;
  std::string currentToken;
  for (std::string::const_iterator it = strIn.begin(); it != strIn.end(); ++it)
  {
    if (*it == 'p') {
      // Allow either a single number or a range
      if (advance(it, strIn.end())) {
        ErrorMsg("Ran out of tokens for 'p'.\n");
        return 1;
      }
      currentToken.clear();
      std::string::const_iterator jt = it;
      for (; jt != strIn.end(); ++jt) {
        if (*jt == 's' || *jt == 'r' || isspace(*jt)) break;
        if (!validChar(*jt)) return 1;
        currentToken += *jt;
      }
      Msg("DEBUG: Project token: %s\n", currentToken.c_str());
      projectNumbers_ = ParseRange( currentToken );
      if (projectNumbers_.empty()) {
        ErrorMsg("Could not process project token '%s'\n", currentToken.c_str());
        return 1;
      }
      it = jt;
      //iToken = PROJECT;
    }

    if (it == strIn.end()) break;
  }

  Msg("DEBUG: Project numbers:");
  for (Iarray::const_iterator it = projectNumbers_.begin(); it != projectNumbers_.end(); ++it)
    Msg(" %i", *it);
  Msg("\n");
  return 0;
}
