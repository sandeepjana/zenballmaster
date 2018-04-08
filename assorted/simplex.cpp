#if 0

//page 20 of references\simplex-solving-linear-programs.pdf
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

int numcols = 0;
int numrows = 0;

double table[4 + 1][8];

void print_table();

int find_pivot_var()
{
    //the max of coefficients in objective function
    int lastrow = numrows - 1;
    auto it = std::max_element(&table[lastrow][0], &table[lastrow][numcols - 1]);
    if (*it <= 0)
        return -1; //reached optimal state

    return std::distance(&table[lastrow][0], it);
}

int find_pivot_row(int s)
{
    //check for unbounded case:
    //if there is no single +ve coeff in s-column, 
    //we can increase the x_s as much as we want in the objective function.
    auto nopositve = true;

    //find the min-ratio-test wala among the positive coeffs
    double minratio = INT_MAX;

    int r = 0;
    for (int i = 0; i < numrows - 1; ++i)
    {
        double bi = table[i][numcols - 1]; //always positive
        double as = table[i][s];
        if (as > 0.0 && as * minratio >  bi)
        {
            nopositve = false;
            minratio = bi / as;
            r = i;
        }
    }

    return (true == nopositve) ? -1 : r;
}

void normalize_row(int r, int s)
{
    double ars = table[r][s];
    for (int j = 0; j < numcols; ++j)
    {
        table[r][j] /= ars;
    }
}

void change_basis(int r, int s)
{
    //for all constraint rows + objectiv row
    for (int i = 0; i < numrows; ++i)
    {
        if (i == r)
            continue;

        double m = table[i][s];
        for (int j = 0; j < numcols; ++j)
        {
            //use the r-th row to eliminate x_s in all rows except r-th row
            table[i][j] = table[i][j] - m * table[r][j];
        }
    }
}

double apply_simplex()
{
    while (1)
    {
        print_table();

        int s = find_pivot_var();
        if (-1 == s)
        {
            //arrived at optimal state
            return -1 * table[numrows - 1][numcols - 1];
        }

        int r = find_pivot_row(s);
        if (-1 == r)
        {
            //unbounded case
            return INT_MAX;
        }

        normalize_row(r, s);
        change_basis(r, s); //eliminate this var from all rows except r-th row
    }
    
    return 0.0;
}

void read_table(std::ifstream& input)
{
    input >> numrows >> numcols;
    for (int i = 0; i < numrows; ++i)
        for (int j = 0; j < numcols; ++j)
            input >> table[i][j];
}
void print_table()
{
    for (int i = 0; i < numrows; ++i)
    {
        for (int j = 0; j < numcols; ++j)
        {
            std::cout << table[i][j] << '\t';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

int main(int, char**)
{
    auto input = (std::ifstream("simplex_data.txt"));
    int numtc;
    input >> numtc;

    for (int i = 0; i < numtc; ++i)
    {
        read_table(input);

        auto max = apply_simplex();
        std::cout << "maximum value " << max << '\n';
    }

    return 0;
}

#endif