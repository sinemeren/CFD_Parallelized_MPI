#include "sor.hpp"
#include <cmath>
#include "parallel.hpp"

void sor(
  double omg,
  double dx,
  double dy,
  matrix<double> &P,
  matrix<double> &RS,
  double *res,
  int il,
  int ir,
  int jb,
  int jt,
  int l_rank,
  int r_rank,
  int b_rank,
  int t_rank,
  double *bufSend,
  double *bufRecv,
  int imax,
  int jmax,
  MPI_Status status
 )
{
	int i, j;
	double rloc;
	double coeff = omg/(2.0*(1.0/(dx*dx)+1.0/(dy*dy)));
	int xdim = ir - il + 1;
	int ydim = jt - jb + 1;

	// Set boundary values
	if(l_rank == MPI_PROC_NULL)
	{
		for(j = 0; j <= ydim; j++) {
			P.at(0).at(j) = P.at(1).at(j);
		}
	}
	if(r_rank == MPI_PROC_NULL)
	{
		for(j = 0; j <= ydim; j++) {
			P.at(xdim+1).at(j) = P.at(xdim).at(j);
		}
	}

	if(t_rank == MPI_PROC_NULL)
	{
		for(i = 0; i <= xdim; i++) {
			P.at(i).at(ydim+1) = P.at(i).at(ydim);
		}
	}
	if(b_rank == MPI_PROC_NULL)
	{
		for(i = 0; i <= xdim; i++) {
			P.at(i).at(0) = P.at(i).at(1);
		}
	}

	// SOR iteration
	for(i = 1; i <= xdim; i++) {
		for(j = 1; j <= ydim; j++) {
			P.at(i).at(j) = (1.0-omg)*P.at(i).at(j)
			                            + coeff*(( P.at(i+1).at(j)+P.at(i-1).at(j))/(dx*dx) + ( P.at(i).at(j+1)+P.at(i).at(j-1))/(dy*dy) - RS.at(i).at(j));
		}
	}
	// Communicate pressure
	  pressure_comm(P, il, ir, jb, jt, l_rank, r_rank, b_rank, t_rank, bufSend, bufRecv, &status);

	// compute the residual
	for(i = 1; i <= xdim; i++) {
		for(j = 1; j <= ydim; j++) {
			rloc += ( (P.at(i+1).at(j)-2.0*P.at(i).at(j)+P.at(i-1).at(j))/(dx*dx) + ( P.at(i).at(j+1)-2.0*P.at(i).at(j)+P.at(i).at(j-1))/(dy*dy) - RS.at(i).at(j))*
								( (P.at(i+1).at(j)-2.0*P.at(i).at(j)+P.at(i-1).at(j))/(dx*dx) + ( P.at(i).at(j+1)-2.0*P.at(i).at(j)+P.at(i).at(j-1))/(dy*dy) - RS.at(i).at(j));
		}
	}

	  MPI_Allreduce(&rloc, res, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	  *res = sqrt(*res/(imax*jmax));
	}
