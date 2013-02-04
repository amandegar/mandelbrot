#ifndef ARGUMENT_H
#define ARGUMENT_H

enum _RUNNING_MODE{MODE_THREAD, MODE_MPI, MODE_GPU, MODE_UNKNOWN};

struct __parameterCurrent{
    _RUNNING_MODE R_MODE;
    int rowMax;
    int colMax;
    int Passes;
};

#endif // ARGUMENT_H
