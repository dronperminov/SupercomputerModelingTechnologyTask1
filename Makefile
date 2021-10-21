COMPILER=mpicxx
FLAGS=-O3 -pedantic -Wall -std=c++11
PROCESSORS=16

all: main

main: main.cpp
	$(COMPILER) $(FLAGS) main.cpp -o main

submit: main
	rm -rf output.txt error.txt
	mpisubmit.pl -p $(PROCESSORS) -w 00:05 --stdout output.txt --stderr error.txt ./main -- -e 1e-6 -n 1000 -xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1