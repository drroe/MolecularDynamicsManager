#ifndef INC_SELECTIONTOKEN_H
#define INC_SELECTIONTOKEN_H
#include <vector>
#include <string>
#include "RunStatus.h"
namespace MdManager {
/// Used to select 1 or more runs
class SelectionToken {
    typedef std::vector<int> Iarray;
    typedef std::vector<RunStatus::StatusType> StatArray;
  public:
    /// CONSTRUCTOR
    SelectionToken();
    /// Set up token from a selection string
    int SetFromStr(std::string const&);
  private:
    Iarray projectNumbers_; ///< Valid project numbers
    Iarray systemNumbers_;  /// Valid system numbers
    Iarray runNumbers_;     ///< Valid run numbers
    StatArray statuses_;    ///< Valid statuses
};
}
#endif
