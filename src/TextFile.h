#ifndef INC_TEXTFILE_H
#define INC_TEXTFILE_H
#include <string>
class OptArray;
/// Simple wrapper for text file.
class TextFile {
  public:
    /// CONSTRUCTOR
    TextFile() : file_(0), isPipe_(false) {}
    ~TextFile();
    int OpenRead(std::string const&);
    int OpenPipe(std::string const&);
    int OpenWrite(std::string const&);
    void Close();
    /// \return next line in internal char buffer 
    const char* Gets();
    /// \return next line as string, no newline.
    std::string GetString();
    /// Print formatted text to file.
    int Printf(const char*, ...);
    /// \return pointer to internal buffer.
    const char* Buffer() const { return buffer_; }
    /// \return Options array from <OPT> <VAR> style file.
    OptArray GetOptionsArray(std::string const&, int);
  private:
    static const unsigned int BUF_SIZE = 8192;
    char buffer_[BUF_SIZE];
    void* file_;
    bool isPipe_;
};
#endif
