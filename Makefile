build_type=Debug
#build_type=Release
all:
	cd build; make
clean:
	rm -rf build
build_dir: clean
	mkdir build
dependencies: build_dir
	cd build;\
        conan install .. --build missing;
generate:
	cd build;\
        cmake -G "Unix Makefiles" .. -DCMAKE_BUILD_TYPE=$(build_type)
all:
	cd build; make
app: 
	build/bin/main
test:
	cd build; ctest
app-perf: 
	cd build;\
        cmake -G "Unix Makefiles" .. -DCMAKE_BUILD_TYPE=Debug -DPERFORMANCE=ON;\
        make
	CPUPROFILE=/tmp/prof.out build/bin/main	
	pprof --gv build/bin/main /tmp/prof.out	
