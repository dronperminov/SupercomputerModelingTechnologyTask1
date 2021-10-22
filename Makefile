COMPILER=mpixlC
FLAGS=-O3 -pedantic -Wall -std=c++11
EPS=1e-6
POINTS=1000
PROCESSORS=16
MINUTES=05

all: main

# main: main_normal.cpp
# 	$(COMPILER) $(FLAGS) main_normal.cpp -o main

main: main_master.cpp
	$(COMPILER) $(FLAGS) main_master.cpp -o main

submit-polus: main
	rm -rf output_$(PROCESSORS).txt error_$(PROCESSORS).txt
	mpisubmit.pl -p $(PROCESSORS) -w 00:$(MINUTES) --stdout output_$(PROCESSORS).txt --stderr error_$(PROCESSORS).txt ./main -- -e $(EPS) -n $(POINTS) -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1

submit-bluegene: main
	rm -rf output_$(PROCESSORS).txt error_$(PROCESSORS).txt
	mpisubmit.bg -n $(PROCESSORS) -w 00:$(MINUTES):00 --stdout output_$(PROCESSORS).txt --stderr error_$(PROCESSORS).txt ./main -- -e $(EPS) -n $(POINTS) -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1

submit-polus-all: main
	mpisubmit.pl -p 2 -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./main -- -e 3e-5 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.pl -p 4 -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./main -- -e 3e-5 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.pl -p 16 -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./main -- -e 3e-5 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.pl -p 64 -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./main -- -e 3e-5 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1

	mpisubmit.pl -p 2 -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./main -- -e 5e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.pl -p 4 -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./main -- -e 5e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.pl -p 16 -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./main -- -e 5e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.pl -p 64 -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./main -- -e 5e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1

	mpisubmit.pl -p 2 -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./main -- -e 1.5e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.pl -p 4 -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./main -- -e 1.5e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.pl -p 16 -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./main -- -e 1.5e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.pl -p 64 -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./main -- -e 1.5e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1

submit-bluegene-all: main
	mpisubmit.bg -n 2 -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./main -- -e 1e-4 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.bg -n 4 -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./main -- -e 1e-4 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.bg -n 16 -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./main -- -e 1e-4 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.bg -n 64 -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./main -- -e 1e-4 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1

	mpisubmit.bg -n 2 -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./main -- -e 2e-5 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.bg -n 4 -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./main -- -e 2e-5 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.bg -n 16 -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./main -- -e 2e-5 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.bg -n 64 -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./main -- -e 2e-5 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1

	mpisubmit.bg -n 2 -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./main -- -e 8e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.bg -n 4 -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./main -- -e 8e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.bg -n 16 -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./main -- -e 8e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
	mpisubmit.bg -n 64 -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./main -- -e 8e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
