#pragma once

//#define max length of 2D array

typedef struct col_cell {
   
   int n;
   bool set; 

}col_cell;

struct bsa {

   col_cell *p[BSA_ROWS]; //array of 30 col_cell pointers 

}; 

int n2row_len(int n); // go from row number to length of row 

void row_alloc(bsa* b, int row);

int indx2row(int indx);

int max_row_indx(int row);

int indx2col(int indx);

