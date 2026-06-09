#include "Project.h"

/** Add system to project. Read from file. */
void Project::AddSystem(System const& s) {
  if (activeSystemIdx_ < 0) activeSystemIdx_ = 0;
    systems_.push_back( s );
  }

/** Add system to project. Created. */
void Project::AddNewSystem(std::string const& prefix, std::string const& dir, std::string const& desc) {
  if (activeSystemIdx_ < 0) activeSystemIdx_ = 0;
  systems_.push_back( System(prefix, dir, desc) );
  needsWrite_ = true;
}
