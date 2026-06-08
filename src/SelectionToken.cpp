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

RunStatus::StatusType SelectionToken::strToStatus(std::string const& tokenIn)
{
  RunStatus::StatusType ret = RunStatus::UNKNOWN;
  if (tokenIn == "r" || tokenIn == "run" || tokenIn == "running")
    ret = RunStatus::IN_PROGRESS;
  else if (tokenIn == "q" || tokenIn == "que" || tokenIn == "queued")
    ret = RunStatus::IN_QUEUE;
  else if (tokenIn == "p" || tokenIn == "pen" || tokenIn == "pending")
    ret = RunStatus::PENDING;
  else if (tokenIn == "ready")
    ret = RunStatus::READY;
  else if (tokenIn == "c" || tokenIn == "com" || tokenIn == "complete")
    ret = RunStatus::COMPLETE;
  else if (tokenIn == "i" || tokenIn == "inc" || tokenIn == "incomplete")
    ret = RunStatus::INCOMPLETE;
  else
    ErrorMsg("Unrecognized status: %s\n", tokenIn.c_str());
  return ret;
}

/** Set up token from selection string. */
int SelectionToken::SetFromStr(std::string const& strIn)
{
  using namespace StringRoutines;

  projectNumbers_.clear();
  systemNumbers_.clear();
  runNumbers_.clear();
  statuses_.clear();

  std::vector<bool> statusSelected( (int)RunStatus::COMPLETE, false );

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
        if (*jt == 's' || *jt == 'r' || *jt == '%' || isspace(*jt)) break;
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

    if (*it == 's') {
      // Allow either a single number or a range
      if (advance(it, strIn.end())) {
        ErrorMsg("Ran out of tokens for 's'.\n");
        return 1;
      }
      currentToken.clear();
      std::string::const_iterator jt = it;
      for (; jt != strIn.end(); ++jt) {
        if (*jt == 'r' || *jt == '%' || isspace(*jt)) break;
        if (!validChar(*jt)) return 1;
        currentToken += *jt;
      }
      Msg("DEBUG: System token: %s\n", currentToken.c_str());
      systemNumbers_ = ParseRange( currentToken );
      if (systemNumbers_.empty()) {
        ErrorMsg("Could not process system token '%s'\n", currentToken.c_str());
        return 1;
      }
      it = jt;
    }
    if (it == strIn.end()) break;

    if (*it == 'r') {
      // Allow either a single number or a range
      if (advance(it, strIn.end())) {
        ErrorMsg("Ran out of tokens for 'r'.\n");
        return 1;
      }
      currentToken.clear();
      std::string::const_iterator jt = it;
      for (; jt != strIn.end(); ++jt) {
        if (*jt == '%' || isspace(*jt)) break;
        if (!validChar(*jt)) return 1;
        currentToken += *jt;
      }
      Msg("DEBUG: Run token: %s\n", currentToken.c_str());
      runNumbers_ = ParseRange( currentToken );
      if (runNumbers_.empty()) {
        ErrorMsg("Could not process run token '%s'\n", currentToken.c_str());
        return 1;
      }
      it = jt;
    }
    if (it == strIn.end()) break;

    if (*it == '%') {
      // Status query
      ++it;
      if (it == strIn.end()) {
        ErrorMsg("Ran out of tokens for '%%'.\n");
        return 1;
      }
      currentToken.clear();
      std::string::const_iterator jt = it;
      for (; jt != strIn.end(); ++jt) {
        if (isspace(*jt)) break;
        // TODO check valid
        currentToken += *jt;
      }
      std::vector<std::string> STATS = CommaSep( currentToken );
      for (std::vector<std::string>::const_iterator st = STATS.begin();
                                                    st != STATS.end(); ++st)
      {
        Msg("DEBUG: Status token: %s\n", st->c_str());
        RunStatus::StatusType rstat = strToStatus( *st );
        if (rstat == RunStatus::UNKNOWN) {
          return 1;
        }
        statusSelected[(int)rstat] = true;
      }
      it = jt;
    }
    if (it == strIn.end()) break;

  }

  Msg("DEBUG: Project numbers:");
  for (Iarray::const_iterator it = projectNumbers_.begin(); it != projectNumbers_.end(); ++it)
    Msg(" %i", *it);
  Msg("\n");
  Msg("DEBUG: System numbers:");
  for (Iarray::const_iterator it = systemNumbers_.begin(); it != systemNumbers_.end(); ++it)
    Msg(" %i", *it);
  Msg("\n");
  Msg("DEBUG: Run numbers:");
  for (Iarray::const_iterator it = runNumbers_.begin(); it != runNumbers_.end(); ++it)
    Msg(" %i", *it);
  Msg("\n");
  Msg("DEBUG: Statuses:");
  for (unsigned int idx = 1; idx < statusSelected.size(); idx++) {
    if (statusSelected[idx])
      Msg(" %s", RunStatus::statusString((RunStatus::StatusType)idx));
  }
  Msg("\n");


  return 0;
}
