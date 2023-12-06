#include "../bsa.h"
#include "specific.h"

//watch out for segfaults when user passes in big numbers
//handle edge cases e.g. 
   //check index is between zero and max index (hash define max)
   //check row between 0 and 29 inclusive

bsa* bsa_init(void)
{
   bsa* b = (bsa*) calloc(1, sizeof(bsa)); //calloc sets pointers to NULL i.e. pointer to zero is null pointer 
   return b;
} //if calloc fails, exit or return null pointer

// Set element at index indx with value d i.e. b[i] = d;
   // May require an allocation if it's the first element in that row
bool bsa_set(bsa* b, int indx, int d)
{  
   if(b == NULL){
      return false;
   }
   int row = indx2row(indx);
   int col = indx2col(indx);
   if(row_is_null(b, row)){
      row_alloc(b, row);
      data_inserted(b, row, col, d);
      return true;
   }
   if(!row_is_null(b, row)){
      data_inserted(b, row, col, d);
      return true;
   }
  return false;
}

// Return pointer to data at element b[i]
   // or NULL if element is unset, or part of a row that hasn't been allocated.

int* bsa_get(bsa* b, int indx)
{   
   int row = indx2row(indx); 
   if(row_is_null(b, row)){
      return NULL;
   }
   if(!row_is_null(b, row)){
      int col = indx2col(indx);
      if(!cell_is_set(b, row, col)){ 
         return NULL;
      }
      if(cell_is_set(b, row, col)){
         return &b->p[row][col].n; 
      }
   } 
   return NULL;   
}

// Delete element at index indx - forces a shrink
   // if that was the only cell in the row occupied.
