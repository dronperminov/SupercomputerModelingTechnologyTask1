#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <mpi.h>
#include "ArgumentParser.hpp"

using namespace std;

// интегрируемая функция
double f(double x, double y, double z) {
    if (x >= 0 && x <= 1 && y >= 0 && y <= x && z >= 0 && z <= x * y)
        return x*y*y*z*z*z;

    return 0;
}

// генерация случайного вещественного числа
double Generate(double a, double b) {
    return a + rand() * (b - a) / RAND_MAX;
}

// вычисление средней суммы
double CalculateSum(int n, double *points) {
    double sum = 0;

    for (int i = 0; i < n * 3; i += 3)
        sum += f(points[i], points[i + 1], points[i + 2]);

    return sum / n;
}

int main(int argc, char** argv) {
    ArgumentParser parser;

    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) {
        parser.Help();
        return 0;
    }

    int master = 0; // номер мастер-процесса
    double accurate = 1.0 / 364; // точное значение интеграла

    Arguments arguments;

    try {
        arguments = parser.Parse(argc, argv);
    }
    catch (const char *error) {
        cout << error << endl;
        return -1;
    }

    ofstream fout(arguments.output ? arguments.output : "output.txt");
    fout.close();

    srand(arguments.rank);

    double startTime = MPI_Wtime();

    int needAnother = 1;
    int loops = 0;

    double *points = new double[arguments.points * 3];
    double integrate = 0;
    double totalSum = 0;

    while (needAnother) {
        for (int i = 0; i < arguments.points * 3; i += 3) {
            points[i] = Generate(arguments.xmin, arguments.xmax);
            points[i + 1] = Generate(arguments.ymin, arguments.ymax);
            points[i + 2] = Generate(arguments.zmin, arguments.zmax);
        }

        double localSum = CalculateSum(arguments.points, points);
        double sum = 0;

        MPI_Reduce(&localSum, &sum, 1, MPI_DOUBLE, MPI_SUM, master, MPI_COMM_WORLD);

        if (arguments.rank == master) {
            loops++;
            totalSum += sum / arguments.size;
            integrate = totalSum * arguments.volume / loops;

            if (arguments.debug) {
                ofstream fout(arguments.output ? arguments.output : "output.txt", ios::app);                
                fout << "Loop " << loops << ": " << integrate << " (accurate: " << accurate << ", defference: " << fabs(accurate - integrate) << ")" << endl;
                fout.close();
            }

            needAnother = fabs(integrate - accurate) > arguments.eps;
        }

        MPI_Bcast(&needAnother, 1, MPI_INT, master, MPI_COMM_WORLD);
    }

    delete[] points;

    double endTime = MPI_Wtime();
    double delta = endTime - startTime;
    double maxTime, minTime, avgTime;

    MPI_Reduce(&delta, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&delta, &minTime, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&delta, &avgTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (arguments.rank == master) {
        ofstream fout(arguments.output ? arguments.output : "output.txt", ios::app);
        fout << "Paradigm: usual" << endl;
        fout << "Processors: " << arguments.size << endl;
        fout << "Eps: " << arguments.eps << endl;
        fout << "Points per process: " << arguments.points << endl;
        fout << "Dimensions: [" << arguments.xmin << ", " << arguments.xmax << "] x [" << arguments.ymin << ", " << arguments.ymax << "] x [" << arguments.zmin << ", " << arguments.zmax << "]" << endl;
        fout << "Volume: " << arguments.volume << endl;
        fout << "Loops: " << loops << endl;
        fout << "Total number of points: " << (arguments.points * arguments.size * loops) << endl;
        fout << "Target integrate value: " << accurate << endl;
        fout << "Calculated integrate value: " << integrate << endl;
        fout << "Error: " << fabs(integrate - accurate) << endl;

        fout << "Max time: " << maxTime << endl;
        fout << "Min time: " << minTime << endl;
        fout << "Average time: " << avgTime / arguments.size << endl;

        fout.close();
    }

    MPI_Finalize();
    return 0;
}
