#include "../bsa.h"
#include "specific.h"

//store size of each row in bsa struct? 
//watch out for segfaults when user passes in big numbers
//remember to swap out isfactorial.c etc files (I've commented stuff out)
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
   int row = indx2row(indx);
   int col = indx2col(indx);
   if(b->p[row]==NULL){
      row_alloc(b, row);
      b->p[row][col].n = d; //could this be rewritten to avoid duplicating insertion lines across if statements?
      b->p[row][col].set = true;
      return true; //need to test it's actually inserted before returning true?
   }
   if(b->p[row]!=NULL){
      b->p[row][col].n = d;
      b->p[row][col].set = true;
      return true;
   }
  return false; //when would you ever get to this case?
}



// Return pointer to data at element b[i]
   // or NULL if element is unset, or part of a row that hasn't been allocated.

int* bsa_get(bsa* b, int indx)
{   
   int row = indx2row(indx); 
   if(b->p[row]==NULL){
      return NULL; //or return a null pointer? 
   }
   if(b->p[row]!=NULL){
      int col = indx2col(indx);
      if(b->p[row][col].set == false){ 
         return NULL;
      }
      if(b->p[row][col].set == true){
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
   if(b->p[row][col].set == false){
      return false; 
   }
   if(b->p[row][col].set == true){
      b->p[row][col].set = false;
      if(is_row_empty(b,row)==true){
         free(b->p[row]); //free row pointer 
         b->p[row]=NULL;  //set pointer to NULL
      }
      return true; 
   }
  return false;    
}

int bsa_maxindex(bsa* b)
{  
   if(b == NULL){ //has this actually handled the test case in driver.c?
      return -1;
   }
   if(bsa_is_empty(b)){
      return -1;
   }
   if(!bsa_is_empty(b)){
      int row = top_live_row(b); //get highest allocated row
      int row_len = k2row_len(row);
      for(int col = row_len - 1; col >= 0; col--){
         if(b->p[row][col].set == true){
            int row_indx = col; 
            int max_indx = row_indx2indx(row, row_indx);
            return max_indx;
         }
      }   
   }
   return -1;
   //account for case where row is allocated without anything set in it (even though this shouldn't happen) 
}

// Returns stringified version of structure
  // Each row has its elements printed between {}, up to the maximum index.
  // Rows after the maximum index are ignored.

bool bsa_tostring(bsa* b, char* str) //trap if string is null and handle gracefully
{
   if(b == NULL){
      str = "\0";
      printf("%s", str);
      return false;
   }
  
   return false;
} //print out for null BSA is null string 


// Clears up all space used
bool bsa_free(bsa* b)  //if null BSA is passed to this funct, return false
{ 

 //step through 30 row pointers
   for(int row = 0; row<30; row++){
      if(b->p[row]!=NULL){
         free(b->p[row]);
         b->p[row] = NULL;
      }
   }
   if(bsa_is_empty(b)==true){
      free(b);
      return true;
   }
   if(bsa_is_empty(b)==false){
      return false; 
   }
   return false;
}

/*

void bsa_foreach(void (*func)(int* p, int* n), bsa* b, int* acc)
{
// Allow a user-defined function to be applied to each (valid) value 
// in the array. The user defined 'func' is passed a pointer to an int,
// and maintains an accumulator of the result where required.

}

*/

int indx2row(int indx)
{
   int shifted = 0;
   int n_shift = 0;
   indx ++; 

   while(shifted <= indx){
      shifted = 1<<n_shift; //1 == magic number
      n_shift++; 
   }
   return n_shift-2; //2 == magic number 
}

int max_row_indx(int row)
{  
   int n_shift = row+1; //1 == magic number
   int pow2 = 1<<n_shift;  //1 == magic number
   int max_indx = pow2 - 2; //2 == magic number 
   return max_indx;
}

int indx2col(int indx)
{
   int row = indx2row(indx);
   int prv_rw_offst = max_row_indx(row-1); // 1 == magic num, need max index prev row
   int col = indx-prv_rw_offst-1; // 1 == magic num
   return col;
}

int row_indx2indx(int row, int row_indx)
{  
   if(row > 0 && row < 30){
      int prv_rw = row - 1; //magic number
      int prv_mx_rw_indx = max_row_indx(prv_rw);
      int abs_indx = prv_mx_rw_indx + row_indx + 1; //magic number
      return abs_indx;
   }
   if(row == 0){
      return 0;
   }
   return 1;
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
   for(int i = 0; i<30; i++){
      if(b->p[i]==NULL){
         cnt++;
      } 
   }
   //printf("null cnt %i", cnt);
   if(cnt == 30){
      return true;
   }
   else{
      return false;
   }
}

int top_live_row(bsa* b)
{
   for(int k=29; k>-1; k--){
      if(b->p[k]!=NULL){
         return k;
      } 
   }
   return 0; //does this overlap with case where only 0 is allocated? 
}

bool is_row_empty(bsa* b, int row)
{
   int cnt = 0;
   int row_len = k2row_len(row);
   for(int col = row_len - 1; col > -1; col--){ //magic num
      if(b->p[row][col].set == true){
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


void test(void)
{
  
   //think about edge test cases esp for functions in driver.c 
   
   //BSA_INIT
   bsa* b = bsa_init();
   assert(b);
   //check that 30 pointers in empty BSA are null
   for(int i = 0; i<30; i++){
      assert(b->p[i]==NULL);
   }
   free(b);

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
   free(f->p[0]); //this is a hacky way of freeing block allocated by row_alloc - find a better way
   free(f);

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
   free(g->p[2]);
   free(g);

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
   free(h->p[17]);
   free(h);
   

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
   free(c->p[0]);
   free(c);
   
   
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
   free(d->p[0]); //create a function to free stuff allocated by row_alloc in test funct
   free(d->p[5]);
   free(d->p[17]);
   free(d->p[29]);
   free(d);

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
   free(e->p[3]);
   free(e);
   
   //remember to free everything in this test function 
   
}


