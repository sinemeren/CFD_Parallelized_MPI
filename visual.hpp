#ifndef __VISUAL_HPP__
#define __VISUAL_HPP__

#include "datastructures.hpp"
#include "grid.hpp"

/**
 * Method for writing header information in vtk format.
 *
 * The name of the file consists of the problem name (szProblem)
 * and of the current time step. It gets the suffix .vtk.
 *
 * @param szProblem      File pointer for writing info.
 * @param timeStepNumber Number of the current time step to be printed.
 * @param xlength Length in x-direction
 * @param ylength Length in y-direction
 * @param imax    Maximum number of entries (?) in x-direction
 * @param jmax    Maximum number of entries (?) in y-direction
 * @param dx      Mesh size in x-direction
 * @param dy      Mesh size in x-direction
 * @param U       Velocities in x-direction
 * @param V       Velocities in y-direction
 * @param P       Pressure data
 *
 * @author Tobias Neckel
 */
void write_vtkFile(const char *szProblem,
		  	  	   int    timeStepNumber,
				   double xlength,
				   double ylength,
				   int    imax,
				   int    jmax,
				   double dx,
				   double dy,
				   matrix<double> &U,
				   matrix<double> &V,
				   matrix<double> &P);

/**
 * Method for writing header information in vtk format.
 *
 * @param fp      File pointer for writing info.
 * @param imax    Maximum number of entries (minus 2) in x-direction
 * @param jmax    Maximum number of entries (minus 2) in y-direction
 * @param dx      mesh size dx
 * @param dy      mesh size dy
 *
 * @author Tobias Neckel
 */
void write_vtkHeader( FILE *fp, int imax, int jmax,
                      double dx, double dy);

/**
 * Method for writing grid coordinate information in vtk format.
 *
 * @param fp      File pointer for writing info.
 * @param imax    Maximum number of entries (minus 2) in x-direction
 * @param jmax    Maximum number of entries (minus 2) in y-direction
 * @param dx      mesh size dx
 * @param dy      mesh size dy
 *
 * @author Tobias Neckel
 */
void write_vtkPointCoordinates( FILE *fp, int imax, int jmax,
                                double dx, double dy);

class VTKHelper {
    public:
    static void printVTKFile(Grid grid, double dx, double dy,
                                 std::string casename, std::string outputdir, int timestep);
};

void output_uvp(matrix<double> &U,matrix<double> &V,matrix<double> &P,int il,int ir,int jb,int jt,int omg_i,int omg_j,const char *szProblem, int n);

#endif
