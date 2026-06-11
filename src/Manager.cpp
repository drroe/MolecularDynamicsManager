#include "Manager.h"
#include "Messages.h"
#include "TextFile.h"
#include "FileRoutines.h"
#include "StringRoutines.h"
#include "Cols.h"

using namespace MdManager;
using namespace Messages;

/** CONSTRUCTOR */
Manager::Manager() :
  debug_(0),
  activeProjectIdx_(0)
{}

/** Set debug levels. */
void Manager::SetDebug(int debugIn) {
  debug_ = debugIn;
}

/** Save projects/systems to systems file. */
int Manager::SaveManager() {
  if (Fname_.empty()) {
    ErrorMsg("Manager::SaveManager called with no file name set.\n");
    return 1;
  }
  Msg("DEBUG: Projects file is '%s'\n", Fname_.c_str());
  bool write_projects_file = true;
  if (FileRoutines::fileExists( Fname_ )) {
    if (!YesNoPrompt("Overwrite?")) {
      write_projects_file = false;
    }
  }
  if (write_projects_file) {
    TextFile outfile;
    if (outfile.OpenWrite( Fname_ )) {
      ErrorMsg("Could not open '%s' for write.\n", Fname_.c_str());
      return 1;
    }
    for (LineList::const_iterator it = PfileLines_.begin(); it != PfileLines_.end(); ++it)
    {
      int pidx = it->ProjIdx();
      if (!it->Comment().empty())
        outfile.Printf("%s\n", it->Comment().c_str());
      if (pidx > -1) {
        Msg("DEBUG: Project %i (needs write=%i)\n", pidx, (int)projects_[pidx].NeedsWrite());
        if (projects_[pidx].SaveSystems( outfile )) {
          ErrorMsg("Write of project %i failed.", pidx);
          return 1;
        }
      }
    }
  }
  return 0;
}

/** Add new Project (created). */
void Manager::AddNewProject(std::string const& desc) {
  PfileLines_.push_back( ProjectFileLine(projects_.size()) );
  projects_.push_back( Project(desc, true ) );
}

/** Initialize with input file. */
int Manager::InitManager(std::string const& CurrentDir, std::string const& inputFileName) {
  if (inputFileName.empty()) {
    ErrorMsg("No manager input file given.\n");
    return 1;
  }
  topDir_ = CurrentDir;

  Fname_ = FileRoutines::AbsPath( CurrentDir + "/" + inputFileName );
  // Check if systems file exists. If not, offer to create it.
  if (!FileRoutines::fileExists( Fname_ )) {
    Msg("Systems file '%s' does not exist.\n", Fname_.c_str());
    if (YesNoPrompt("Create it?")) {
      TextFile output;
      if (output.OpenWrite( Fname_ )) {
        ErrorMsg("Could not open '%s' for writing.\n", Fname_.c_str());
        return 1;
      }
      std::string tstring = StringRoutines::TimeString();
      output.Printf("#MdManager systems file. %s\n", tstring.c_str());
      output.Close();
      return 0;
    } else {
      ErrorMsg("No systems file.\n");
      return 1;
    }
  }

  // Read an existing systems file
  PfileLines_.clear();
  TextFile input;
  if (input.OpenRead(inputFileName)) {
    ErrorMsg("Could not open manager input file '%s'\n", inputFileName.c_str());
    return 1;
  }
  static const char* SEP = " \n\r";
  const char* ptr = input.Gets();
  while (ptr != 0) {
    Cols colsIn;
    if (colsIn.Split( std::string(ptr), SEP )) {
      ErrorMsg("Could not split line: %s\n", ptr);
      return 1;
    }
    unsigned int ncols = colsIn.Ncolumns();
    if (ncols > 0) {
      if (colsIn[0][0] != '#') {
        if ( colsIn[0] == "project" ) {
          // Expect project <name>
          if (ncols < 2) {
            std::string errline;
            for (unsigned int col = 1; col < ncols; col++)
              errline.append(" " + colsIn[col]);
            ErrorMsg("Not enough columns for 'project': %s\n", errline.c_str());
            return 1;
          }
          // All columns beyond 0 are project name
          std::string description = colsIn[1];
          for (unsigned int col = 2; col < ncols; col++)
            description.append(" " + colsIn[col]);
          Msg("Project: %s\n", description.c_str());
          PfileLines_.push_back( ProjectFileLine( projects_.size() ) );
          projects_.push_back( Project(description) );
        } if ( colsIn[0] == "system" ) {
          // Expect system <system dir>, <description>
          if (ncols < 3) {
            std::string errline;
            for (unsigned int col = 1; col < ncols; col++)
              errline.append(" " + colsIn[col]);
            ErrorMsg("Not enough columns for 'system': %s\n", errline.c_str());
            return 1;
          }
          // If no Project yet, add default
          if (projects_.empty()) {
            PfileLines_.push_back( ProjectFileLine( projects_.size() ) );
            projects_.push_back( Project() );
          }
          // All columns beyond the first are description
          std::string description = colsIn[2];
          for (unsigned int col = 3; col < ncols; col++)
            description.append(" " + colsIn[col]);
          std::string system_dir = FileRoutines::tildeExpansion(colsIn[1]);
          Msg("\nSystem: %s  Description: '%s'\n", system_dir.c_str(), description.c_str());
          projects_.back().AddSystem( System(CurrentDir, system_dir, description) );
          projects_.back().LastSystem().SetDebug( debug_ );
          if (projects_.back().LastSystem().FindRuns()) return 1;
        }
      } else {
        // Save comment
        Msg("DEBUG: Saving comment: %s\n", ptr);
        PfileLines_.push_back( ProjectFileLine( StringRoutines::NoTrailingWhitespace(std::string(ptr)) ) );
      }
    }
    ptr = input.Gets();
  }
  input.Close();
  // Change to the active systems directory
  if (ChangeToActiveSystemDir()) return 1;

  return 0;
}

