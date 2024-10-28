#include "../bsa.h"
#include "specific.h"

bsa* bsa_init(void)
{
   bsa* b = (bsa*) calloc(1, sizeof(bsa));
   if(b == NULL){
      return NULL;
   } 
   return b;
}

bool bsa_set(bsa* b, int indx, int d)
{  
   if(b == NULL){
      return false;
   }
   int row = indx_to_row(indx);
   int col = indx_to_col(indx);
   if(row_is_null(b, row)){
      allocate_row(b, row);
      data_inserted(b, row, col, d);
      return true;
   }
   if(!row_is_null(b, row)){
      data_inserted(b, row, col, d);
      return true;
   }
   return false;
}

int* bsa_get(bsa* b, int indx)
{   
   int row = indx_to_row(indx); 
   if(row_is_null(b, row)){
      return NULL;
   }
   if(!row_is_null(b, row)){
      int col = indx_to_col(indx);
      if(!cell_is_set(b, row, col)){ 
         return NULL;
      }
      if(cell_is_set(b, row, col)){
         return &b->p[row][col].n; 
      }
   } 
   return NULL;   
}

bool bsa_delete(bsa* b, int indx)
{  
   int row = indx_to_row(indx); 
   int col = indx_to_col(indx);
   if(!indx_is_valid(indx)){
      return false;
   }
   if(!cell_is_set(b, row, col)){
      return false; 
   }
   if(cell_is_set(b, row, col)){
      b->p[row][col].set = false;
      if(is_row_empty(b,row)){
         free(b->p[row]); 
         b->p[row] = NULL;  
      }
      return true; 
   }
  return false;    
}

int bsa_maxindex(bsa* b)
{  
   if(b == NULL){
      return -1;
   }
   if(bsa_is_empty(b)){
      return -1;
   }
   if(!bsa_is_empty(b)){
      int row = top_live_row(b); 
      int max_row_indx = max_set_row_indx(b, row);
      int max_indx = row_indx_to_indx(row, max_row_indx);
      return max_indx;
   }
   return -1;
}

bool bsa_tostring(bsa* b, char* str) 
{  
   if(str == NULL){
      return false;
   }
   char tmp[TMPSTRLEN] = {'\0'};  
   strcpy(str, ""); 
   int max_indx = bsa_maxindex(b);
   int max_indx_rw = indx_to_row(max_indx);
   if(b == NULL){
      return false;
   }
   for(int row = 0; row <= max_indx_rw; row++){ 
      if(row_is_null(b, row)){
         sprintf(tmp, "%c%c", OPN_CUR_BR, CLS_CUR_BR); 
         strcat(str, tmp);
      } 
      if(!row_is_null(b, row)){
         valid_row_to_string(b, str, tmp, row);
      }
   }
   return true;
} 

bool bsa_free(bsa* b)
{ 
   if(b == NULL){
      return false;
   }
   for(int row = 0; row < BSA_ROWS; row++){ 
      if(!row_is_null(b, row)){
         free(b->p[row]);
         b->p[row] = NULL;
      }
   }
   if(bsa_is_empty(b)){
      free(b);
      return true;
   }
   if(!bsa_is_empty(b)){
      return false; 
   }
   return false;
}

void bsa_foreach(void (*func)(int* p, int* n), bsa* b, int* acc) 
{  
   for(int row = 0; row < BSA_ROWS; row++){ 
      if(!row_is_null(b, row)){  
         int row_len = k_to_row_len(row);
         for(int col = 0; col < row_len; col++){ 
            if(cell_is_set(b, row, col)){
               int* ptr = &b->p[row][col].n;
               func(ptr, acc);
            }  
         }
      }       
   }   
}

int max_set_row_indx(bsa* b, int row)
{   
   if(!k_is_valid(row)){
      return -1;
   }
   int row_len = k_to_row_len(row);
   for(int col = row_len - 1; col >= 0; col--){
      if(cell_is_set(b, row, col)){
         int row_indx = col; 
         return row_indx;
      }
   }   
   return 0;
}

