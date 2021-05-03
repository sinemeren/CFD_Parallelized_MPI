#include "helper.hpp"
#include "visual.hpp"
#include "init.hpp"
#include "sor.hpp"
#include <cstdio>
#include <iostream>
#include "uvp.hpp"
#include "boundary_val.hpp"
#include "parallel.hpp"
#include "mpi.h"

/**
 * The main operation reads the configuration file, initializes the scenario and
 * contains the main loop. So here are the individual steps of the algorithm:
 *
 * - DONE - read the program configuration file using read_parameters()
 * - DONE - set up the matrices (arrays) needed. Use the predefined matrix<typename> type and give initial values in the constructor.
 * - DONE - perform the main loop
 * - at the end: destroy any memory allocated and print some useful statistics
 *
 * The layout of the grid is decribed by the first figure below, the enumeration
 * of the whole grid is given by the second figure. All the unknowns corresond
 * to a two-dimensional degree of freedom layout, so they are not stored in
 * arrays, but in a matrix.
 *
 * @image html grid.jpg
 *
 * @image html whole-grid.jpg
 *
 * Within the main loop, the following steps are required (for some of the
 * operations, a definition is defined already within uvp.h):
 *
 * - DONE - calculate_dt() Determine the maximal time step size.
 * - DONE - boundaryvalues() Set the boundary values for the next time step.
 * - DONE - calculate_fg() Determine the values of F and G (diffusion and confection).
 *   This is the right hand side of the pressure equation and used later on for
 *   the time step transition.
 * - DONE - calculate_rs()
 * - DONE - Iterate the pressure poisson equation until the residual becomes smaller
 *   than eps or the maximal number of iterations is performed. Within the
 *   iteration loop the operation sor() is used.
 * - DONE - calculate_uv() Calculate the velocity at the next time step.
 */

int main(int argc, char* argv[]){

  // dat file
	std::string szFileName = "cavity100.dat";

  // decleration of variables
  double Re, UI, VI, PI, GX, GY, t_end, xlength, ylength, dt, dx, dy, alpha, omg, tau, eps, dt_value;
  int imax, jmax, itermax;
  int iproc, jproc, num_proc, myrank, il, ir, jb, jt, l_rank, r_rank, b_rank, t_rank, omg_i, omg_j;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Status status;

  read_parameters(szFileName, &Re, &UI, &VI, &PI, &GX, &GY, &t_end,&xlength, &ylength, &dt, &dx, &dy, &imax, &jmax, &alpha, &omg, &tau, &itermax, &eps, &dt_value, &iproc, &jproc);

  if (argc != 1){
	  printf("Invalid run. Example: mpirun -np 6 ./sim  \n");
	  MPI_Finalize();
	  return EXIT_FAILURE;
  }

  //Initializing the parallel parameters to broadcast
  init_parallel(iproc, jproc, imax, jmax, &myrank, &il, &ir, &jb, &jt, &l_rank, &r_rank, &b_rank, &t_rank, &omg_i, &omg_j, num_proc);

  int xdim = ir-il+1;
  int ydim = jt-jb+1;

  // setting up the matrices and assigning initial values
  matrix<double> F(xdim+3, std::vector<double>(ydim+2, 0.0));
  matrix<double> G(xdim+2, std::vector<double>(ydim+3, 0.0));
  matrix<double> RS(xdim+2, std::vector<double>(xdim+2, 0.0));
  matrix<double> U(xdim+3, std::vector<double>(ydim+2, 0.0));
  matrix<double> V(xdim+2, std::vector<double>(ydim+3, 0.0));
  matrix<double> P(xdim+2, std::vector<double>(ydim+2, 0.0));

  double t = 0;
  int iter_visual=0;

  int buffsize = (xdim)>(ydim)?(xdim+1):(ydim+1);
  double *bufSend = (double*)malloc(sizeof(double)*2*buffsize);
  double *bufRecv = (double*)malloc(sizeof(double)*2*buffsize);
  
  while (t < t_end)
  {
	// setting the boundary values
    boundaryvalues(xdim, ydim, U, V,l_rank, r_rank, b_rank, t_rank );

    // calculation of F and G matrices
    calculate_fg(Re, GX, GY, alpha, dt, dx, dy, xdim, ydim, U, V, F, G, l_rank,r_rank, b_rank, t_rank);

    // calculation of right hand side
    calculate_rs(dt, dx, dy, xdim, ydim, F, G, RS);

	int iter = 0;
	double res = 1.0;

	// performing pressure poisson iteration
	while( iter<itermax && res>eps ){

    	sor(omg, dx, dy, P, RS, &res, il, ir, jb, jt, l_rank, r_rank, b_rank, t_rank, bufSend, bufRecv, imax,jmax,status);

    	MPI_Barrier(MPI_COMM_WORLD);

    	iter = iter +1;
    }

	// calculation of velocities
  	calculate_uv(dt, dx, dy, xdim, ydim, U, V, F, G, P);

    boundaryvalues(xdim, ydim, U, V,l_rank, r_rank, b_rank, t_rank );

    uv_comm(U, V, il, ir, jb, jt, l_rank, r_rank, b_rank, t_rank, bufSend, bufRecv, &status);

    calculate_dt(Re, tau, &dt, dx, dy, xdim, ydim, U, V);

    if (t >= iter_visual*dt_value)
  	{
    	output_uvp(U, V, P, il, ir, jb, jt, omg_i, omg_j, "Solution", iter_visual);
    	iter_visual=iter_visual+ 1;
  	}

    if(myrank ==0){
      printf("t = %f ,dt = %f, Res = %f,iterations=%d \n",t,dt,res,iter-1);
    }

    t =t + dt;

  }

    //Free memory
    free(bufSend);
    free(bufRecv);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

  return 0;
}