/** Change to active systems directory. */
int Manager::ChangeToActiveSystemDir() const {
  if (!projects_.empty()) {
    Project const& activeProject = projects_[activeProjectIdx_];
    if (activeProject.ActiveSystemIdx() > -1) {
      System const& activeSystem = activeProject.Systems()[activeProject.ActiveSystemIdx()];
      Msg("  Active system dir: '%s'\n", activeSystem.FullSystemPath().c_str());
      if (activeSystem.ChangeToSystemDir()) {
        ErrorMsg("Change to active system directory failed.\n");
        return 1;
      }
    }
  }
  return 0;
}

/** Set active project and system. */
int Manager::SetActiveProjectSystem(int tgtProjectIdx, int tgtSystemIdx) {
  if (tgtProjectIdx < 0) {
    ErrorMsg("Must specify a valid project index.\n");
    return 1;
  }

  if ((unsigned int)tgtProjectIdx >= projects_.size()) {
    ErrorMsg("Project index %i is out of range.\n", tgtProjectIdx);
    return 1;
  }
  activeProjectIdx_ = tgtProjectIdx;
  Project& activeProject = ActiveProject();

  // If no system index given default to zero.
  if (tgtSystemIdx < 0) {
    Msg("No system specified; defaulting to 0.\n");
    tgtSystemIdx = 0;
  }

  if ((unsigned int)tgtSystemIdx >= activeProject.Systems().size()) {
    ErrorMsg("System index %i is out of range.\n", tgtSystemIdx);
    return 1;
  }
  activeProject.SetActiveSystem( tgtSystemIdx );

  return 0;
}

/** \return True if there is an active system to return. */
bool Manager::HasActiveProjectSystem() const {
  if (!projects_.empty()) {
    Project const& activeProject = projects_[activeProjectIdx_];
    if (activeProject.ActiveSystemIdx() > -1)
      return true;
  }
  return false;
}

/** \return True if any systems need to be saved. */
bool Manager::SystemsNeedSave() const {
  int n_needs_save = 0;
  for (ProjectArray::const_iterator project = projects_.begin();
                                    project != projects_.end(); ++project)
  {
    if (project->NeedsWrite()) {
      Msg("Needs save: Project %li : %s\n", project - projects_.begin(), project->name());
      n_needs_save++;
    }
    for (Project::SystemArray::const_iterator system = project->Systems().begin();
                                              system != project->Systems().end(); ++system)
    {
      if (system->NeedsSave()) {
        Msg("Needs save: ");
        system->PrintSummary();
        n_needs_save++;
      }
    }
  }
  return (n_needs_save > 0);
}
