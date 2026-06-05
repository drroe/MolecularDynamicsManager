#include "SelectionToken.h"

using namespace MdManager;

/** CONSTRUCTOR */
SelectionToken::SelectionToken()
{}

static inline bool advance(std::string::const_iterator& it, std::string::const_iterator const& end)
{
  ++it;
  return (it == end);
}

/** Set up token from selection string. */
int SelectionToken::SetFromStr(std::string const& strIn)
{
  projectNumbers_.clear();
  systemNumbers_.clear();
  runNumbers_.clear();
  statuses_.clear();

  enum TknType { NONE=0, PROJECT };
  TknType iToken = NONE;
  for (std::string::const_iterator it = strIn.begin(); it != strIn.end(); ++it)
  {
    if (*it == 'p') {
      if (advance(it, strIn.end())) {
        ErrorMsg("Ran out of tokens for 'p'.\n");
        return 1;
      }
      iToken = PROJECT;
    }
  }

  return 0;
}
