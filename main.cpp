#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <mpi.h>

using namespace std;

struct Arguments {
    double eps; // точность
    int points; // количество точек для каждого процесса
    int size; // количество процессов
    int rank; // номер процесса
    bool debug; // отладка

    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;

    double volume; // объём области
};

void Help() {
    cout << "Usage: ./integrate [-e eps] [-n points] [-d] [-xmin xmin] [-xmax xmax] [-ymin ymin] [-ymax ymax] [-zmin zmin] [-zmax zmax]" << endl;
    cout << "Arguments:" << endl;
    cout << "  -e    - required accuracy, default - 1e-3" << endl;
    cout << "  -n    - number of point for process, default - 1000" << endl;
    cout << "  -d    - debug mode, not used by default" << endl;
    cout << "  -xmin - left bound on x axis, default - 0" << endl;
    cout << "  -xmax - right bound on x axis, default - 1" << endl;
    cout << "  -ymin - left bound on y axis, default - 0" << endl;
    cout << "  -ymax - right bound on y axis, default - 1" << endl;
    cout << "  -zmin - left bound on z axis, default - 0" << endl;
    cout << "  -zmax - right bound on z axis, default - 1" << endl;
}

// TODO: normal parsing of arguments
Arguments ParseArguments(int argc, char **argv) {
    Arguments arguments;

    arguments.eps = 1e-3;
    arguments.points = 1000;

    arguments.xmin = 0;
    arguments.xmax = 1;

    arguments.ymin = 0;
    arguments.ymax = 1;

    arguments.zmin = 0;
    arguments.zmax = 1;

    arguments.debug = false;

    for (int i = 1; i < argc; i += 2) {
        if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--eps")) {
            arguments.eps = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--points")) {
            arguments.points = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-xmin")) {
            arguments.xmin = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-xmax")) {
            arguments.xmax = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-ymin")) {
            arguments.ymin = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-ymax")) {
            arguments.ymax = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-zmin")) {
            arguments.zmin = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-zmax")) {
            arguments.zmax = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug")) {
            arguments.debug = true;
            i--;
        }
        else {
            throw "Unknown argument";
        }
    }

    if (arguments.eps < 1e-6 || arguments.eps > 1)
        throw "Invalid eps value";

    if (arguments.points < 1)
        throw "Invalid points value";

    if (MPI_Init(NULL, NULL) != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, -1);
        throw "MPI_Init failed";
    }

    if (MPI_Comm_size(MPI_COMM_WORLD, &arguments.size) != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, -1);
        throw "MPI_Comm_size failed";
    }

    if (MPI_Comm_rank(MPI_COMM_WORLD, &arguments.rank) != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, -1);
        throw "MPI_Comm_rank failed";
    }

    double dx = arguments.xmax - arguments.xmin;
    double dy = arguments.ymax - arguments.ymin;
    double dz = arguments.zmax - arguments.zmin;

    arguments.volume = dx * dy * dz;

    return arguments;
}

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
    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) {
        Help();
        return 0;
    }

    int master = 0; // номер мастер-процесса
    double accurate = 1.0 / 364; // точное значение интеграла

    Arguments arguments;

    try {
        arguments = ParseArguments(argc, argv);
    }
    catch (const char *error) {
        cout << error << endl;
        return -1;
    }

    double startTime = MPI_Wtime();

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

            if (arguments.debug) {
                cout << "Loop " << loops << ": " << integrate << " (accurate: " << accurate << ", defference: " << fabs(accurate - integrate) << ")" << endl;
            }

            needAnother = fabs(integrate - accurate) > arguments.eps;
            MPI_Bcast(&needAnother, 1, MPI_INT, master, MPI_COMM_WORLD);
        }

        delete[] points;

        cout << "Processors: " << arguments.size << endl;
        cout << "Eps: " << arguments.eps << endl;
        cout << "Points per process: " << arguments.points << endl;
        cout << "Dimensions: [" << arguments.xmin << ", " << arguments.xmax << "] x [" << arguments.ymin << ", " << arguments.ymax << "] x [" << arguments.zmin << ", " << arguments.zmax << "]" << endl;
        cout << "Volume: " << arguments.volume << endl;
        cout << "Loops: " << loops << endl;
        cout << "Total number of points: " << (totalPoints * loops) << endl;
        cout << "Target integrate value: " << accurate << endl;
        cout << "Calculated integrate value: " << integrate << endl;
        cout << "Error: " << fabs(integrate - accurate) << endl;
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

    double endTime = MPI_Wtime();
    double delta = endTime - startTime;
    double maxTime, minTime, avgTime;

    MPI_Reduce(&delta, &maxTime, 1, MPI_DOUBLE,MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&delta, &minTime, 1, MPI_DOUBLE, MPI_MIN, 0,MPI_COMM_WORLD);
    MPI_Reduce(&delta, &avgTime, 1, MPI_DOUBLE, MPI_SUM, 0,MPI_COMM_WORLD);

    if (arguments.rank == master) {
        cout << "Max time: " << maxTime << endl;
        cout << "Min time: " << minTime << endl;
        cout << "Average time: " << avgTime / arguments.size << endl;
    }

    MPI_Finalize();
    return 0;
}