bool bsa_delete(bsa* b, int indx)
{  
   int row = indx2row(indx); 
   int col = indx2col(indx);
   //if(!indx_is_valid(indx)){
   //   return false;
  // }
   if(!cell_is_set(b, row, col)){
      return false; 
   }
   if(cell_is_set(b, row, col)){
      b->p[row][col].set = false;
      if(is_row_empty(b,row)){
         free(b->p[row]); //free row pointer 
         b->p[row]=NULL;  //set pointer to NULL
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
      int row = top_live_row(b); //get highest allocated row
      int max_row_indx = maxrowindx(b, row);
      int max_indx = row_indx2indx(row, max_row_indx);
      return max_indx;
   }
   return -1;
}

// Returns stringified version of structure
  // Each row has its elements printed between {}, up to the maximum index.
  // Rows after the maximum index are ignored.

bool bsa_tostring(bsa* b, char* str) //do I need logic to exit gracefully if size of str passed in is bigger than 1000? - ASK in lab
{  
   if(str == NULL){
      return false;
   }
   char tmp[TMPSTRLEN] = {'\0'};  
   strcpy(str, ""); //clear garbage from str passed in
   int max_indx = bsa_maxindex(b);
   int max_indx_rw = indx2row(max_indx);
   if(b == NULL){
      return false;
   }
   for(int row = 0; row <= max_indx_rw; row++){ 
      if(row_is_null(b, row)){
         sprintf(tmp, "%c%c", OPN_CUR_BR, CLS_CUR_BR); 
         strcat(str, tmp);
      } 
      if(!row_is_null(b, row)){
         vald_rw_2str(b, str, tmp, row);
      }
   }
   return true;
} 
//the print out for null BSA is null string - have I done this?


// Clears up all space used
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

// Allow a user-defined function to be applied to each (valid) value 
// in the array. The user defined 'func' is passed a pointer to an int,
// and maintains an accumulator of the result where required.
void bsa_foreach(void (*func)(int* p, int* n), bsa* b, int* acc) //can I reduce the nesting in this function? TO DO
{  
   for(int row = 0; row < BSA_ROWS; row++){ //remove one level of nesting: from 3 to 4, even if it runs a bits slower
      if(!row_is_null(b, row)){  
         int row_len = k2row_len(row);
         for(int col = 0; col < row_len; col++){ 
            if(cell_is_set(b, row, col)){
               int* ptr = &b->p[row][col].n;
               func(ptr, acc);
            }  
         }
      }       
   }   
}


//incorporate this function into bsa_maxindex (currently, they overlap)
int maxrowindx(bsa* b, int row)
{   
    //if(!k_is_valid(row)){
    //   return -1;
   // }//test for this case
      int row_len = k2row_len(row);
      for(int col = row_len - 1; col >= 0; col--){
         if(cell_is_set(b, row, col)){
            int row_indx = col; 
            return row_indx;
         }
      }   
   return 0;
}

void vald_rw_2str(bsa* b, char* str, char* tmp, int row)
{
   int max_set_cell = maxrowindx(b, row);
   sprintf(tmp, "%c", OPN_CUR_BR); 
   strcat(str, tmp);
   int row_len = k2row_len(row);
   for(int col = 0 ; col < row_len ; col++){
      if(cell_is_set(b, row, col)){ 
         int d = b->p[row][col].n;
         int indx = row_indx2indx(row, col);
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

int indx2row(int indx)
{  
   //if(!indx_is_valid(indx)){
   //   return -1; //test for this case
   //}
   int shifted = 0;
   int n_shift = 0;
   indx ++; 
   while(shifted <= indx){
      shifted = 1<<n_shift; 
      n_shift++; 
   }
   return n_shift-SHIFT_OFFSET; 
}

int max_row_indx(int row)
{  
   //if(!k_is_valid(row)){
   //    return -1;
   //}//test for this case
   int n_shift = row+1; 
   int pow2 = 1<<n_shift;  
   int max_indx = pow2 - SHIFT_OFFSET; 
   return max_indx;
}

int indx2col(int indx)
{  
   if(!indx_is_valid(indx)){
      return -1; //test for this case
   }
   int row = indx2row(indx);
   int prv_rw_offst = max_row_indx(row-1); 
   int col = indx-prv_rw_offst-1; 
   return col;
}

int row_indx2indx(int row, int row_indx)
{  
   if(row > 0 && row < BSA_ROWS){ 
      int prv_rw = row - 1; 
      int prv_mx_rw_indx = max_row_indx(prv_rw);
      int abs_indx = prv_mx_rw_indx + row_indx + 1; 
      return abs_indx;
   }
   if(row == 0){
      return 0;
   }
   //if(!k_is_valid(row)){
   //    return -1;
   // }//test for this case
   return -1;
}

//function to get length of row from row number 
int k2row_len(int k)
{  
   int len = 1<<k;
   return len;  
}

//function to allocate row, called in bsa_set
void row_alloc(bsa* b, int row)
{ 
   int row_len = k2row_len(row);
   b->p[row] = (col_cell*) calloc(row_len, sizeof(col_cell));
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
   for(int row=BSA_ROWS - 1; row > - 1; row--){  
      if(!row_is_null(b, row)){
         return row;
      } 
   }
   return 0; //does this overlap with case where only 0 is allocated? 
}


bool is_row_empty(bsa* b, int row)
{
   int cnt = 0;
   int row_len = k2row_len(row);
   for(int col = row_len - 1; col > -1; col--){ //magic num
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
   //if(!k_is_valid(row)){
   //    return false;
   //}//test for this case
   if(b == NULL){
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

void test(void)
{
   //remember to free everything in this test function 
   //think about edge test cases esp for functions in driver.c 
   
   //BSA_INIT
   bsa* z = bsa_init(); //do I need to keep changing letters of BSA like this?
   assert(z);
   //check that 30 pointers in empty BSA are null
   for(int i = 0; i < BSA_ROWS; i++){
      assert(z->p[i]==NULL);
   }
   bsa_free(z);

   //ROW_ALLOC
   
   //allocate row 0
   bsa* f = bsa_init();
   assert(f);
   row_alloc(f, 0); 
   int len = k2row_len(0); //length is 1
   assert(f->p[0]!=NULL); //check row 0 pointer is pointing to array
   for(int c = 0; c < len; c++){
      assert(f->p[0][c].n == 0); //check calloc has flood-filled with zeros
      assert(f->p[0][c].set == 0);
   }
   bsa_free(f);

   //allocate row 2
   bsa* g = bsa_init();
   assert(g);
   row_alloc(g, 2); 
   len = k2row_len(2); //length is 4
   assert(g->p[2]!=NULL); //check row 2 pointer is pointing to array
   for(int c = 0; c < len; c++){
      assert(g->p[2][c].n == 0);
      assert(g->p[2][c].set == 0);
   }
   bsa_free(g);

   //allocate row 17
   bsa* h = bsa_init();
   assert(h);
   row_alloc(h, 17); 
   len = k2row_len(17); //length is 131072
   assert(h->p[17]!=NULL); //check row 2 pointer is pointing to array
   for(int c = 0; c < len; c++){
      assert(h->p[17][c].n == 0);
      assert(h->p[17][c].set == 0);
   }
   bsa_free(h);
   

   //N2ROW_LEN

   assert(k2row_len(0)==1); 
   assert(k2row_len(1)==2); 
   assert(k2row_len(2)==4); 
   assert(k2row_len(3)==8); 
   assert(k2row_len(4)==16); 
   assert(k2row_len(5)==32); 
   assert(k2row_len(6)==64); 
   assert(k2row_len(7)==128); 
   assert(k2row_len(8)==256); 
   assert(k2row_len(9)==512); 
   assert(k2row_len(10)==1024); 
   assert(k2row_len(11)==2048); 
   assert(k2row_len(12)==4096); 
   assert(k2row_len(13)==8192); 
   assert(k2row_len(14)==16384); 
   assert(k2row_len(15)==32768); 
   assert(k2row_len(16)==65536); 
   assert(k2row_len(17)==131072); 
   assert(k2row_len(18)==262144); 
   assert(k2row_len(19)==524288); 
   assert(k2row_len(20)==1048576); 
   assert(k2row_len(21)==2097152); 
   assert(k2row_len(22)==4194304); 
   assert(k2row_len(23)==8388608); 
   assert(k2row_len(24)==16777216); 
   assert(k2row_len(25)==33554432); 
   assert(k2row_len(26)==67108864); 
   assert(k2row_len(27)==134217728); 
   assert(k2row_len(28)==268435456); 
   assert(k2row_len(29)==536870912);

   //INDEX2ROW
 
   //what about negative index? - define what this function does, and have function to ensure it's positive? 
   assert(indx2row(0)==0); //exhaustive testing for first few rows
   assert(indx2row(1)==1);
   assert(indx2row(2)==1);
   assert(indx2row(3)==2);
   assert(indx2row(4)==2);
   assert(indx2row(5)==2);
   assert(indx2row(6)==2);
   assert(indx2row(7)==3);
   assert(indx2row(8)==3);
   assert(indx2row(9)==3);
   assert(indx2row(10)==3);
   assert(indx2row(11)==3);
   assert(indx2row(12)==3);
   assert(indx2row(13)==3);
   assert(indx2row(14)==3);
   assert(indx2row(15)==4); //first in row
   assert(indx2row(26)==4); //middle
   assert(indx2row(30)==4); //last
   assert(indx2row(31)==5); //first in row
   assert(indx2row(45)==5); //middle
   assert(indx2row(62)==5); //last
   assert(indx2row(63)==6); //first 
   assert(indx2row(90)==6); //middle
   assert(indx2row(126)==6); //last
   assert(indx2row(127)==7); //first 
   assert(indx2row(180)==7); //middle
   assert(indx2row(254)==7); //last
   assert(indx2row(255)==8); //first 
   //KEEP GOING UP TO 29 .............................
   
   //MAX_ROW_INDX
   
   assert(max_row_indx(0)==0);
   assert(max_row_indx(1)==2);
   assert(max_row_indx(2)==6);
   assert(max_row_indx(3)==14);
   assert(max_row_indx(4)==30);
   assert(max_row_indx(5)==62);
   assert(max_row_indx(6)==126);
   assert(max_row_indx(7)==254);
   //KEEP GOING UP TO 29 .............................

   //INDX2COL 
   
   assert(indx2col(0)==0);
   assert(indx2col(1)==0);
   assert(indx2col(2)==1);
   assert(indx2col(3)==0);
   assert(indx2col(4)==1);
   assert(indx2col(5)==2);
   assert(indx2col(6)==3);
   assert(indx2col(7)==0);
   assert(indx2col(8)==1);
   assert(indx2col(9)==2);
   assert(indx2col(10)==3);
   assert(indx2col(11)==4);
   assert(indx2col(12)==5);
   assert(indx2col(13)==6);
   assert(indx2col(14)==7);
   assert(indx2col(15)==0); //first indx in row 4
   assert(indx2col(30)==15); //last indx in row 4
   assert(indx2col(31)==0); //first indx in row 5
   assert(indx2col(62)==31); //last indx in row 5
   assert(indx2col(63)==0); //first indx in row 6
   assert(indx2col(126)==63); //last indx in row 6
   assert(indx2col(127)==0);//first indx in row 7
   assert(indx2col(254)==127);//first indx in row 7
   
   //KEEP GOING UP TO 29 .............................
   //and add middle index tests above

   //BSA_IS_EMPTY
   
   bsa* c = bsa_init(); //empty BSA
   assert(c);
   assert(bsa_is_empty(c)==true);
   row_alloc(c, 0); //allocate one row: BSA not empty
   assert(bsa_is_empty(c)==false); 
   bsa_free(c);
   
   
   //TOP_LIVE_ROW
   bsa* d = bsa_init();
   row_alloc(d, 0); //allocate row 0
   assert(top_live_row(d)==0);
   row_alloc(d, 5); //allocate row 5
   assert(top_live_row(d)==5);
   row_alloc(d, 17); //allocate row 17
   assert(top_live_row(d)==17);
   row_alloc(d, 29); //allocate row 29
   assert(top_live_row(d)==29);
   //add more testing
   bsa_free(d);

   //ROW_INDX2INDX
   assert(row_indx2indx(0, 0)==0);
   assert(row_indx2indx(2, 0)==3);
   assert(row_indx2indx(2, 1)==4);
   assert(row_indx2indx(2, 3)==6);
   assert(row_indx2indx(3, 4)==11);
   //add more testing

   //IS_ROW_EMPTY
   bsa* e = bsa_init();
   row_alloc(e, 3); //allocate row 3
   assert(is_row_empty(e, 3)==true);
   bsa_set(e, 8, 10);
   assert(is_row_empty(e, 3)==false);
   bsa_free(e);

   //BSA_TOSTRING
   
   char tst[TSTSTRLEN];
   
   //empty BSA, no rows set i.e. BSA max index == -1
   bsa* i = bsa_init();   
   bsa_tostring(i, tst);
   assert(strcmp(tst, "")==0);
   free(i);
   strcpy(tst, "");

   //one set element & stop printing rows after max index 
   bsa* j = bsa_init();
   bsa_set(j, 4, 9); //set "9" at index 4, row 2
   bsa_tostring(j, tst);
   assert(strcmp(tst, "{}{}{[4]=9}")==0); 
   bsa_free(j);
   strcpy(tst, "");
  
   //two elements set as only element in two rows
        //maybe some redundant stuff in this test
   bsa* k = bsa_init();
   assert(k);
   assert(bsa_maxindex(k)==-1);
   assert(bsa_set(k, 0, 4));
   assert(bsa_maxindex(k)==0);
   assert(bsa_set(k, 0, 0));
   assert(bsa_set(k, 15, 15));
   assert(bsa_tostring(k, tst));
   assert(strcmp(tst, "{[0]=0}{}{}{}{[15]=15}")==0);
   bsa_free(k);
   strcpy(tst, "");
            
   //two set elements same row
   bsa* m = bsa_init();
   bsa_set(m, 1, 1); //set "1" at index 1, row 1
   bsa_set(m, 2, 2); //set "2" at index 2, row 1
   bsa_set(m, 3, 3); //set "3" at index 3, row 2
   bsa_tostring(m, tst);
   assert(strcmp(tst, "{}{[1]=1 [2]=2}{[3]=3}")==0);
   bsa_free(m);
   strcpy(tst, "");

   //three elements same row
   bsa* n = bsa_init();
   bsa_set(n, 3, 9); //set "9" at index 3, row 2
   bsa_set(n, 4, 10); //set "10" at index 4, row 2
   bsa_set(n, 5, 11); //set "11" at index 5, row 2
   bsa_tostring(n, tst);
   assert(strcmp(tst, "{}{}{[3]=9 [4]=10 [5]=11}")==0);
   bsa_free(n);
   strcpy(tst, "");
 
   //two set elements  per row on two different rows
   bsa* o = bsa_init();
   bsa_set(o, 1, 5); //set "1" at index 1, row 1
   bsa_set(o, 2, 6); //set "2" at index 2, row 1
   bsa_set(o, 3, 7); //set "3" at index 3, row 2
   bsa_set(o, 4, 8); //set "3" at index 3, row 2
   bsa_tostring(o, tst);
   assert(strcmp(tst, "{}{[1]=5 [2]=6}{[3]=7 [4]=8}")==0);
   bsa_free(o);
   strcpy(tst, "");

   //MAX_ROW_INDX
   bsa* l = bsa_init();
   assert(l);
   bsa_set(l, 8, 4); //set index 8 to 4
   bsa_set(l, 10, 5); //set index 10 to 5
   assert(maxrowindx(l, 3)==3); // max set index in row 3 is row index 3 i.e. abs index 10
   bsa_set(l, 13, 7); //set index 13 to 6
   assert(maxrowindx(l, 3)==6); // max set index in row 3 is row index 6 i.e. abs index 13
   bsa_set(l, 14, 12); //set index 14 to 12
   assert(maxrowindx(l, 3)==7); // max set index in row 3 is row index 7 i.e. abs index 14
   bsa_free(l);
  /*
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
   */

   //shouldn't be able to set or get after freeing BSA

}


