# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

# Directories
SRCDIR = src
INCDIR = include
BINDIR = bin
OBJDIR = obj

# Target executable name
TARGET = ftm_txt2mid.exe

# Find all .cpp files and map them to .o files in obj/
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))

# Default target
all: $(BINDIR)/$(TARGET)

# Link the executable
$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

# Compile source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create directories if they don’t exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(BINDIR)/$(TARGET)

.PHONY: all clean