void valid_row_to_string(bsa* b, char* str, char* tmp, int row)
{
   int max_set_cell = max_set_row_indx(b, row);
   sprintf(tmp, "%c", OPN_CUR_BR); 
   strcat(str, tmp);
   int row_len = k_to_row_len(row);
   for(int col = 0 ; col < row_len ; col++){
      if(cell_is_set(b, row, col)){ 
         int d = b->p[row][col].n;
         int indx = row_indx_to_indx(row, col);
         if(col != max_set_cell){
            sprintf(tmp, FMT_ST_SPACE, OPN_SQ_BR, indx, CLS_SQ_BR, EQ, d); //[indx]=d+SPACE
            strcat(str, tmp);
         }
         if(col == max_set_cell){
            sprintf(tmp, FMT_ST_NSPACE, OPN_SQ_BR, indx, CLS_SQ_BR, EQ, d); //[indx]=d
            strcat(str, tmp);
         }
      }
   }
   sprintf(tmp, "%c", CLS_CUR_BR); 
   strcat(str, tmp); 
}


int indx_to_row(int indx)
{  
   if(!indx_is_valid(indx)){
      return -1; 
   }
   int shifted = 0;
   int n_shifts = 0;
   indx ++; 
   while(shifted <= indx){
      shifted = 1 << n_shifts; 
      n_shifts++; 
   }
   return n_shifts - SHIFT_OFFSET; 
}

int max_row_indx(int row)
{  
   if(!k_is_valid(row)){
      return -1;
   }
   int n_shifts = row + 1; 
   int pow2 = 1 << n_shifts;  
   int max_indx = pow2 - SHIFT_OFFSET; 
   return max_indx;
}

int indx_to_col(int indx)
{  
   if(!indx_is_valid(indx)){
      return -1; 
   }
   int row = indx_to_row(indx);
   int prev_row_offset = max_row_indx(row-1); 
   int col = indx - prev_row_offset - 1; 
   return col;
}

int row_indx_to_indx(int row, int row_indx)
{  
   if(row > 0 && row < BSA_ROWS){ 
      int prev_row = row - 1; 
      int prev_max_row_indx = max_row_indx(prev_row);
      int abs_indx = prev_max_row_indx + row_indx + 1; 
      return abs_indx;
   }
   if(row == 0){
      return 0;
   }
   if(!k_is_valid(row)){
      return -1;
   }
   return -1;
}

int k_to_row_len(int k)
{  
   int len = 1<<k;
   return len;  
}

void allocate_row(bsa* b, int row)
{ 
   int row_len = k_to_row_len(row);
   b->p[row] = (cell*) calloc(row_len, sizeof(cell));
}

bool bsa_is_empty(bsa* b)
{
   int cnt = 0; 
   for(int row = 0; row<BSA_ROWS; row++){ 
      if(row_is_null(b, row)){
         cnt++;
      } 
   }
   if(cnt == BSA_ROWS){ 
      return true;
   }
   else{
      return false;
   }
}

int top_live_row(bsa* b)
{
   if(bsa_is_empty(b)){
      return -1;
   }
   for(int row = BSA_ROWS - 1; row > -1; row--){  
      if(!row_is_null(b, row)){
         return row;
      } 
   }
   return -1; 
}

bool is_row_empty(bsa* b, int row)
{
   int cnt = 0;
   int row_len = k_to_row_len(row);
   for(int col = row_len-1; col > -1; col--){ 
      if(cell_is_set(b, row, col)){
         cnt++; 
      }
   }
   if(cnt > 0){ 
      return false;
   }
   if(cnt == 0){
      return true;
   }
   return 0;
}

bool data_inserted(bsa* b, int row, int col, int d)
{  
   if(!k_is_valid(row)){
      return false;
   }
   if(b == NULL){
      return false;
   }
   if(row_is_null(b, row)){
      return false;
   }
   b->p[row][col].n = d;
   b->p[row][col].set = true;
   return true; 
}

