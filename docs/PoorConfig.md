# PoorConfig file format

The PoorConfig file format is a simple key-value storage. Format development is in progress.

## Format

Each line consists of a key-value pair that is separated by either an `=` sign or a `:` sign. Until the format settles
there is no preferred choice. Sample format below:

    CXXFLAGS = -std=c++20 -g3
    JOBS = 6

Spaces are allowed around the separator. There is no special handling of scalars - all values are treated as „Text”.
Currently there is no way to pass a sequence as a value.
