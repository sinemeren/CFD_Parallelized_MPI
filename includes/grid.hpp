

#ifndef CFDLAB_GRID_H
#define CFDLAB_GRID_H
#include <vector>
#include "cell.hpp"
#include "datastructures.hpp"

/*
 * CamelCase for function that returns or sets Objects
 * snake_case for functions that return a specific attribute like imax or jmax
 */

class Grid {
public:
    Grid(int imax_init, int jmax_init, int boundary_size, double& PI, double& UI, double& VI);
    
    // Get and Set Velocity
    void velocity(matrix<double>& vec, velocity_type type);
    void set_velocity(matrix<double>& vec, velocity_type type);

    // Get and Set Velocity
    void pressure(matrix<double>& vec);
    void set_pressure(matrix<double>& vec);

    // get specific row or column
    void cells(std::vector<Cell>& cells, matrix_selection m, int index);
    void set_cells(std::vector<Cell>& cells, matrix_selection m, int index);
    
    // specific row and column
    Cell& cell(int i, int j);
    void set_cell(Cell& cell, int i, int j);

    void innercells(matrix<Cell>& cells);
    void set_innercells(matrix<Cell>& cells);

    // Get Dimensions
    int imax() const;
    int jmax() const;

    // i/jmax with borders
    int imaxb() const;
    int jmaxb() const;

    // Print matrices
    void print_velocity(velocity_type type);
    void print_pressure();

private:
    matrix<Cell> _cells;
    std::array<matrix<double>, 2> _velocities;
    const int _imax;
    const int _jmax;
    const int _imax_b;
    const int _jmax_b;
    const int _boundary_size;

};

#endif //CFDLAB_GRID_H
