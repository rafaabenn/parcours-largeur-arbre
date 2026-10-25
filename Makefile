# Compilation des programmes de parcours en largeur
#   Windows (MinGW) : mingw32-make        | mingw32-make run | mingw32-make clean
#   Linux           : make                | make run         | make clean
CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

PROGS = LargeurQueue LargeurFileMaison LargeurPerf

all: $(addprefix build/,$(PROGS))

build/%: src/%.cpp src/Arbre.h src/File.h | build
	$(CXX) $(CXXFLAGS) $< -o $@

build:
	mkdir -p build

run: all
	@for p in $(PROGS); do echo "===== $$p"; ./build/$$p; done

clean:
	rm -f $(addprefix build/,$(PROGS)) $(addsuffix .exe,$(addprefix build/,$(PROGS)))

.PHONY: all run clean
