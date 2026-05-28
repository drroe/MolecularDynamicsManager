include ../config.h

include srcfiles

OBJECTS=$(SOURCES:.cpp=.o)

install: MdManager ../bin
	/bin/mv MdManager ../bin/

../bin:
	mkdir ../bin

uninstall: clean
	/bin/rm -f ../bin/MdManager

all: MdManager

MdManager: $(OBJECTS) ../external/readline/libreadline.a
	$(CXX) -o MdManager $(OBJECTS) $(LDFLAGS) ../external/readline/libreadline.a

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(DIRECTIVES) -o $@ $<

../external/readline/libreadline.a:
	cd ../external && $(MAKE) libreadline

clean:
	/bin/rm -f $(OBJECTS) FindDepend.o MdManager

debug: clean
	$(MAKE) install CXXFLAGS='-Wall -g'

findDepend: FindDepend.o
	$(CXX) -o findDepend FindDepend.o

depend: findDepend
	./findDepend $(SOURCES) > depends

# Dependencies
include depends
