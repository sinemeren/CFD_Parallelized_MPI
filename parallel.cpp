#include "parallel.hpp"
#include <mpi.h>

void init_parallel(int iproc,
               int jproc,
               int imax,
               int jmax,
               int *myrank,
               int *il,
               int *ir,
               int *jb,
               int *jt,
               int *l_rank,
               int *r_rank,
               int *b_rank,
               int *t_rank,
               int *omg_i,
               int *omg_j,
               int num_proc)
{

  *omg_i = (*myrank % iproc) + 1;
  *omg_j = ((*myrank) / iproc) + 1;

  *il = (*omg_i-1)*(imax/iproc) + 1;
  *ir = (*omg_i)*(imax/iproc);

  *jb = (*omg_j-1)*(jmax/jproc) + 1;
  *jt = (*omg_j)*(jmax/jproc);

  if(*il == 1)      *l_rank = MPI_PROC_NULL;
  else              *l_rank = *myrank - 1;

  if(*ir == imax)   *r_rank = MPI_PROC_NULL;
  else              *r_rank = *myrank + 1;


  if(*jb == 1)      *b_rank = MPI_PROC_NULL;
  else              *b_rank = *myrank - iproc;

  if(*jt == jmax)   *t_rank = MPI_PROC_NULL;
  else              *t_rank = *myrank + iproc;


MPI_Barrier(MPI_COMM_WORLD);
}

void pressure_comm(	matrix<double> &P,
					int il, int ir,
					int jb, int jt,
					int l_rank,int r_rank,int b_rank, int t_rank,
					double *bufSend,double *bufRecv,
					MPI_Status *status)

{
	int x_dim = ir - il + 1;
	int y_dim = jt - jb + 1;

    //Send to left - receive from right
	if (l_rank != MPI_PROC_NULL)
	{
		for(int j=1; j<=y_dim; j++)
		{
			bufSend[j-1] = P[1][j];
		}
		MPI_Send( bufSend, y_dim, MPI_DOUBLE, l_rank, 1, MPI_COMM_WORLD );
	}

	if (r_rank != MPI_PROC_NULL)
	{
		MPI_Recv( bufRecv, y_dim, MPI_DOUBLE, r_rank, 1, MPI_COMM_WORLD, status );
		for(int j=1; j<=y_dim; j++)
		{
			P[x_dim+1][j] = bufRecv[j-1];
		}
  }

  // send to right - receive from left
  if (r_rank != MPI_PROC_NULL)
  {
		for(int j=1; j<=y_dim; j++)
		{
			bufSend[j-1] = P[x_dim][j];
		}
		MPI_Send( bufSend, y_dim, MPI_DOUBLE, r_rank, 1, MPI_COMM_WORLD );
	}


	if (l_rank != MPI_PROC_NULL)
	{
		MPI_Recv(bufRecv, y_dim, MPI_DOUBLE, l_rank, 1, MPI_COMM_WORLD, status );
		for(int j=1; j<=y_dim; j++)
		{
			P[0][j] = bufRecv[j-1];
		}
	}

  // send to bottom - receive from top
	if (t_rank != MPI_PROC_NULL)
	{
		for(int i=1; i<=x_dim; i++)
		{
			bufSend[i-1] = P[i][y_dim];
		}
		MPI_Send( bufSend, x_dim, MPI_DOUBLE, t_rank, 1, MPI_COMM_WORLD );
	}

	if (b_rank != MPI_PROC_NULL)
	{
		MPI_Recv( bufRecv, x_dim, MPI_DOUBLE, b_rank, 1, MPI_COMM_WORLD, status );
		for(int i=1; i<=x_dim; i++)
		{
			P[i][0] =bufRecv[i-1];
		}
  }

    // send to bottom -  receive from top
  if (b_rank != MPI_PROC_NULL)
	{
		for(int i=1; i<=x_dim; i++)
		{
			bufSend[i-1] = P[i][1];
		}
		MPI_Send( bufSend, x_dim, MPI_DOUBLE, b_rank, 1, MPI_COMM_WORLD );
	}

	if (t_rank != MPI_PROC_NULL)
	{
		MPI_Recv( bufRecv, x_dim, MPI_DOUBLE, t_rank, 1, MPI_COMM_WORLD, status );
		for(int i=1; i<=x_dim; i++)
		{
			P[i][y_dim+1] = bufRecv[i-1];
		}
	}

}

