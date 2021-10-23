#include <iostream>
#include <iomanip>
#include <cmath>
#include <mpi.h>
#include <fstream>
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

    double startTime = MPI_Wtime();

    int iters = 50;
    double error = 0;

    for (int iter = 0; iter < iters; iter++) {
        double *localPoints = new double[arguments.points * 3];
        double localSum = 0;
        int needAnother = 1;

        if (arguments.rank == master) {
            int totalPoints = arguments.points * arguments.size;
            double *points = new double[totalPoints * 3];
            double integrate = 0;
            double totalSum = 0;

            int loops = 0;

            while (needAnother) {
                for (int i = arguments.points; i < totalPoints * 3; i += 3) {
                    points[i] = Generate(arguments.xmin, arguments.xmax);
                    points[i + 1] = Generate(arguments.ymin, arguments.ymax);
                    points[i + 2] = Generate(arguments.zmin, arguments.zmax);
                }

                double sum = 0;

                MPI_Scatter(points, arguments.points * 3, MPI_DOUBLE, localPoints, arguments.points * 3, MPI_DOUBLE, master, MPI_COMM_WORLD);
                MPI_Reduce(&localSum, &sum, 1, MPI_DOUBLE, MPI_SUM, master, MPI_COMM_WORLD);

                loops++;
                totalSum += sum / (arguments.size - 1);
                integrate = totalSum * arguments.volume / loops;

                needAnother = fabs(integrate - accurate) > arguments.eps;
                MPI_Bcast(&needAnother, 1, MPI_INT, master, MPI_COMM_WORLD);
            }

            delete[] points;

            error += fabs(integrate - accurate);
        }
        else {
            while (needAnother) {
                MPI_Scatter(NULL, arguments.points * 3, MPI_DOUBLE, localPoints, arguments.points * 3, MPI_DOUBLE, master, MPI_COMM_WORLD);

                localSum = CalculateSum(arguments.points, localPoints);

                MPI_Reduce(&localSum, NULL, 1, MPI_DOUBLE, MPI_SUM, master, MPI_COMM_WORLD);
                MPI_Bcast(&needAnother, 1, MPI_INT, master, MPI_COMM_WORLD);
            }
        }

        delete[] localPoints;
    }

    double endTime = MPI_Wtime();
    double delta = endTime - startTime;
    double maxTime, minTime, avgTime;

    MPI_Reduce(&delta, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&delta, &minTime, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&delta, &avgTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (arguments.rank == master) {
        ofstream fout("master.txt", ios::app);
        fout << "| " << setw(12) << arguments.eps << " | " << setw(19) << arguments.size << " | " << setw(26) << (avgTime / arguments.size / iters) << " |           | " << setw(11) << (error / iters) << " |" << endl;
        fout.close();
    }

    MPI_Finalize();
    return 0;
}
