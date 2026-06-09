#include "Exec_Add.h"
#include "Messages.h"
#include "Manager.h"
#include "Cols.h"

using namespace Messages;

Exec_Add::Exec_Add() {}

void Exec_Add::Help() const {
  Msg("\t{project [<description>] | system <dir> [to <project#>] [<description>]\n");
}

Exec::RetType Exec_Add::Execute(Manager& manager, Cols& args) const {
  if (args.HasKey("project")) {
    // Description is all remaining arguments.
    std::string description;
    std::string desc = args.NextColumn();
    while (!desc.empty()) {
      description.append(" " + desc);
      desc = args.NextColumn();
    }
    if (desc.empty()) {
      description.assign("DEFAULT PROJECT DESCRIPTION");
    }
    Msg("\tAdding project %zu : '%s'\n", manager.Projects().size(), description.c_str());
    manager.AddProject( Project(description) );
  } else if (args.HasKey("system")) {
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
  }
  return OK;
}
