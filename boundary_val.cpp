#include "mpi.h"
#include"boundary_val.hpp"

void boundaryvalues(int imax, int jmax, matrix<double> &U, matrix<double> &V, int l_rank, int r_rank,int b_rank, int t_rank)
{

 if(b_rank == MPI_PROC_NULL)
{
    for (int i =0; i <=imax+1 ; ++i)
	{
        U.at(i).at(0)  = - U.at(i).at(1);
    }
    for (int i =0; i <=imax ; ++i)
	{
        V.at(i).at(0)  = 0;
    }
}

if(t_rank == MPI_PROC_NULL)
{
    for( int i=0;i<= imax+1;++i)
    {
        U.at(i).at(jmax+1)  = 2-U.at(i).at(jmax);
    }
    for( int i=0;i<= imax;++i)
    {
        V.at(i).at(jmax) = 0;
    }
}

if(l_rank == MPI_PROC_NULL)
{
    for(int j=0;j<=jmax;++j)
    {
      U.at(0).at(j) = 0;
    }
    for(int j=0;j<=jmax+1;++j)
    {
      V.at(0).at(j) = - V.at(1).at(j);
    }

}

if(r_rank == MPI_PROC_NULL)
{
    for(int j=0;j<=jmax;++j)
    {
        U.at(imax+2).at(j) = 0;
    }
    for(int j=0;j<=jmax+1;++j)
    {
        V.at(imax+1).at(j)  = - V.at(imax).at(j);
    }
}

}
