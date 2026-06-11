#ifndef INC_PROJECTFILELINE_H
#define INC_PROJECTFILELINE_H
#include <string>
namespace MdManager {
/// Use to ensure the ordering in the projects file is maintained.
class ProjectFileLine {
  public:
    ProjectFileLine() : prj_(-1) {}
    /// CONSTRUCTOR - Comment only
    ProjectFileLine(std::string const& line) : prj_(-1), comment_(line) {}
    /// CONSTRUCTOR - Project index
    ProjectFileLine(int pidx) : prj_(pidx) {}

    int ProjIdx() const { return prj_; }
    std::string const& Comment() const { return comment_; }
  private:
    int prj_;
    std::string comment_;
};
}
#endif
