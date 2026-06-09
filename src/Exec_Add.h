#ifndef INC_EXEC_ADD_H
#define INC_EXEC_ADD_H
#include "Exec.h"
class Exec_Add : public Exec {
  public:
    Exec_Add();
    void Help() const;
    RetType Execute(Manager&, Cols&) const;
};
#endif
