# KL Tools

Welcome. I use this repository to build from the grounds up God-knows-what. For the time being, a build tool, because
why not? The name comes from KeyLocked (the name of my company) - but to be honest it comes from the fact that I like
the letter K a lot, with an L added to mess up expectations.

## The Build Tool (KLB)

klb is a minimalistic C++ build tool that requires only one source of truth for what you build: the source code itself.
Ideally klb should be a zeroconf solution, but this is impractical (see below). This build system also has an audience
of one (that'd be me, thank you, you're welcome, so nice that someone thinks about me!); features will definitely be
added as long as my audience really wants them. Here's how it works currently:

1. All source code goes into `src/`. Build output goes into `build/`.
2. All local inclusions are made relative to `src/` or the current folder
3. The implementation of what a header file presents to the public is always in the similarly named `.cpp` file. For
   example things declared in `modules.h` will be implemented in `modules.cpp` in the same folder. I use `.cpp`, but
   `.cc` might work, as well as `.c++`. Because I was lazy and considered a source code anything that has the extension
   starting with `c`.
4. There is a difference between `#include <something>` and `#include "something"`. I honor that difference. The system
   includes will be used sometime in the future to determine libraries and include parameters. Maybe use `pkgconfig`
   too.
5. `klb` will automatically pull the dependencies based on the files you include, and their implementation dependencies
   as well. This way for an executable we link only the objects really needed by an application.
6. To build your code you just need to run `klb` in the root folder (not the src folder).
7. Object files are output in the `build/*source/file/subdirectory*/somefile.o`.
8. Executables are output in the `build/*source/file/subdirectory*/somefile.exe` - one can rename at their discression.
   Executable files are built when the C++ file contains an `int main(` line. Nothing but whitespace is accepted before
   `int`.
9. `klb` now supports target designation, and running of tests. Use `-r` to be in *run mode*, which will execute all
   currently active executable targets. `-r` is dangerous, but useful - run in conjunction with tests it will really
   make the difference.

### Current status for **klb**

- The file .depot.conf (name *stolen* from [Rachel By The Bay](https://rachelbythebay.com/bb/)) will be used to
  parametrize the build. The configuration format will be defined in time.
- **klb** now honors some environment variables, documentation is on the roadmap, but nothing urgent.
- Code now compiles with multiple parallel jobs. Use JOBS=\<number> in configuration file or when running **klb** - by
  default it will be 2.
- Single targets can pe specified in the command line. You can specify the module name (*tests/kltext*) or the target
  path (*build/tests/kltext.exe*) or the source path (*src/tests/kltext.cpp*). `klb` basically allows you to run a C++
  file as long as it's under a folder named `src`.

## The KL Library

I'm pretty dissatisfied with some of the offerings of the `std` namespace. `Text` is the way I try to fix part of the
things that make me unhappy about the std::string. `Text` is meant to be a read-only string (why would one try to modify
in place some string?), and I want it to combine the lightness of a `string_view` with the safety of `shared_ptr`.

The `klds` module contains some useful simplifications of std::vector (as List), std::unordered_map (as Dict), Pair,
Set, and who knows what else. Things can be done smarter, more efficiently. What I'm interested in is a flexible and
expressive API more than performance.

The `kltime` offers a simplified `DateTime` that will save me from the insanity of working directly with `std::chrono`.

Other tools will appear as needed for what I'm implementing.

## File formats

Support for several file formats will be implemented in time.

- `codescanner` is a basic parser for source code that doesn't have all the algorithmic complexity of a normal C
  preprocessor, but manages to generally find all the dependencies of a file (albeit sometimes too many, it doesn't know
  yet about `#if`s).
- `poorconfig` is a minimalistic and strict subset of YAML that will be the basis of configurability in KL related
  tools.
- `markwrite` is a somewhat different Markdown, that is work in progress and the main tool for publishing in the klblog
  engine.
- `textscanner` is a set of tools for what it seems that is called a *shift-reduce parser*. I might be very much in the
  wrong, but this is my naïve method of writing my own text scanner. What is a bit different is that you can also save
  locations in the middle of the parser, and restore them at will.
