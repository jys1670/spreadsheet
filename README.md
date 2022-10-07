# Spreadsheet
![C++ version](https://img.shields.io/badge/C%2B%2B-17-blue)

Spreadsheet capable of processing text and formula cells - an attempt to replicate core features of Excel editor.

### Building:

Preparations:

[ANTLR C++ runtime](https://www.antlr.org/) is required for compilation of parser related files.

```sh
git clone https://github.com/jys1670/spreadsheet.git
cd spreadsheet
mkdir build && cd build
```

Building and running main executable:
```sh
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target spreadsheet
./spreadsheet
```

Building and running unit tests:
```sh
cmake --build . --config Release --target unit-tests
./unit-tests
```

Updating documentation:
```sh
cmake --build . --config Release --target doxygen
```

Regenerating ANTLR4 files (in case of breaking interface changes):
```sh
cmake --build . --config Release --target antlr4-generate-files
```