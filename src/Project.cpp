#include "Project.h"
#include "Messages.h"
#include "FileRoutines.h"
#include "TextFile.h"

using namespace Messages;

/** Add system to project. Read from file. */
void Project::AddSystem(System const& s) {
  systems_.push_back( s );
  if (activeSystemIdx_ < 0) activeSystemIdx_ = 0;
}

/** Add system to project. Created. */
int Project::AddNewSystem(std::string const& prefix, std::string const& dir,
                          std::string const& desc, int debug)
{
  std::string fullpath;
  if (prefix.empty())
    fullpath.assign("./" + dir);
  else
    fullpath.assign(prefix + "/" + dir);
  // Check if the directory needs to be created.
  bool dir_exists = false;
  if (FileRoutines::fileExists( fullpath )) {
    if (!FileRoutines::IsDirectory( fullpath )) {
      ErrorMsg("System dir '%s' exists but is not a directory.\n", fullpath.c_str());
      return 1;
    }
    dir_exists = true;
    Msg("DEBUG: System dir '%s' exists\n", fullpath.c_str());
  } else {
    Msg("\tSystem dir '%s' does not exist.\n", fullpath.c_str());
    if (YesNoPrompt("Create system directory?")) {
      if (FileRoutines::CreateDir( fullpath )) return 1;
    } else {
      return 1;
    }
  }
  systems_.push_back( System(prefix, dir, desc, debug) );
  if (activeSystemIdx_ < 0) activeSystemIdx_ = 0;
  needsWrite_ = true;
  // Scan the directory if needed.
  if (dir_exists) {
    if (systems_.back().FindRuns()) return 1;
  }
  return 0;
}

/** Save project and systems to given file. */
int Project::SaveSystems(TextFile& outfile) {
  Msg("Saving project %s\n", pname_.c_str());
  outfile.Printf("project %s\n", pname_.c_str());
  // Save systems
  for (SystemArray::const_iterator it = systems_.begin(); it != systems_.end(); ++it)
  {
    Msg("Saving system %li\n", it-systems_.begin());
    outfile.Printf("system %s %s\n", it->SystemDirName().c_str(), it->SystemDescription().c_str());
  }
  if (needsWrite_) {
    needsWrite_ = false;
  }
  return 0;
}
