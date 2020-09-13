build_type=Debug
#build_type=Release

all: | _build
	cd _build; make

clean:
	rm -rf _build

_build:
	mkdir _build;\
	cd _build;\
        conan install .. --build missing;\
        cmake -G "Unix Makefiles" .. -DCMAKE_BUILD_TYPE=$(build_type)

run: 
	_build/bin/main
tests:
	cd _build; ctest
run-perf: 
	cd _build;\
        cmake -G "Unix Makefiles" .. -DCMAKE_BUILD_TYPE=Debug -DPERFORMANCE=ON;\
        make
	CPUPROFILE=/tmp/prof.out _build/bin/main	
	pprof --gv _build/bin/main /tmp/prof.out	
	
