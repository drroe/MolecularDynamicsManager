#include "Exec_Add.h"
#include "Messages.h"
#include "Manager.h"
#include "Cols.h"
#include "FileRoutines.h"

using namespace Messages;

Exec_Add::Exec_Add() {}

void Exec_Add::Help() const {
  Msg("\t{project [<description>] | system dir <dir> [to <project#>] [<description>]\n");
}

/** Get description from remaining args. */
static inline std::string getDesc(Cols& args) {
  std::string description;
  std::string desc = args.NextColumn();
  while (!desc.empty()) {
    description.append(" " + desc);
    desc = args.NextColumn();
  }
  return description;
} 

/** Execute add command */
Exec::RetType Exec_Add::Execute(Manager& manager, Cols& args) const {
  if (args.HasKey("project")) {
    // ----- Add project ---------------
    // Description is all remaining arguments.
    std::string description = getDesc(args);
    if (description.empty()) {
      description.assign("DEFAULT PROJECT DESCRIPTION");
    }
    Msg("\tAdding project %zu : '%s'\n", manager.Projects().size(), description.c_str());
    manager.AddNewProject( description );
  } else if (args.HasKey("system")) {
    // ----- Add System ----------------
    if (manager.Projects().empty()) {
      Msg("Warning: No projects. Create one with 'add project [<description>]'.\n");
      return OK;
    }
    // Get target project index
    int pidx = -1;
    if (args.GetKeyInteger(pidx, "to", pidx)) return ERR;
    if (pidx < 0) {
      // Active project.
      pidx = manager.ActiveProjectIdx();
    } else {
      if (pidx >= (int)manager.Projects().size()) {
        ErrorMsg("Error: Project index %i is out of range.\n", pidx);
        return ERR;
      }
    }
    Project& selectedProject = manager.Set_Project(pidx);
    Msg("\tAdding system to project %i : '%s'\n", pidx, selectedProject.name());
    // Get directory
    std::string fullSystemPath = args.GetKey("dir");
    if (fullSystemPath.empty()) {
      ErrorMsg("No system directory specified.\n");
      return ERR;
    }
    fullSystemPath = FileRoutines::AbsPath( FileRoutines::tildeExpansion(fullSystemPath) );
    std::string systemPrefix;
    std::string systemDir = FileRoutines::Basename( systemPrefix, fullSystemPath );
    Msg("\tSystem directory: %s/%s\n", systemPrefix.c_str(), systemDir.c_str());
    // Description is all remaining arguments.
    std::string description = getDesc(args);
    if (description.empty()) {
      description.assign("DEFAULT SYSTEM DESCRIPTION");
    }
    Msg("\tAdding system %zu : '%s'\n", selectedProject.Systems().size(), description.c_str());
    selectedProject.AddNewSystem( systemPrefix, systemDir, description );
    selectedProject.LastSystem().SetDebug( manager.Debug() );
    //if (projects_.back().LastSystem().FindRuns(queues_)) return ERR;
  }
  return OK;
}