bool cell_is_set(bsa* b, int row, int col)
{
   if(b->p[row][col].set){
      return true;
   }
   if(!b->p[row][col].set){
      return false;
   }
   return 0;
}

bool row_is_null(bsa* b, int row)
{
  if(b->p[row]==NULL){
     return true;
  }
  if(b->p[row]!=NULL){
     return false;
  }
  return 0;
}

bool k_is_valid(int k)
{
   if(k >= 0 && k < BSA_ROWS){
      return true;
   }
   else{
      return false;
   }
}

bool indx_is_valid(int indx)
{
   if(indx >= 0 && indx <= MAX_INDEX){
      return true;
   }
   else{
      return false;
   }
}

void add(int* p, int* n)
{
   *n = *n + *p;
}

void test(void)
{  
   char tst[TSTSTRLEN];
   char tmp[TMPSTRLEN] = {'\0'};  
   //** MEMORY ALLOCATION AND FREEING TESTS **
  
   //* EMPTY BSA *

   //BSA_INIT
   bsa* b = bsa_init();
   assert(b);
   //check that 30 pointers in empty BSA are null
   for(int i = 0; i < BSA_ROWS; i++){
      assert(row_is_null(b, i));
   }
   
   //BSA_IS_EMPTY
   assert(bsa_is_empty(b)==true);

   //BSA_MAXINDEX
   assert(bsa_maxindex(b)==-1); //returns -1 for empty BSA

   //BSA_TOSTRING
   //empty BSA, no rows set i.e. BSA max index == -1
   bsa_tostring(b, tst);
   assert(strcmp(tst, "")==0);
   strcpy(tst, "");
   
   //DATA_INSERTED
   
   //returns false when try insert data into NULL BSA
   assert(!data_inserted(b, 2, 2, 1234567890));

   //ROW_IS_NULL
   for(int row = 0; row < BSA_ROWS; row++){
      assert(row_is_null(b, row));
   }
   
   //TOP_LIVE_ROW
   assert(top_live_row(b)==-1); //returns -1 for empty BSA i.e. no live rows

   //* ALLOCATE EMPTY ROW *

   allocate_row(b, 0); //allocate one row: BSA not empty
   //BSA_IS_EMPTY
   assert(bsa_is_empty(b)==false); 

   //TOP_LIVE_ROW
   assert(top_live_row(b)==0);

   //* SETTING *
   
   //BSA_SET

   //Set small data value at index 0
   assert(bsa_set(b, 0, 10));
   assert(bsa_maxindex(b)==0);
   assert(max_set_row_indx(b, 0)==0); // max set index in row 0 is row index 0 i.e. abs index 0

   //BSA_TOSTRING
   //one set element & stop printing rows after max index 
   bsa_tostring(b, tst);
   assert(strcmp(tst, "{[0]=10}")==0);
   strcpy(tst, "");
   
   //VALID_ROW_TO_STRING
   //row 0
   valid_row_to_string(b, tst, tmp, 0); 
   assert(strcmp(tst, "{[0]=10}")==0);
   strcpy(tst, "");
  
   //Set big data value at index 5
   //Allocates row 2
   assert(bsa_set(b, 5, 1234567890)); //set integer 10 digits long
   assert(bsa_maxindex(b)==5);
   assert(max_set_row_indx(b, 2)==2); // max set index in row 2 is row index 2 i.e. abs index 5
 
   //BSA_TOSTRING
   //Two set elements, different rows
   bsa_tostring(b, tst);
   assert(strcmp(tst, "{[0]=10}{}{[5]=1234567890}")==0);
   strcpy(tst, "");

   //VALID_ROW_TO_STRING
   //row 2
   valid_row_to_string(b, tst, tmp, 2); 
   assert(strcmp(tst, "{[5]=1234567890}")==0);
   strcpy(tst, "");

   //ROW_IS_NULL
   assert(!row_is_null(b, 2)); // row 2 is not null
   
   //IS_ROW_EMPTY
   assert(is_row_empty(b, 2)==false);

   //reset existing big value at index 5
   assert(bsa_set(b, 5, 1234567891));

   //Two set elements, check value has changed
   bsa_tostring(b, tst);
   assert(strcmp(tst, "{[0]=10}{}{[5]=1234567891}")==0);
   strcpy(tst, "");
   
   //BSA_GET
   //check value has been set at index 5
   int* a = bsa_get(b, 5);
   assert(*a == 1234567891);
   
   //ALLOCATE_ROW
   allocate_row(b, 3); //allocate row 3 without setting anything
   //IS_ROW_EMPTY
   assert(is_row_empty(b, 3)==true);

   //TOP_LIVE_ROW
   assert(top_live_row(b)==3);

   //ROW_IS_NULL
   assert(!row_is_null(b, 3)); // row 3 is not null

   //DATA_INSERTED
   //returns false when try insert in invalid row
   assert(!data_inserted(b, -1, 2, 1234567891));
  
   //returns true when insert data in valid row 3 abs index 9
   assert(data_inserted(b, 3, 2, 1234599999));

   //* GETTING *

   //BSA_GET
   //check value has been set
   a = bsa_get(b, 9);
   assert(*a == 1234599999);
   assert(bsa_maxindex(b)==9);
   assert(max_set_row_indx(b, 3)==2); // max set index in row 3 is row index 2 i.e. abs index 9

   //BSA_TOSTRING
   //Three set elements, three rows
   bsa_tostring(b, tst);
   assert(strcmp(tst, "{[0]=10}{}{[5]=1234567891}{[9]=1234599999}")==0);
   strcpy(tst, "");

   //VALID_ROW_TO_STRING
   //row 2
   valid_row_to_string(b, tst, tmp, 2); 
   assert(strcmp(tst, "{[5]=1234567891}")==0);
   strcpy(tst, "");

   //CELL_IS_SET
   
   //check value has been set 
   assert(cell_is_set(b, 3, 2)); //abs index 9 is set
   //returns false for unset cell 
   assert(!cell_is_set(b, 3, 3)); //nothing set at abs index 10

   //BSA_GET
    
   //set a big value at abs index 11
   assert(bsa_set(b, 11, 1111111111));
   assert(bsa_maxindex(b)==11);
   assert(max_set_row_indx(b, 3)==4); // max set index in row 3 is row index 4 i.e. abs index 11

   //BSA_TOSTRING
   //Four set elements, two in row 3, space between set elements in same row
   bsa_tostring(b, tst);
   assert(strcmp(tst, "{[0]=10}{}{[5]=1234567891}{[9]=1234599999 [11]=1111111111}")==0);
   strcpy(tst, "");
   
   //VALID_ROW_TO_STRING
   //row 3
   valid_row_to_string(b, tst, tmp, 3); 
   assert(strcmp(tst, "{[9]=1234599999 [11]=1111111111}")==0);
   strcpy(tst, "");

   //BSA_DELETE
   //delete set cell index 5
   assert(bsa_delete(b, 5)); 
   
   //BSA_TOSTRING
   //Check element at index 5 is gone
   bsa_tostring(b, tst);
   assert(strcmp(tst, "{[0]=10}{}{}{[9]=1234599999 [11]=1111111111}")==0);
   strcpy(tst, "");

   //BSA_GET

   //get big value
   a = bsa_get(b, 11);
   assert(*a == 1111111111);

   //set big value last cell of BSA row 29
   assert(bsa_set(b, 1073741822, 1111111112));
   assert(bsa_maxindex(b)==1073741822);

   //TOP_LIVE_ROW 
   assert(top_live_row(b)==29);

   //BSA_GET

   //get big value last cell of BSA row 29
   a = bsa_get(b, 1073741822);
   assert(*a == 1111111112);
  
   //* DELETING *

   //BSA_DELETE
 
   //try delete at invalid index
   assert(!bsa_delete(b, -1)); //invalid index low 
   assert(!bsa_delete(b, 1073741823)); //invalid index high
   
   //try delete unset cell: nothing set at abs indx 12
   assert(!bsa_delete(b, 12));
   
   //delete set cell index 11
   assert(bsa_delete(b, 11)); 
   
   //CELL_IS_SET
   //returns false for deleted cell 
   assert(!cell_is_set(b, 3, 4)); //abs index 11
 
   //* FREEING *

   bsa_free(b);
   
   //** FOREACH **
   
   //BSA_FOREACH & ADD

   bsa* c = bsa_init();
   bsa_set(c, 1, 1);
   bsa_set(c, 2, 2);
   bsa_set(c, 3, 4);
   int acc = 0;
   bsa_foreach(add, c, &acc);
   assert(acc==7);
   bsa_free(c);

   //** NULL BSA TESTS **

   //BSA_SET

   //try to set NULL BSA
   bsa* d = NULL;
   assert(!bsa_set(d, 5, 12)); //returns false when try set NULL BSA
   
   //DATA_INSERTED

   d = bsa_init(); //Initialize BSA d

   //returns false when try insert data into NULL row 
   assert(!data_inserted(d, 3, 2, 1234567891));

   //BSA_MAXINDEX
   assert(bsa_maxindex(d)==-1); //returns -1 for NULL BSA

   bsa_free(d);
    
   //** NO MEMORY ALLOCATION AND FREEING **

   //K_IS_VALID
   for(int k=0; k<BSA_ROWS; k++){
      assert(k_is_valid(k)==true);
   }
   assert(k_is_valid(-1)==false); //negative: out of bounds lower
   assert(k_is_valid(30)==false); //out of bounds higher

   //INDX_IS_VALID
   assert(indx_is_valid(0)==true);
   assert(indx_is_valid(1)==true);
   assert(indx_is_valid(9)==true);
   assert(indx_is_valid(100)==true);
   assert(indx_is_valid(600)==true);
   assert(indx_is_valid(600000)==true);
   assert(indx_is_valid(900000000)==true);
   assert(indx_is_valid(MAX_INDEX)==true);
   assert(indx_is_valid(-1)==false); //negative: out of bounds lower
   assert(indx_is_valid(MAX_INDEX+1)==false); //out of bounds higher

   //ROW_INDX_TO_INDX 
   assert(row_indx_to_indx(-1, 0)==-1); //invalid row low
   assert(row_indx_to_indx(30, 0)==-1); //invalid row high
   assert(row_indx_to_indx(0, 0)==0);
   assert(row_indx_to_indx(1, 0)==1);
   assert(row_indx_to_indx(2, 0)==3);
   assert(row_indx_to_indx(2, 1)==4);
   assert(row_indx_to_indx(2, 3)==6);
   assert(row_indx_to_indx(3, 4)==11);
   assert(row_indx_to_indx(3, 7)==14);
   assert(row_indx_to_indx(4, 4)==19);
   assert(row_indx_to_indx(5, 31)==62);
   assert(row_indx_to_indx(6, 63)==126);
   assert(row_indx_to_indx(7, 127)==254);
   assert(row_indx_to_indx(8, 255)==510);
   assert(row_indx_to_indx(9, 511)==1022);
   assert(row_indx_to_indx(10, 1023)==2046);
   assert(row_indx_to_indx(11, 2047)==4094);
   assert(row_indx_to_indx(12, 4095)==8190);
   assert(row_indx_to_indx(13, 8191)==16382);
   assert(row_indx_to_indx(14, 16383)==32766);
   assert(row_indx_to_indx(15, 32767)==65534);
   assert(row_indx_to_indx(16, 65535)==131070);
   assert(row_indx_to_indx(17, 131071)==262142);
   assert(row_indx_to_indx(18, 262143)==524286);
   assert(row_indx_to_indx(19, 524287)==1048574);
   assert(row_indx_to_indx(20, 1048575)==2097150);
   assert(row_indx_to_indx(21, 2097151)==4194302);
   assert(row_indx_to_indx(22, 4194303)==8388606);
   assert(row_indx_to_indx(23, 8388607)==16777214);
   assert(row_indx_to_indx(24, 16777215)==33554430);
   assert(row_indx_to_indx(25, 33554431)==67108862);
   assert(row_indx_to_indx(26, 67108863)==134217726);
   assert(row_indx_to_indx(27, 134217727)==268435454);
   assert(row_indx_to_indx(28, 268435455)==536870910);
   assert(row_indx_to_indx(29, 536870911)==1073741822);

   //INDX_TO_COL
   assert(indx_to_col(-5)==-1); //negative index returns -1
   assert(indx_to_col(1073741823)==-1); //index too high returns -1
   assert(indx_to_col(0)==0);
   assert(indx_to_col(1)==0);
   assert(indx_to_col(2)==1);
   assert(indx_to_col(3)==0);
   assert(indx_to_col(4)==1);
   assert(indx_to_col(5)==2);
   assert(indx_to_col(6)==3);
   assert(indx_to_col(7)==0);
   assert(indx_to_col(8)==1);
   assert(indx_to_col(9)==2);
   assert(indx_to_col(10)==3);
   assert(indx_to_col(11)==4);
   assert(indx_to_col(12)==5);
   assert(indx_to_col(13)==6);
   assert(indx_to_col(14)==7);
   assert(indx_to_col(15)==0); //first indx in row 4
   assert(indx_to_col(16)==1); //second indx in row 4
   assert(indx_to_col(30)==15); //last indx in row 4
   assert(indx_to_col(31)==0); //first indx in row 5
   assert(indx_to_col(35)==4); //4th indx in row 5
   assert(indx_to_col(62)==31); //last indx in row 5
   assert(indx_to_col(63)==0); //first indx in row 6
   assert(indx_to_col(69)==6); //6th indx in row 6
   assert(indx_to_col(126)==63); //last indx in row 6
   assert(indx_to_col(127)==0);//first indx in row 7
   assert(indx_to_col(137)==10);//10th indx in row 7
   assert(indx_to_col(254)==127);//last indx in row 7
   assert(indx_to_col(255)==0);//first indx in row 8
   assert(indx_to_col(510)==255); //last index row 8
   assert(indx_to_col(511)==0); //first index row 9
   assert(indx_to_col(1022)==511); //last index row 9
   assert(indx_to_col(2046)==1023); //last index row 10
   assert(indx_to_col(4094)==2047); //last index row 11
   assert(indx_to_col(8190)==4095); //last index row 12
   assert(indx_to_col(16382)==8191); //last index row 13
   assert(indx_to_col(32766)==16383); //last index row 14
   assert(indx_to_col(65534)==32767); //last index row 15
   assert(indx_to_col(131070)==65535); //last index row 16
   assert(indx_to_col(262142)==131071); //last index row 17
   assert(indx_to_col(524286)==262143); //last index row 18
   assert(indx_to_col(1048574)==524287); //last index row 19
   assert(indx_to_col(2097150)==1048575); //last index row 20
   assert(indx_to_col(4194302)==2097151); //last index row 21
   assert(indx_to_col(8388606)==4194303); //last index row 22
   assert(indx_to_col(16777214)==8388607); //last index row 23
   assert(indx_to_col(33554430)==16777215); //last index row 24
   assert(indx_to_col(67108862)==33554431); //last index row 25
   assert(indx_to_col(134217726)==67108863); //last index row 26
   assert(indx_to_col(268435454)==134217727); //last index row 27
   assert(indx_to_col(536870910)==268435455); //last index row 28
   assert(indx_to_col(1073741822)==536870911); //last index row 29

   //MAX_ROW_INDX
   assert(max_row_indx(-1)==-1); //invalid row returns -1
   assert(max_row_indx(0)==0);
   assert(max_row_indx(1)==2);
   assert(max_row_indx(2)==6);
   assert(max_row_indx(3)==14);
   assert(max_row_indx(4)==30);
   assert(max_row_indx(5)==62);
   assert(max_row_indx(6)==126);
   assert(max_row_indx(7)==254);
   assert(max_row_indx(8)==510);
   assert(max_row_indx(9)==1022);
   assert(max_row_indx(10)==2046);
   assert(max_row_indx(11)==4094);
   assert(max_row_indx(12)==8190);
   assert(max_row_indx(13)==16382);
   assert(max_row_indx(14)==32766);
   assert(max_row_indx(15)==65534);
   assert(max_row_indx(16)==131070);
   assert(max_row_indx(17)==262142);
   assert(max_row_indx(18)==524286);
   assert(max_row_indx(19)==1048574);
   assert(max_row_indx(20)==2097150);
   assert(max_row_indx(21)==4194302);
   assert(max_row_indx(22)==8388606);
   assert(max_row_indx(23)==16777214);
   assert(max_row_indx(24)==33554430);
   assert(max_row_indx(25)==67108862);
   assert(max_row_indx(26)==134217726);
   assert(max_row_indx(27)==268435454);
   assert(max_row_indx(28)==536870910);
   assert(max_row_indx(29)==1073741822);
   assert(max_row_indx(30)==-1); //invalid row returns -1
   
   //K_TO_ROW_LEN
   assert(k_to_row_len(0)==1); 
   assert(k_to_row_len(1)==2); 
   assert(k_to_row_len(2)==4); 
   assert(k_to_row_len(3)==8); 
   assert(k_to_row_len(4)==16); 
   assert(k_to_row_len(5)==32); 
   assert(k_to_row_len(6)==64); 
   assert(k_to_row_len(7)==128); 
   assert(k_to_row_len(8)==256); 
   assert(k_to_row_len(9)==512); 
   assert(k_to_row_len(10)==1024); 
   assert(k_to_row_len(11)==2048); 
   assert(k_to_row_len(12)==4096); 
   assert(k_to_row_len(13)==8192); 
   assert(k_to_row_len(14)==16384); 
   assert(k_to_row_len(15)==32768); 
   assert(k_to_row_len(16)==65536); 
   assert(k_to_row_len(17)==131072); 
   assert(k_to_row_len(18)==262144); 
   assert(k_to_row_len(19)==524288); 
   assert(k_to_row_len(20)==1048576); 
   assert(k_to_row_len(21)==2097152); 
   assert(k_to_row_len(22)==4194304); 
   assert(k_to_row_len(23)==8388608); 
   assert(k_to_row_len(24)==16777216); 
   assert(k_to_row_len(25)==33554432); 
   assert(k_to_row_len(26)==67108864); 
   assert(k_to_row_len(27)==134217728); 
   assert(k_to_row_len(28)==268435456); 
   assert(k_to_row_len(29)==536870912);

   //INDEX2ROW
   assert(indx_to_row(-1)==-1); //negative index returns -1
   assert(indx_to_row(1073741823)==-1); //index above max returns -1
   assert(indx_to_row(-1)==-1);
   assert(indx_to_row(0)==0); //exhaustive testing for first few rows
   assert(indx_to_row(1)==1);
   assert(indx_to_row(2)==1);
   assert(indx_to_row(3)==2);
   assert(indx_to_row(4)==2);
   assert(indx_to_row(5)==2);
   assert(indx_to_row(6)==2);
   assert(indx_to_row(7)==3);
   assert(indx_to_row(8)==3);
   assert(indx_to_row(9)==3);
   assert(indx_to_row(10)==3);
   assert(indx_to_row(11)==3);
   assert(indx_to_row(12)==3);
   assert(indx_to_row(13)==3);
   assert(indx_to_row(14)==3);
   assert(indx_to_row(15)==4); //first in row
   assert(indx_to_row(26)==4); //middle
   assert(indx_to_row(30)==4); //last
   assert(indx_to_row(31)==5); //first in row
   assert(indx_to_row(45)==5); //middle
   assert(indx_to_row(62)==5); //last
   assert(indx_to_row(63)==6); //first 
   assert(indx_to_row(90)==6); //middle
   assert(indx_to_row(126)==6); //last
   assert(indx_to_row(127)==7); //first 
   assert(indx_to_row(180)==7); //middle
   assert(indx_to_row(254)==7); //last
   assert(indx_to_row(255)==8); //first 
   assert(indx_to_row(400)==8); //middle
   assert(indx_to_row(510)==8); //last
   assert(indx_to_row(511)==9); //first 
   assert(indx_to_row(700)==9); //middle
   assert(indx_to_row(1022)==9); //last
   assert(indx_to_row(1023)==10); //first 
   assert(indx_to_row(2000)==10); //middle
   assert(indx_to_row(2046)==10); //last
   assert(indx_to_row(2047)==11); //first 
   assert(indx_to_row(3000)==11); //middle
   assert(indx_to_row(4094)==11); //last
   assert(indx_to_row(4095)==12); //first 
   assert(indx_to_row(6000)==12); //middle
   assert(indx_to_row(8190)==12); //last
   assert(indx_to_row(8191)==13); //first 
   assert(indx_to_row(10000)==13); //middle
   assert(indx_to_row(16382)==13); //last
   assert(indx_to_row(16383)==14); //first 
   assert(indx_to_row(18000)==14); //middle
   assert(indx_to_row(32766)==14); //last
   assert(indx_to_row(32767)==15); //first 
   assert(indx_to_row(34000)==15); //middle
   assert(indx_to_row(65534)==15); //last
   assert(indx_to_row(65535)==16); //first 
   assert(indx_to_row(80000)==16); //middle
   assert(indx_to_row(131070)==16); //last
   assert(indx_to_row(131071)==17); //first 
   assert(indx_to_row(150000)==17); //middle
   assert(indx_to_row(262142)==17); //last
   assert(indx_to_row(262143)==18); //first 
   assert(indx_to_row(300000)==18); //middle
   assert(indx_to_row(524286)==18); //last
   assert(indx_to_row(524287)==19); //first 
   assert(indx_to_row(624287)==19); //middle
   assert(indx_to_row(1048574)==19); //last
   assert(indx_to_row(1048575)==20); //first 
   assert(indx_to_row(1049999)==20); //middle
   assert(indx_to_row(2097150)==20); //last
   assert(indx_to_row(2097151)==21); //first 
   assert(indx_to_row(2099999)==21); //middle
   assert(indx_to_row(4194302)==21); //last
   assert(indx_to_row(4194303)==22); //first 
   assert(indx_to_row(4999999)==22); //middle
   assert(indx_to_row(8388606)==22); //last 
   assert(indx_to_row(8388607)==23); //first 
   assert(indx_to_row(8399999)==23); //middle
   assert(indx_to_row(16777214)==23); //last
   assert(indx_to_row(16777215)==24); //first 
   assert(indx_to_row(16787215)==24); //middle
   assert(indx_to_row(33554430)==24); //last
   assert(indx_to_row(33554431)==25); //first 
   assert(indx_to_row(33999999)==25); //middle
   assert(indx_to_row(67108862)==25); //last
   assert(indx_to_row(67108863)==26); //first 
   assert(indx_to_row(67109999)==26); //middle
   assert(indx_to_row(134217726)==26); //last
   assert(indx_to_row(134217727)==27); //first 
   assert(indx_to_row(134219999)==27); //middle
   assert(indx_to_row(268435454)==27); //last
   assert(indx_to_row(268435455)==28); //first 
   assert(indx_to_row(268999999)==28); //middle
   assert(indx_to_row(536870910)==28); //last
   assert(indx_to_row(536870911)==29); //first 
   assert(indx_to_row(536999999)==29); //middle
   assert(indx_to_row(1073741822)==29); //last
}


