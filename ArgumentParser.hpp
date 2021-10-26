#pragma once

#include <cstring>

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

    char *output; // путь к выходному файлу
    double volume; // объём области
};

class ArgumentParser {
    bool IsInteger(const char *s) const;
    bool IsReal(const char *s) const;
public:
    Arguments Parse(int argc, char **argv) const;
    void Help() const;
};

bool ArgumentParser::IsInteger(const char *s) const {
    for (int i = s[0] == '-' ? 1 : 0; s[i]; i++)
        if (s[i] < '0' || s[i] > '9') // если не цифра
            return false; // то не целое число

    return true; // целое число
}

bool ArgumentParser::IsReal(const char *s) const {
    int points = 0; // количество точек
    bool exponent = false; // была ли экспонента

    for (int i = s[0] == '-' ? 1 : 0; s[i]; i++) {
        if (s[i] == '.' || s[i] == ',') { // если точка
            points++; // увеличиваем счётчик точек

            if (points > 1 || exponent) // если больше одной точки или была экспонента
                return false; // то не вещественное число
        }
        else if (s[i] == 'e') {
            if (exponent) // если экспонента уже была
                return false; // то не вещественное число

            exponent = true; // обновляем флаг
        }
        else if (s[i] == '-' || s[i] == '+') {
            if (!exponent || s[i - 1] != 'e') // если не было экспоненты, но знак
                return false; // то не число
        }
        else if (s[i] < '0' || s[i] > '9') { // если не цифра
            return false; // то не вещественное число
        }
    }

    return true; // вещественное число
}

void ArgumentParser::Help() const {
    std::cout << "Usage: ./integrate [-e eps] [-n points] [-o path] [-d] [-xmin xmin] [-xmax xmax] [-ymin ymin] [-ymax ymax] [-zmin zmin] [-zmax zmax]" << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "  -e    - required accuracy, default - 1e-3" << std::endl;
    std::cout << "  -n    - number of point for process, default - 1000" << std::endl;
    std::cout << "  -d    - debug mode, not used by default" << std::endl;
    std::cout << "  -o    - path to print all stdout, \"output.txt\" by default" << std::endl;
    std::cout << "  -xmin - left bound on x axis, default - 0" << std::endl;
    std::cout << "  -xmax - right bound on x axis, default - 1" << std::endl;
    std::cout << "  -ymin - left bound on y axis, default - 0" << std::endl;
    std::cout << "  -ymax - right bound on y axis, default - 1" << std::endl;
    std::cout << "  -zmin - left bound on z axis, default - 0" << std::endl;
    std::cout << "  -zmax - right bound on z axis, default - 1" << std::endl;
}

Arguments ArgumentParser::Parse(int argc, char **argv) const {
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
    arguments.output = NULL;

    for (int i = 1; i < argc; i += 2) {
        if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--eps")) {
            if (i >= argc - 1)
                throw "-e argument value missed";

            if (!IsReal(argv[i + 1]))
                throw "-e argument value is not real";

            arguments.eps = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--points")) {
            if (i >= argc - 1)
                throw "-n argument value missed";

            if (!IsInteger(argv[i + 1]))
                throw "-n argument value is not integer";

            arguments.points = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")) {
            if (i >= argc - 1)
                throw "-o argument value missed";

            arguments.output = argv[i + 1];
        }
        else if (!strcmp(argv[i], "-xmin")) {
            if (i >= argc - 1)
                throw "-xmin argument value missed";

            if (!IsReal(argv[i + 1]))
                throw "-xmin argument value is not real";

            arguments.xmin = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-xmax")) {
            if (i >= argc - 1)
                throw "-xmax argument value missed";

            if (!IsReal(argv[i + 1]))
                throw "-xmax argument value is not real";

            arguments.xmax = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-ymin")) {
            if (i >= argc - 1)
                throw "-ymin argument value missed";

            if (!IsReal(argv[i + 1]))
                throw "-ymin argument value is not real";

            arguments.ymin = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-ymax")) {
            if (i >= argc - 1)
                throw "-ymax argument value missed";

            if (!IsReal(argv[i + 1]))
                throw "-ymax argument value is not real";

            arguments.ymax = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-zmin")) {
            if (i >= argc - 1)
                throw "-zmin argument value missed";

            if (!IsReal(argv[i + 1]))
                throw "-zmin argument value is not real";

            arguments.zmin = atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-zmax")) {
            if (i >= argc - 1)
                throw "-zmax argument value missed";

            if (!IsReal(argv[i + 1]))
                throw "-zmax argument value is not real";

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