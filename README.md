# README: Famitracker Text To MIDI — C++ Version

A C++ application that converts a FamiTracker text export file (`.txt`) into a MIDI file.

This tool enables efficient conversion of tracker compositions into standard MIDI format for use in digital audio workstations (DAWs) or notation software.

---

## Features

* Constructs a `Project` structure to contain parsed FamiTracker data.
* `ProjectReader` loads and parses the text file line by line.
* Uses an `std::unordered_map<std::string, std::function<void(...)>>` dispatch table for opcode handling.
* Processes large text exports (~200,000 lines) in under 3 seconds.
* `ProjectFormatter` decompresses pattern tokens into sequential tracks.
* Custom `MidiWriter` class handles MIDI event construction and low-level binary writing.
* `ProjectExporter` creates full MIDI tracks for each song in the module.

---

## Technical Highlights

* Efficient use of STL containers (`std::vector`, `std::unordered_map`, `std::string`, etc.).
* Clean separation between parsing, formatting, and exporting components.
* Regex-based parsing with `std::regex` and stream parsing via `std::stringstream`.
* Implementation of object-oriented patterns, including abstract interfaces and modular class design.
* Binary I/O using standard C++ streams.
* CMake build configuration for cross-platform support.

---

## Build & Run

### Build
```bash
mkdir build && cd build
cmake ..
make
````

### Run

```bash
./famitracker_to_midi <input_famitracker_file.txt> <output_directory (optional)>
```

This will export `.mid` files corresponding to all songs found in the input.

---

## Project Significance

This version highlights:

* Advanced C++ file and string parsing.
* Design pattern implementation (factory, interface, registry).
* Deep understanding of binary encoding through MIDI structure handling.
* Build management with CMake and modular project structure.
* Git repository management and collaborative development discipline.

---

## Next Steps

* Implement more accurate drum channel mapping.
* Explore MIDI to FamiTracker conversion pipeline (converting in the other direction).
* Experiment with template metaprogramming for compile-time dispatch optimization.

# License 
The MIT License (MIT)

Copyright (c) 2025 LoganARichey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

