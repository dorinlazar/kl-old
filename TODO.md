# Stuff to do

- add clang-tidy for this project.

## KLB

- support for compilation profiles. Suggested profiles: debug and release. Debug should have support for coverage and other tools, while release should have support for things like LTO.
- support for library folders: all source files in a certain folders will be compiled into one library. Then there will be the possibility to have a special link step - especially useful for google-test-like builds.
- cleaner makefile generation: use $
