#include "mpi.h"
#include <stdio.h>
#include "datastructures.hpp"

void init_parallel (int iproc, int jproc,
                    int imax,int jmax, int *myrank,
                    int *il, int *ir,
                    int *jb, int *jt,
                    int *rank_l, int *rank_r,
                    int *rank_b,int *rank_t,
                    int *omg_i, int *omg_j, int num_proc);

void pressure_comm (matrix<double> &P,
                    int il, int ir,
                    int jb,int jt,
                    int rank_l,int rank_r,
                    int rank_b,int rank_t,
                    double *bufSend,double *bufRecv, 
                    MPI_Status *status);

void uv_comm   (matrix<double> &U,matrix<double> &V,
                int il,int ir, 
                int jb,int jt,
                int rank_l,int rank_r,
                int rank_b,int rank_t,
                double *bufSend, double *bufRecv, 
                MPI_Status *status);

