#include "../bsa.h"
#include "specific.h"

//store size of each row in bsa struct? 

bsa* bsa_init(void)
{
   bsa* b = (bsa*) calloc(1, sizeof(bsa)); //calloc sets pointers to NULL i.e. pointer to zero is null pointer 
   return b;
} 


/*
bool bsa_set(bsa* b, int indx, int d)
{
   // Set element at index indx with value d i.e. b[i] = d;
   // May require an allocation if it's the first element in that row
       //i.e. if pointer to that row is null
}


int* bsa_get(bsa* b, int indx)
{
   // Return pointer to data at element b[i]
   // or NULL if element is unset, or part of a row that hasn't been allocated.
}


bool bsa_delete(bsa* b, int indx)
{
   // Delete element at index indx - forces a shrink
   // if that was the only cell in the row occupied.
}


int bsa_maxindex(bsa* b)
{
   // Returns maximum index written to so far or
   // -1 if no cells have been written to yet
}

bool bsa_tostring(bsa* b, char* str)
{
  // Returns stringified version of structure
  // Each row has its elements printed between {}, up to the maximum index.
  // Rows after the maximum index are ignored.

}


bool bsa_free(bsa* b)
{
   // Clears up all space used

}


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

//function to get length of row from row number 
int n2row_len(int n)
{
   int len = 1<<n;
   return len;  
}

//function to allocate row, called in bsa_set
void row_alloc(bsa* b, int row)
{ 
   int row_len = n2row_len(row);
   b->p[row] = (col_cell*) calloc(row_len, sizeof(col_cell));
}


void test(void)
{
   //BSA_INIT
   bsa* b = bsa_init();
   assert(b);
   //check that 30 pointers in empty BSA are null
   for(int i = 0; i<30; i++){
      assert(b->p[i]==NULL);
   }
   
   //ROW_ALLOC

   //allocate row 0
   row_alloc(b, 0); 
   int len = n2row_len(0); //length is 1
   assert(b->p[0]!=NULL); //check row 0 pointer is pointing to array
   for(int c = 0; c < len; c++){
      assert(b->p[0][c].n == 0); //check calloc has flood-filled with zeros
      assert(b->p[0][c].set == 0);
   }

   //allocate row 2
   row_alloc(b, 2); 
   len = n2row_len(2); //length is 4
   assert(b->p[2]!=NULL); //check row 2 pointer is pointing to array
   for(int c = 0; c < len; c++){
      assert(b->p[2][c].n == 0);
      assert(b->p[2][c].set == 0);
   }

   //allocate row 17
   row_alloc(b, 17); 
   len = n2row_len(17); //length is 131072
   assert(b->p[17]!=NULL); //check row 2 pointer is pointing to array
   for(int c = 0; c < len; c++){
      assert(b->p[17][c].n == 0);
      assert(b->p[17][c].set == 0);
   }

   //N2ROW_LEN

   assert(n2row_len(0)==1); 
   assert(n2row_len(1)==2); 
   assert(n2row_len(2)==4); 
   assert(n2row_len(3)==8); 
   assert(n2row_len(4)==16); 
   assert(n2row_len(5)==32); 
   assert(n2row_len(6)==64); 
   assert(n2row_len(7)==128); 
   assert(n2row_len(8)==256); 
   assert(n2row_len(9)==512); 
   assert(n2row_len(10)==1024); 
   assert(n2row_len(11)==2048); 
   assert(n2row_len(12)==4096); 
   assert(n2row_len(13)==8192); 
   assert(n2row_len(14)==16384); 
   assert(n2row_len(15)==32768); 
   assert(n2row_len(16)==65536); 
   assert(n2row_len(17)==131072); 
   assert(n2row_len(18)==262144); 
   assert(n2row_len(19)==524288); 
   assert(n2row_len(20)==1048576); 
   assert(n2row_len(21)==2097152); 
   assert(n2row_len(22)==4194304); 
   assert(n2row_len(23)==8388608); 
   assert(n2row_len(24)==16777216); 
   assert(n2row_len(25)==33554432); 
   assert(n2row_len(26)==67108864); 
   assert(n2row_len(27)==134217728); 
   assert(n2row_len(28)==268435456); 
   assert(n2row_len(29)==536870912);

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
   
}


