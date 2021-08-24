# Roadmap

## File formats

- **very** basic support for the following file formats is planned:
  - `(short term)` PoorConfig - a restricted subset of YAML, specs to be defined.
  - `(long term)` Markwrite - a restricted subset of Markdown with support for extensibility.

## KL Blog Engine

Unbenownkst to all my follower (Hi, mum!, just kidding, mum doesn't follow me here), this is meant to be a content
management system. This is the next thing I'll start implementing, starting from defining a derivation of markdown that
allows easy extensibility with external modules and a template engine.

## KLB

**KLB** works for the time being.

- `(medium term)` Support for external libraries, based on either **pkgconfig** or directly configuring compilation flags.
  I strongly prefer the pkgconfig version, but let's see.
- `(medium term)` Clear and complete documentation of configuration (**environment variables** and **.depot.conf**).
- `(medium term)` Specialized folders. Perhaps special handling for src/include (but how?), special handling for
  `src/tests` folder (on `klb test` we could build and run all the executables in the src/tests folder and subfolders),
  `src/lib` for libraries - things that would get compiled to lib\<folder>.a
- `(long term)` Support for modules (and a complete switch of the code of the whole KeyLocked Library). I'm thinking of
  supporting special folder `src/modules/` for modules, allowing compilation of hybrid code bases.
