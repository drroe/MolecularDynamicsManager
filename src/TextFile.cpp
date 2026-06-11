#include <cstdio>
#include <cstdarg>
#include <cstring>
#include "TextFile.h"
#include "Messages.h"
#include "OptArray.h"
#include "Cols.h"

using namespace Messages;

TextFile::~TextFile() { Close(); }

/** Open file for reading. */
int TextFile::OpenRead(std::string const& fname) {
  FILE* infile = fopen(fname.c_str(), "rb");
  if (infile == 0) {
    perror("Error is:");
    ErrorMsg("Opening file '%s'\n", fname.c_str());
    return 1;
  }
  file_ = (void*)infile;
  return 0;
}

/** Open file for reading from a pipe. */
int TextFile::OpenPipe(std::string const& cmd) {
  FILE* pipe = popen(cmd.c_str(), "r");
  if (pipe == 0) {
    perror("Error is:");
    ErrorMsg("Opening pipe: '%s'\n", cmd.c_str());
    return 1;
  }
  file_ = (void*)pipe;
  isPipe_ = true;
  return 0;
}

/** Open file for writing. */
int TextFile::OpenWrite(std::string const& fname) {
  FILE* outfile = fopen(fname.c_str(), "wb");
  if (outfile == 0) {
    perror("Error is:");
    ErrorMsg("Opening file '%s'\n", fname.c_str());
    return 1;
  }
  file_ = (void*)outfile;
  return 0;
}

/** Close file. */
void TextFile::Close() {
  if (file_ != 0) {
    if (isPipe_) {
      pclose((FILE*)file_);
      isPipe_ = false;
    } else
      fclose((FILE*)file_);
  }
  file_ = 0;
}

/** \return Pointer to next line in the raw buffer. */
const char* TextFile::Gets() {
  if (file_ == 0) return 0; // TODO perror?
  return (const char*)fgets(buffer_, BUF_SIZE-1, (FILE*)file_);
}

/** \return Next line as a string, no terminal whitespace. */
std::string TextFile::GetString() {
  if (file_ == 0) return std::string("");
  char* ptr = fgets(buffer_, BUF_SIZE-1, (FILE*)file_);
  if (ptr == 0) return std::string("");
  // Remove any newline.
  for (unsigned int i = 0; i != BUF_SIZE; i++) {
    if (buffer_[i] == '\0') break;
    if (buffer_[i] == '\n') {
      buffer_[i] = '\0';
      break;
    }
  }
  return std::string(buffer_);
}

/** Print formatted text to file */
int TextFile::Printf(const char *format, ...) {
  if (file_==0) return 1;
  va_list args;
  va_start(args, format);
  vsprintf(buffer_,format,args);
  fwrite(buffer_, 1, strlen(buffer_), (FILE*)file_);
  va_end(args);
  return 0;
}

/** Read in an options file with format 'KEY VAR ...'*/
OptArray TextFile::GetOptionsArray(std::string const& fname, int debug) {
  OptArray options;
  if (OpenRead( fname )) return options;
  const char* SEP = " \t\n";

  const char* ptr = Gets();
  while (ptr != 0) {
    Cols colsIn;
    if (colsIn.Split( std::string(ptr), SEP )) {
      ErrorMsg("Could not split line %s\n", ptr);
      options.clear();
      break;
    }

    unsigned int ncols = colsIn.Ncolumns();
    if (ncols > 0 && colsIn[0][0] != '#') {
      if (ncols < 2) {
        ErrorMsg("Malformed input: %s\n", ptr);
        options.clear();
        break;
      }
      std::string OPT = colsIn[0];
      std::string VAR = colsIn[1];
      for (unsigned int i = 2; i < ncols; i++)
        VAR += (" " + colsIn[i]);
      if (debug > 0)
        Msg("    File '%s': Option: %s  Variable: %s\n", fname.c_str(), OPT.c_str(), VAR.c_str());
      options.AddOpt( OptArray::OptPair(OPT, VAR) );
    }
    ptr = Gets();
  }
  Close();
  return options;
}
