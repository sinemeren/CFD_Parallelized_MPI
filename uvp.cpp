#include <cmath>
#include "uvp.hpp"
#include"helper.hpp"
#include"mpi.h"

// Determines the value of F and G
void calculate_fg(double Re,
		 double GX,
		 double GY,
		 double alpha,
		 double dt,
		 double dx,
		 double dy,
		 int imax,
		 int jmax,
		 matrix<double> &U,
		 matrix<double> &V,
		 matrix<double> &F,
		 matrix<double> &G,
		 int l_rank,
		 int r_rank,
		 int b_rank,
		 int t_rank)
{

for (int i = 0; i <=imax+1; i++) {
        //Boundary conditions for G
        G.at(i).at(0) = V.at(i).at(0);
        G.at(i).at(1) = V.at(i).at(1);
	    G.at(i).at(jmax+1) = V.at(i).at(jmax+1);
    }

    //Boundary conditions for F
    for (int j = 0; j <=jmax+1; j++) {
        F.at(0).at(j)=U.at(0).at(j);
        F.at(1).at(j) = U.at(1).at(j);
        F.at(imax+1).at(j) = U.at(imax+1).at(j);
    }

    for(int i=2; i<=imax; i++){

        for(int j=1; j<=jmax; j++){

        	F.at(i).at(j)=U.at(i).at(j)+dt*(

        	                (1/Re)*((U.at(i-1).at(j)-2*U.at(i).at(j)+U.at(i+1).at(j))/(dx*dx)+(U.at(i).at(j-1)-2*U.at(i).at(j)+U.at(i).at(j+1))/(dy*dy))

        	                -(1/dx)*0.25*(
        	                        ( (U.at(i+1).at(j)+U.at(i).at(j))*(U.at(i+1).at(j)+U.at(i).at(j)) - (U.at(i-1).at(j)+U.at(i).at(j))*(U.at(i-1).at(j)+U.at(i).at(j)) )
        	                        +alpha*(fabs(U.at(i+1).at(j)+U.at(i).at(j))*(U.at(i).at(j)-U.at(i+1).at(j))-fabs(U.at(i-1).at(j)+U.at(i).at(j))*(U.at(i-1).at(j)-U.at(i).at(j)))
        	                             )

        	                -(1/dy)*0.25*(
        	                        ((V.at(i-1).at(j+1)+V.at(i).at(j+1))*(U.at(i).at(j)+U.at(i).at(j+1))- (V.at(i-1).at(j)+V.at(i).at(j))*(U.at(i).at(j-1)+U.at(i).at(j)))
        	                    +alpha*(fabs(V.at(i-1).at(j+1)+V.at(i).at(j+1))*(U.at(i).at(j)-U.at(i).at(j+1))-fabs(V.at(i-1).at(j)+V.at(i).at(j))*(U.at(i).at(j-1)-U.at(i).at(j)))
        	                             )

        	                +GX);

		}
	}

	for(int i=1; i<=imax; i++){

        for(int j=2; j<=jmax; j++){

        	G.at(i).at(j)=V.at(i).at(j)+dt*(

        	                (1/Re)*((V.at(i-1).at(j)-2*V.at(i).at(j)+ V.at(i+1).at(j))/(dx*dx)+(V.at(i).at(j-1)-2*V.at(i).at(j)+ V.at(i).at(j+1))/(dy*dy))

        	                -(1/dx)*0.25*(
        	                        ((U.at(i+1).at(j-1)+U.at(i+1).at(j))*(V.at(i).at(j)+V.at(i+1).at(j))- (U.at(i).at(j-1)+U.at(i).at(j))*(V.at(i-1).at(j)+V.at(i).at(j)))
        	                    +alpha*(fabs(U.at(i+1).at(j-1)+U.at(i+1).at(j))*(V.at(i).at(j)-V.at(i+1).at(j))-fabs(U.at(i).at(j-1)+U.at(i).at(j))*(V.at(i-1).at(j)-V.at(i).at(j)))
        	                            )

        	                -(1/dy)*0.25*(
        	                        ( (V.at(i).at(j)+V.at(i).at(j+1))*(V.at(i).at(j)+V.at(i).at(j+1)) - (V.at(i).at(j-1)+V.at(i).at(j))*(V.at(i).at(j-1)+V.at(i).at(j)) )
        	                        +alpha*(fabs(V.at(i).at(j)+V.at(i).at(j+1))*(V.at(i).at(j)-V.at(i).at(j+1))-fabs(V.at(i).at(j-1)+V.at(i).at(j))*(V.at(i).at(j-1)-V.at(i).at(j)))
        	                            )
        	                +GY);

        }
	}


}




void calculate_dt(double Re,
                  double tau,
                  double *dt,
                  double dx,
                  double dy,
                  int imax,
                  int jmax,
				  matrix<double> &U,
				  matrix<double> &V)
{

  double Umax = maxElementOfMatrix(U);
  double Umax1;
  MPI_Allreduce(&Umax, &Umax1, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

  double Vmax = maxElementOfMatrix(V);
  double Vmax1;
  MPI_Allreduce(&Vmax, &Vmax1, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

  double term1 = (Re / 2.0) * ( 1.0 / ((1.0 / (dx * dx) ) + ( 1.0 / (dy * dy) ) ) );
  double term2 = dx / fabs(Umax1);
  double term3 = dy / fabs(Vmax1);

  double min1 = std::min(term1,term2);
  double min2 = std::min(min1,term3);


  //*dt = tau * min2;
  if( (tau > 0) && (tau < 1))
   {
	 *dt = tau * min2;
   }


}


void calculate_uv(double dt,double dx,double dy,int imax, int jmax, matrix<double> &U, matrix<double> &V,matrix<double> &F,matrix<double> &G,matrix<double> &P)
{
	for (int i = 1; i<= imax+1;i++)
	{
		for (int j=1; j<=jmax;j++)
		{
			U.at(i).at(j) = F.at(i).at(j) - (dt/dx)*(P.at(i).at(j)-P.at(i-1).at(j));
		}
	}

	for (int i = 1; i<= imax;i++)
	{
		for (int j=1; j<=jmax+1;j++)
		{
			V.at(i).at(j) = G.at(i).at(j) - (dt/dy)*(P.at(i).at(j) -P.at(i).at(j-1));
		}
	}
}


void calculate_rs(double dt,
		  double dx,
		  double dy,
		  int imax,
		  int jmax,
		  matrix<double> &F,
		  matrix<double> &G,
		  matrix<double> &RS)
{

for(int i=1; i<=imax; i++)
   {

        for(int j=1; j<=jmax; j++)
		{
        	RS.at(i).at(j) =  (1/dt)*( (F.at(i+1).at(j)-F.at(i).at(j))/dx + (G.at(i).at(j+1)-G.at(i).at(j))/dy );
		}

	}

}
