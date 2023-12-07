#pragma once

#define OPN_CUR_BR '{'
#define CLS_CUR_BR '}'
#define OPN_SQ_BR '['
#define CLS_SQ_BR ']'
#define EQ '='
#define FMT_ST_SPACE "%c%i%c%c%i " 
#define FMT_ST_NSPACE "%c%i%c%c%i" 
#define MAX_ROW 30
#define MAX_INDEX 1073741823-1
#define SHIFT_OFFSET 2
#define CHAR_LEN_INT_MAX 10
//maybe give more space to temp string because this makes me nervous...
#define TMPSTRLEN 3+2*CHAR_LEN_INT_MAX+1 //'[' + ']' + '=' + 2 * LEN_INT_MAX
#define TSTSTRLEN 1000

typedef struct cell {
   
   int n;
   bool set; 

}cell;

struct bsa {

   cell *p[BSA_ROWS]; //array of 30 col_cell pointers 

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
int max_set_row_indx(bsa* b, int row);
void vald_rw_2str(bsa* b, char* str, char* tmp, int row); // sub-function for 2 string l
bool data_inserted(bsa* b, int row, int col, int d);
bool cell_is_set(bsa* b, int row, int col);
bool row_is_null(bsa* b, int row);
bool k_is_valid(int k);
bool indx_is_valid(int indx);
