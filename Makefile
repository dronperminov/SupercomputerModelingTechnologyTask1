COMPILER=mpicxx
FLAGS=-O3 -pedantic -Wall -std=c++11
EPS=1e-6
POINTS=1000
PROCESSORS=16
MINUTES=05

all: main

main: main.cpp
	$(COMPILER) $(FLAGS) main.cpp -o main

submit-polus: main
	rm -rf output_$(PROCESSORS).txt error_$(PROCESSORS).txt
	mpisubmit.pl -p $(PROCESSORS) -w 00:$(MINUTES) --stdout output_$(PROCESSORS).txt --stderr error_$(PROCESSORS).txt ./main -- -e $(EPS) -n $(POINTS) -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1

submit-bluegene: main
	rm -rf output_$(PROCESSORS).txt error_$(PROCESSORS).txt
	mpisubmit.bg -n $(PROCESSORS) -w 00:$(MINUTES):00 --stdout output_$(PROCESSORS).txt --stderr error_$(PROCESSORS).txt ./main -- -e $(EPS) -n $(POINTS) -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1
