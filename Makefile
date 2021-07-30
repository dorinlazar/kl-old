CXXFLAGS += -Isrc -Isrc/kl -Isrc/ff -std=c++20 -g3
LIBRARIES = build/libkl.a
LDFLAGS = $(LIBRARIES)

LIBHEADERS = $(wildcard src/*/*.h)
LIBSOURCES = $(wildcard src/kl/*.cpp)
LIBOBJECTS = $(subst src/,build/,$(LIBSOURCES:.cpp=.o))

TESTSSRCS = $(wildcard src/tests/kl*.cpp)
TESTSEXES = $(patsubst src/tests/%,build/%,$(TESTSSRCS:.cpp=.tst))

KLBSRCS = $(wildcard src/klb/*.cpp)
KLBOBJS = $(subst src/,build/,$(KLBSRCS:.cpp=.o))

KLB = build/klb.exe

.PHONY: all clean test mkdirs

all: mkdirs $(TESTSEXES) $(LIBRARIES) $(KLB) build/modules-check

mkdirs:
	mkdir -p build $(dir $(LIBOBJECTS)) $(dir $(KLBOBJS))

clean:
	$(RM) -r build/

test: mkdirs $(TESTSEXES) $(LIBRARIES)
	for i in $(TESTSEXES) ; do $$i ; done

$(KLB): $(KLBOBJS) $(LIBRARIES) mkdirs
	$(CXX) $(CXXFLAGS) $(KLBOBJS) $(LDFLAGS) -o $@ 

build/libkl.a: mkdirs $(LIBOBJECTS) $(LIBHEADERS)
	$(AR) cru $@ $(LIBOBJECTS)

build/%.o: src/%.cpp $(LIBHEADERS)
	$(CXX) -c $(CXXFLAGS) $< -o $@ 

build/%.tst: src/tests/%.cpp $(LIBRARIES)
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -o $@ 

build/modules-check: src/tests/modules-check.cpp build/klb/modules.o build/klb/klbsettings.o build/klb/klbfs.o $(LIBRARIES)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@
