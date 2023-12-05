#pragma once

//#define max length of 2D array
#define TMPSTRLEN 30
#define TSTSTRLEN 1000

typedef struct col_cell {
   
   int n;
   bool set; 

}col_cell;

struct bsa {

   col_cell *p[BSA_ROWS]; //array of 30 col_cell pointers 

}; 

int k2row_len(int k); // go from row number to length of row 

void row_alloc(bsa* b, int row);

int indx2row(int indx);

int max_row_indx(int row);

int indx2col(int indx);

bool bsa_is_empty(bsa* b); 

int top_live_row(bsa* b); // return number highest allocated rowcd .

int row_indx2indx(int row, int row_indx); //go from index in row to absolute index

bool is_row_empty(bsa* b, int row); // true if no element of row is set

int maxrowindx(bsa* b, int row);


