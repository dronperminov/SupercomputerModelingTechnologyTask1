COMPILER=mpixlC
FLAGS=-O3 -std=c++11
EPS=1e-6
POINTS=1000
PROCESSORS=16
MINUTES=10
VOLUME_ARGS=-xmin 0 -xmax 1 -ymin 0 -ymax 1 -zmin 0 -zmax 1

all: normal master

normal: main_normal.cpp
	$(COMPILER) $(FLAGS) main_normal.cpp -o normal

master: main_master.cpp
	$(COMPILER) $(FLAGS) main_master.cpp -o master

test-normal: test_normal.cpp
	$(COMPILER) $(FLAGS) test_normal.cpp -o test_normal

test-master: test_master.cpp
	$(COMPILER) $(FLAGS) test_master.cpp -o test_master

submit-polus: submit-polus-normal submit-polus-master

submit-polus-normal:
	mpixlC -O3 -std=c++11 test_normal.cpp -o normal

	for eps in 3e-5 5e-6 1.5e-6 ; do \
		for p in 1 4 16 64 ; do \
			mpisubmit.pl -p $$p -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./normal -- -e $$eps -n $(POINTS) -o normal_$(POINTS).txt $(VOLUME_ARGS) ; \
		done \
	done

submit-polus-master:
	mpixlC -O3 -std=c++11 test_master.cpp -o master

	for eps in 3e-5 5e-6 1.5e-6 ; do \
		for p in 2 4 16 64 ; do \
			mpisubmit.pl -p $$p -w 00:$(MINUTES) --stdout /dev/null --stderr /dev/null ./master -- -e $$eps -n $(POINTS) -o master_$(POINTS).txt $(VOLUME_ARGS) ; \
		done \
	done


submit-bluegene: submit-bluegene-normal submit-bluegene-master

submit-bluegene-normal:
	mpixlcxx -O3 test_normal.cpp -o normal

	for eps in 1e-4 2e-5 8e-6 ; do \
		for p in 1 4 16 64 ; do \
			mpisubmit.bg -n $$p -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./normal -- -e $$eps -n $(POINTS) -o normal_$(POINTS).txt $(VOLUME_ARGS) ; \
		done \
	done

submit-bluegene-master:
	mpixlcxx -O3 test_master.cpp -o master

	for eps in 1e-4 2e-5 8e-6 ; do \
		for p in 2 4 16 64 ; do \
			mpisubmit.bg -n $$p -w 00:$(MINUTES):00 --stdout /dev/null --stderr /dev/null ./master -- -e $$eps -n $(POINTS) -o master_$(POINTS).txt $(VOLUME_ARGS) ; \
		done \
	done