void uv_comm(matrix<double> &U,
		matrix<double> &V,
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
            MPI_Status *status)

{
  int xdim = ir-il+1;
  int ydim = jt-jb+1;

  if(l_rank != MPI_PROC_NULL && r_rank != MPI_PROC_NULL)
  {

    //Send to left - receive from right
    for (int j = 1; j <=ydim; ++j)
    {
      bufSend[j-1] = U[2][j];
    }
    MPI_Sendrecv(bufSend, ydim, MPI_DOUBLE, l_rank, 1,
              bufRecv, ydim, MPI_DOUBLE, r_rank, 1, MPI_COMM_WORLD, status);
    for (int j = 1; j <=ydim; ++j)
    {
      U[xdim+1][j] = bufRecv[j-1];
    }
    // send to right - receive from left
    for (int j = 1; j <=ydim; ++j)
    {
      bufSend[j-1] = U[xdim-1][j];
    }
    MPI_Sendrecv(bufSend, ydim, MPI_DOUBLE, r_rank, 1,
      bufRecv, ydim, MPI_DOUBLE, l_rank, 1, MPI_COMM_WORLD, status);
    for (int j = 1; j <=ydim; ++j)
    {
      U[0][j] = bufRecv[j-1];
    }


    //Send to left - receive from right
    for (int j = 1; j <=ydim+1; ++j)
    {
      bufSend[j-1] = V[1][j];
    }
    MPI_Sendrecv(bufSend, ydim+1, MPI_DOUBLE, l_rank, 1,
              bufRecv, ydim+1, MPI_DOUBLE, r_rank, 1, MPI_COMM_WORLD, status);

    for (int j = 1; j <=ydim+1; ++j)
    {
      V[xdim+1][j] = bufRecv[j-1];
    }

  // send to right - receive from left
    for (int j = 1; j <=ydim+1; ++j)
    {
      bufSend[j-1] = V[xdim][j];
    }
    MPI_Sendrecv(bufSend, ydim+1, MPI_DOUBLE, r_rank, 1,
                bufRecv, ydim+1, MPI_DOUBLE, l_rank, 1, MPI_COMM_WORLD, status);
    for (int j = 1; j <=ydim+1; ++j)
    {
      V[0][j] = bufRecv[j-1];
    }
  }

  // send and receive from the right
else if(l_rank == MPI_PROC_NULL && r_rank != MPI_PROC_NULL)
{

  for (int j = 1; j <=ydim; ++j)
  {
    bufSend[j-1] = U[xdim-1][j];
  }
  MPI_Send(bufSend, ydim, MPI_DOUBLE, r_rank, 1, MPI_COMM_WORLD);

  MPI_Recv(bufRecv, ydim, MPI_DOUBLE, r_rank, 1, MPI_COMM_WORLD, status);
  for (int j = 1; j <=ydim; ++j)
  {
    U[xdim+1][j] = bufRecv[j-1];
  }


  for (int j = 1; j <=ydim+1; ++j)
  {
    bufSend[j-1] = V[xdim][j];
  }
  MPI_Send(bufSend, ydim+1, MPI_DOUBLE, r_rank, 1, MPI_COMM_WORLD);

  MPI_Recv(bufRecv, ydim+1, MPI_DOUBLE, r_rank, 1, MPI_COMM_WORLD, status);
  for (int j = 1; j <=ydim+1; ++j)
  {
    V[xdim+1][j] = bufRecv[j-1];
  }

}

else if(l_rank != MPI_PROC_NULL && r_rank == MPI_PROC_NULL)
{

  for (int j = 1; j <=ydim; ++j)
  {
    bufSend[j-1] = U[2][j];
  }
  MPI_Send(bufSend, ydim, MPI_DOUBLE, l_rank, 1, MPI_COMM_WORLD);

  MPI_Recv(bufRecv, ydim, MPI_DOUBLE, l_rank, 1, MPI_COMM_WORLD, status);
  for (int j = 1; j <=ydim; ++j)
  {
    U[0][j] = bufRecv[j-1];
  }


  for (int j = 1; j <=ydim+1; ++j)
  {
    bufSend[j-1] = V[1][j];
  }
  MPI_Send(bufSend, ydim+1, MPI_DOUBLE, l_rank, 1, MPI_COMM_WORLD);

  MPI_Recv(bufRecv, ydim+1, MPI_DOUBLE, l_rank, 1, MPI_COMM_WORLD, status);
  for (int j = 1; j <=ydim+1; ++j)
  {
    V[0][j] = bufRecv[j-1];
  }
}




if(b_rank != MPI_PROC_NULL && t_rank != MPI_PROC_NULL)
{

  // send to top receive from bottom
  for (int i = 1; i <=xdim+1; ++i)
  {
    bufSend[i-1] = U[i][ydim];
  }
  MPI_Sendrecv(bufSend, xdim+1, MPI_DOUBLE, t_rank, 1,
     bufRecv, xdim+1, MPI_DOUBLE, b_rank, 1, MPI_COMM_WORLD, status);

  for (int i = 1; i <=xdim+1; ++i)
  {
    U[i][0] = bufRecv[i-1];
  }

  // send to bottom receive from top
  for (int i = 1; i <=xdim+1; ++i)
  {
    bufSend[i-1] = U[i][1];
  }
  MPI_Sendrecv(bufSend, xdim+1, MPI_DOUBLE, b_rank, 1,
     bufRecv, xdim+1, MPI_DOUBLE, t_rank, 1, MPI_COMM_WORLD, status);
  for (int i = 1; i <=xdim+1; ++i)
  {
    U[i][ydim+1] = bufRecv[i-1];
  }


  // send to top receive from bottom
  for (int i = 1; i <=xdim; ++i)
  {
    bufSend[i-1] = V[i][ydim-1];
  }
  MPI_Sendrecv(bufSend, xdim, MPI_DOUBLE, t_rank, 1,
     bufRecv, xdim, MPI_DOUBLE, b_rank, 1, MPI_COMM_WORLD, status);
  for (int i = 1; i <=xdim; ++i)
  {
    V[i][0] = bufRecv[i-1];
  }
  // send to bottom receive from top
  for (int i = 1; i <=xdim; ++i)
  {
    bufSend[i-1] = V[i][1];
  }
  MPI_Sendrecv(bufSend, xdim, MPI_DOUBLE, b_rank, 1,
     bufRecv, xdim, MPI_DOUBLE, t_rank, 1, MPI_COMM_WORLD, status);
  for (int i = 1; i <=xdim; ++i)
  {
    V[i][ydim+1] = bufRecv[i-1];
  }
}

//Send and receive from the bottom
else if(t_rank == MPI_PROC_NULL && b_rank != MPI_PROC_NULL)
{

  MPI_Recv(bufRecv, xdim+1, MPI_DOUBLE, b_rank, 1, MPI_COMM_WORLD, status);
  for (int i = 1; i <=xdim+1; ++i)
  {
    U[i][0] = bufRecv[i-1];
  }
  for (int i = 1; i <=xdim+1; ++i)
  {
    bufSend[i-1] = U[i][1];
  }
  MPI_Send(bufSend, xdim+1, MPI_DOUBLE, b_rank, 1, MPI_COMM_WORLD);


  MPI_Recv(bufRecv, xdim, MPI_DOUBLE, b_rank, 1, MPI_COMM_WORLD, status);
  for (int i = 1; i <=xdim; ++i)
  {
    V[i][0] = bufRecv[i-1];
  }
  for (int i = 1; i <=xdim; ++i)
  {
    bufSend[i-1] = V[i][1];
  }
  MPI_Send(bufSend, xdim, MPI_DOUBLE, b_rank, 1, MPI_COMM_WORLD);
}

else if(t_rank != MPI_PROC_NULL && b_rank == MPI_PROC_NULL)
{

  for (int i = 1; i <=xdim+1; ++i)
  {
    bufSend[i-1] = U[i][ydim];
  }
  MPI_Send(bufSend, xdim+1, MPI_DOUBLE, t_rank, 1, MPI_COMM_WORLD);

  MPI_Recv(bufRecv, xdim+1, MPI_DOUBLE, t_rank, 1, MPI_COMM_WORLD, status);
  for (int i = 1; i <=xdim+1; ++i)
  {
    U[i][ydim+1] = bufRecv[i-1];
  }


  for (int i = 1; i <=xdim; ++i)
  {
    bufSend[i-1] = V[i][ydim-1];
  }
  MPI_Send(bufSend, xdim, MPI_DOUBLE, t_rank, 1, MPI_COMM_WORLD);

  MPI_Recv(bufRecv, xdim, MPI_DOUBLE, t_rank, 1, MPI_COMM_WORLD, status);
  for (int i = 1; i <=xdim; ++i)
  {
    V[i][ydim+1] = bufRecv[i-1];
  }
}

}
