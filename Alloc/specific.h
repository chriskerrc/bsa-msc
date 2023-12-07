#pragma once

#define OPN_CUR_BR '{'
#define CLS_CUR_BR '}'
#define OPN_SQ_BR '['
#define CLS_SQ_BR ']'
#define EQ '='
#define FMT_ST_SPACE "%c%i%c%c%i " 
#define FMT_ST_NSPACE "%c%i%c%c%i" 
#define SHIFT_OFFSET 2
#define CHAR_LEN_INT_MAX 10+1
#define TMPSTRLEN 3+2*CHAR_LEN_INT_MAX //'[' + ']' + '=' + 2 * LEN_INT_MAX
#define TSTSTRLEN 1000
#define MAX_INDEX 1073741823-1

typedef struct cell {
   
   int n;
   bool set; 

}cell;

struct bsa {

   cell *p[BSA_ROWS];

}; 

int k_to_row_len(int k); 
void allocate_row(bsa* b, int row);
int indx_to_row(int indx);
int max_row_indx(int row);
int indx_to_col(int indx);
bool bsa_is_empty(bsa* b); 
int top_live_row(bsa* b); 
int row_indx_to_indx(int row, int row_indx); 
bool is_row_empty(bsa* b, int row); 
int max_set_row_indx(bsa* b, int row);
void valid_row_to_string(bsa* b, char* str, char* tmp, int row); 
bool data_inserted(bsa* b, int row, int col, int d);
bool cell_is_set(bsa* b, int row, int col);
bool row_is_null(bsa* b, int row);
bool k_is_valid(int k);
bool indx_is_valid(int indx);
void add(int* p, int* n);
