#include "../bsa.h"
#include "specific.h"
//DOUBLE CHECK WHAT FILES WE INCLUDE IN .zip
//minimise use of bounds checking function
//make naming consistent
bsa* bsa_init(void)
{
   bsa* b = (bsa*) calloc(1, sizeof(bsa)); //calloc sets pointers to NULL i.e. pointer to zero is null pointer 
   return b;
} //if calloc fails, exit or return null pointer (do I need to test calloc? - ASK)

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

bool bsa_delete(bsa* b, int indx)
{  
   int row = indx2row(indx); 
   int col = indx2col(indx);
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
         b->p[row]=NULL;  
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
      int max_indx = row_indx2indx(row, max_row_indx);
      return max_indx;
   }
   return -1;
}

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

int max_set_row_indx(bsa* b, int row)
{   
    if(!k_is_valid(row)){
       return -1;
    }
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
   int max_set_cell = max_set_row_indx(b, row);
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
   if(!indx_is_valid(indx)){
      return -1; 
   }
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
   if(!k_is_valid(row)){
       return -1;
   }
   int n_shift = row+1; 
   int pow2 = 1<<n_shift;  
   int max_indx = pow2 - SHIFT_OFFSET; 
   return max_indx;
}

int indx2col(int indx)
{  
   if(!indx_is_valid(indx)){
      return -1; 
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
   if(!k_is_valid(row)){
       return -1;
    }
   return -1;
}

int k2row_len(int k)
{  
   int len = 1<<k;
   return len;  
}

void row_alloc(bsa* b, int row)
{ 
   int row_len = k2row_len(row);
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
   for(int row=BSA_ROWS - 1; row > - 1; row--){  
      if(!row_is_null(b, row)){
         return row;
      } 
   }
   return -1; 
}

bool is_row_empty(bsa* b, int row)
{
   int cnt = 0;
   int row_len = k2row_len(row);
   for(int col = row_len - 1; col > -1; col--){ 
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
   }//test for this case
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
   //BSA_INIT
   bsa* z = bsa_init();
   assert(z);
   //check that 30 pointers in empty BSA are null
   for(int i = 0; i < BSA_ROWS; i++){
      assert(row_is_null(z, i));
   }
   bsa_free(z);
   
   //BSA_SET

   //set big int 
   bsa* u = bsa_init();
   assert(bsa_set(u, 5, 1234567890)); //set integer 10 digits long
   assert(bsa_maxindex(u)==5);
   
   //reset existing big value 
   assert(bsa_set(u, 5, 1234567891));
   
   //check value has been set
   int* a = bsa_get(u, 5);
   assert(*a == 1234567891);
   bsa_free(u);

   //try to set NULL BSA
   bsa* v = NULL;
   assert(!bsa_set(v, 5, 12)); //returns false when try set NULL BSA
   
   //DATA_INSERTED
   
   //returns false when try insert data into NULL BSA
   assert(!data_inserted(v, 2, 2, 1234567890));
   bsa_free(v);

   bsa* w = bsa_init();
   assert(w);

   //returns false when try insert data into NULL row 
   assert(!data_inserted(w, 3, 2, 1234567891));
   
   row_alloc(w, 2); //allocate row 2
 
   //returns false when try insert in invalid row
   assert(!data_inserted(w, -1, 2, 1234567891));
 
   //returns true when insert data in valid row 2
   assert(data_inserted(w, 2, 2, 1234599999));
   
   //check value has been set
   a = bsa_get(w, 5);
   assert(*a == 1234599999);

   //CELL_IS_SET
   
   //check value has been set 
   assert(cell_is_set(w, 2, 2));
   //returns false for unset ccell 
   assert(!cell_is_set(w, 2, 3));
   
   bsa_free(w);

   //BSA_GET
    
   bsa* x = bsa_init();
   //set a big value 
   assert(bsa_set(x, 11, 1111111111));

   //get big value
   a = bsa_get(x, 11);
   assert(*a == 1111111111);

   //set big value last cell of BSA row 29
   assert(bsa_set(x, 1073741822, 1111111112));

   //get big value last cell of BSA row 29
   a = bsa_get(x, 1073741822);
   assert(*a == 1111111112);
   
   bsa_free(x);
    
   //BSA_DELETE
   bsa* y = bsa_init();
   bsa_set(y, 11, 9); //allocate row 3 and set index 11 to "9"
   
   //try delete at invalid index
   assert(!bsa_delete(y, -1)); //invalid index low 
   assert(!bsa_delete(y, 1073741823)); //invalid index high
  
   //try delete unset cell
   assert(!bsa_delete(y, 12));
   
   //delete set cell
   bsa_set(y, 1073741822, 999000); //set last cell in row 29
   assert(bsa_maxindex(y)==1073741822); 
   assert(bsa_delete(y, 1073741822)); //delete last cell row 29
   assert(bsa_maxindex(y)==11); 
   bsa_free(y);

   //BSA_MAX_INDEX
   bsa* aa = NULL;
   assert(bsa_maxindex(aa)==-1); //returns -1 for NULL BSA
   aa = bsa_init();
   assert(bsa_maxindex(aa)==-1); //returns -1 for empty BSA
   bsa_set(aa, 0, 999000);
   assert(bsa_maxindex(aa)==0);
   bsa_set(aa, 5, 999000);
   assert(bsa_maxindex(aa)==5);
   bsa_set(aa, 17899, 999000);
   assert(bsa_maxindex(aa)==17899);
   bsa_set(aa, 1073741822, 999000);
   assert(bsa_maxindex(aa)==1073741822);
   bsa_free(aa);

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
   bsa* k = bsa_init();
   assert(k);
   assert(bsa_set(k, 0, 4));
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
   bsa_set(o, 4, 8); //set "3" at index 4, row 2
   bsa_tostring(o, tst);
   assert(strcmp(tst, "{}{[1]=5 [2]=6}{[3]=7 [4]=8}")==0);
   bsa_free(o);
   strcpy(tst, "");

   //two set elements  per row on two different rows big d values
   bsa* ab = bsa_init();
   bsa_set(ab, 1, 1234567891); //set "1" at index 1, row 1
   bsa_set(ab, 2, 1234567892); //set "2" at index 2, row 1
   bsa_set(ab, 3, 1234567893); //set "3" at index 3, row 2
   bsa_set(ab, 4, 1234567894); //set "3" at index 4, row 2
   bsa_tostring(ab, tst);
   assert(strcmp(tst, "{}{[1]=1234567891 [2]=1234567892}{[3]=1234567893 [4]=1234567894}")==0);
   bsa_free(ab);
   strcpy(tst, "");

   //three elements same row big d values
   bsa* ac = bsa_init();
   bsa_set(ac, 3, 1234567891); //set "9" at index 3, row 2
   bsa_set(ac, 4, 1234567892); //set "10" at index 4, row 2
   bsa_set(ac, 5, 1234567893); //set "11" at index 5, row 2
   bsa_tostring(ac, tst);
   assert(strcmp(tst, "{}{}{[3]=1234567891 [4]=1234567892 [5]=1234567893}")==0);
   bsa_free(ac);
   strcpy(tst, "");
   
   //VALD_RW_2STR
   char tmp[TMPSTRLEN] = {'\0'};  

   bsa* ad = bsa_init();
   bsa_set(ad, 3, 1234567891); //set "9" at index 3, row 2
   //print row with 1 set element 
   vald_rw_2str(ad, tst, tmp, 2); 
   assert(strcmp(tst, "{[3]=1234567891}")==0);
   strcpy(tst, "");
   bsa_set(ad, 4, 1234567892); //set "10" at index 4, row 2
   //print row with 2 set elements 
   vald_rw_2str(ad, tst, tmp, 2); 
   assert(strcmp(tst, "{[3]=1234567891 [4]=1234567892}")==0);
   strcpy(tst, "");
   bsa_free(ad);

   //BSA_FOREACH & ADD

   bsa* ae = bsa_init();
   bsa_set(ae, 1, 1);
   bsa_set(ae, 2, 2);
   bsa_set(ae, 3, 4);
   int acc = 0;
   bsa_foreach(add, ae, &acc);
   assert(acc==7);
   bsa_free(ae);

   //ROW_ALLOC
   
   //allocate row 0
   bsa* f = bsa_init();
   assert(f);
   row_alloc(f, 0); 
   int len = k2row_len(0); //length is 1
   assert(!row_is_null(f, 0)); //check row 0 pointer is pointing to array
   for(int col = 0; col < len; col++){
      assert(f->p[0][col].n == 0); //check calloc has flood-filled with zeros
      assert(f->p[0][col].set == 0);
   }
   bsa_free(f);
 
   //allocate row 1
   bsa* q = bsa_init();
   assert(q);
   row_alloc(q, 1); 
   len = k2row_len(1); //length is 2
   assert(!row_is_null(q, 1)); //check row 1 pointer is pointing to array
   for(int c = 0; c < len; c++){
      assert(q->p[1][c].n == 0);
      assert(q->p[1][c].set == 0);
   }
   bsa_free(q);

   //allocate row 2
   bsa* g = bsa_init();
   assert(g);
   row_alloc(g, 2); 
   len = k2row_len(2); //length is 4
   assert(!row_is_null(g, 2)); //check row 2 pointer is pointing to array
   for(int c = 0; c < len; c++){
      assert(g->p[2][c].n == 0);
      assert(g->p[2][c].set == 0);
   }
   bsa_free(g);

   //allocate row 10
   bsa* r = bsa_init();
   assert(r);
   row_alloc(r, 10); 
   len = k2row_len(10); 
   assert(!row_is_null(r, 10)); 
   for(int c = 0; c < len; c++){
      assert(r->p[10][c].n == 0);
      assert(r->p[10][c].set == 0);
   }
   bsa_free(r);

   //allocate row 17
   bsa* h = bsa_init();
   assert(h);
   row_alloc(h, 17); 
   len = k2row_len(17); //length is 131072
   assert(!row_is_null(h, 17)); //check row 2 pointer is pointing to array
   for(int c = 0; c < len; c++){
      assert(h->p[17][c].n == 0);
      assert(h->p[17][c].set == 0);
   }
   bsa_free(h);

   //allocate row 29
   bsa* s = bsa_init();
   assert(s);
   row_alloc(s, 29); 
   len = k2row_len(29); 
   assert(!row_is_null(s, 29)); 
   for(int c = 0; c < len; c++){
      assert(s->p[29][c].n == 0);
      assert(s->p[29][c].set == 0);
   }
   bsa_free(s);
   

   //K2ROW_LEN
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
   assert(indx2row(-1)==-1); //negative index returns -1
   assert(indx2row(1073741823)==-1); //index above max returns -1
   assert(indx2row(-1)==-1);
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
   assert(indx2row(400)==8); //middle
   assert(indx2row(510)==8); //last
   assert(indx2row(511)==9); //first 
   assert(indx2row(700)==9); //middle
   assert(indx2row(1022)==9); //last
   assert(indx2row(1023)==10); //first 
   assert(indx2row(2000)==10); //middle
   assert(indx2row(2046)==10); //last
   assert(indx2row(2047)==11); //first 
   assert(indx2row(3000)==11); //middle
   assert(indx2row(4094)==11); //last
   assert(indx2row(4095)==12); //first 
   assert(indx2row(6000)==12); //middle
   assert(indx2row(8190)==12); //last
   assert(indx2row(8191)==13); //first 
   assert(indx2row(10000)==13); //middle
   assert(indx2row(16382)==13); //last
   assert(indx2row(16383)==14); //first 
   assert(indx2row(18000)==14); //middle
   assert(indx2row(32766)==14); //last
   assert(indx2row(32767)==15); //first 
   assert(indx2row(34000)==15); //middle
   assert(indx2row(65534)==15); //last
   assert(indx2row(65535)==16); //first 
   assert(indx2row(80000)==16); //middle
   assert(indx2row(131070)==16); //last
   assert(indx2row(131071)==17); //first 
   assert(indx2row(150000)==17); //middle
   assert(indx2row(262142)==17); //last
   assert(indx2row(262143)==18); //first 
   assert(indx2row(300000)==18); //middle
   assert(indx2row(524286)==18); //last
   assert(indx2row(524287)==19); //first 
   assert(indx2row(624287)==19); //middle
   assert(indx2row(1048574)==19); //last
   assert(indx2row(1048575)==20); //first 
   assert(indx2row(1049999)==20); //middle
   assert(indx2row(2097150)==20); //last
   assert(indx2row(2097151)==21); //first 
   assert(indx2row(2099999)==21); //middle
   assert(indx2row(4194302)==21); //last
   assert(indx2row(4194303)==22); //first 
   assert(indx2row(4999999)==22); //middle
   assert(indx2row(8388606)==22); //last 
   assert(indx2row(8388607)==23); //first 
   assert(indx2row(8399999)==23); //middle
   assert(indx2row(16777214)==23); //last
   assert(indx2row(16777215)==24); //first 
   assert(indx2row(16787215)==24); //middle
   assert(indx2row(33554430)==24); //last
   assert(indx2row(33554431)==25); //first 
   assert(indx2row(33999999)==25); //middle
   assert(indx2row(67108862)==25); //last
   assert(indx2row(67108863)==26); //first 
   assert(indx2row(67109999)==26); //middle
   assert(indx2row(134217726)==26); //last
   assert(indx2row(134217727)==27); //first 
   assert(indx2row(134219999)==27); //middle
   assert(indx2row(268435454)==27); //last
   assert(indx2row(268435455)==28); //first 
   assert(indx2row(268999999)==28); //middle
   assert(indx2row(536870910)==28); //last
   assert(indx2row(536870911)==29); //first 
   assert(indx2row(536999999)==29); //middle
   assert(indx2row(1073741822)==29); //last
   
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

   //INDX2COL 
   assert(indx2col(-5)==-1); //negative index returns -1
   assert(indx2col(1073741823)==-1); //index too high returns -1
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
   assert(indx2col(16)==1); //second indx in row 4
   assert(indx2col(30)==15); //last indx in row 4
   assert(indx2col(31)==0); //first indx in row 5
   assert(indx2col(35)==4); //4th indx in row 5
   assert(indx2col(62)==31); //last indx in row 5
   assert(indx2col(63)==0); //first indx in row 6
   assert(indx2col(69)==6); //6th indx in row 6
   assert(indx2col(126)==63); //last indx in row 6
   assert(indx2col(127)==0);//first indx in row 7
   assert(indx2col(137)==10);//10th indx in row 7
   assert(indx2col(254)==127);//last indx in row 7
   assert(indx2col(255)==0);//first indx in row 8
   assert(indx2col(510)==255); //last index row 8
   assert(indx2col(511)==0); //first index row 9
   assert(indx2col(1022)==511); //last index row 9
   assert(indx2col(2046)==1023); //last index row 10
   assert(indx2col(4094)==2047); //last index row 11
   assert(indx2col(8190)==4095); //last index row 12
   assert(indx2col(16382)==8191); //last index row 13
   assert(indx2col(32766)==16383); //last index row 14
   assert(indx2col(65534)==32767); //last index row 15
   assert(indx2col(131070)==65535); //last index row 16
   assert(indx2col(262142)==131071); //last index row 17
   assert(indx2col(524286)==262143); //last index row 18
   assert(indx2col(1048574)==524287); //last index row 19
   assert(indx2col(2097150)==1048575); //last index row 20
   assert(indx2col(4194302)==2097151); //last index row 21
   assert(indx2col(8388606)==4194303); //last index row 22
   assert(indx2col(16777214)==8388607); //last index row 23
   assert(indx2col(33554430)==16777215); //last index row 24
   assert(indx2col(67108862)==33554431); //last index row 25
   assert(indx2col(134217726)==67108863); //last index row 26
   assert(indx2col(268435454)==134217727); //last index row 27
   assert(indx2col(536870910)==268435455); //last index row 28
   assert(indx2col(1073741822)==536870911); //last index row 29

   //BSA_IS_EMPTY
   bsa* c = bsa_init(); //empty BSA
   assert(bsa_is_empty(c)==true);
   row_alloc(c, 0); //allocate one row: BSA not empty
   assert(bsa_is_empty(c)==false); 
   bsa_free(c);
   
   //TOP_LIVE_ROW
   bsa* d = bsa_init();
   assert(top_live_row(d)==-1); //returns -1 for empty BSA i.e. no live rows
   row_alloc(d, 0); //allocate row 0
   assert(top_live_row(d)==0);
   row_alloc(d, 1); //allocate row 1
   assert(top_live_row(d)==1);
   row_alloc(d, 5); //allocate row 5
   assert(top_live_row(d)==5);
   row_alloc(d, 10); //allocate row 10
   assert(top_live_row(d)==10);
   row_alloc(d, 17); //allocate row 17
   assert(top_live_row(d)==17);
   row_alloc(d, 25); //allocate row 25
   assert(top_live_row(d)==25);
   row_alloc(d, 29); //allocate row 29
   assert(top_live_row(d)==29);
   bsa_free(d);

   //ROW_INDX2INDX
   assert(row_indx2indx(-1, 0)==-1); //invalid row low
   assert(row_indx2indx(30, 0)==-1); //invalid row high
   assert(row_indx2indx(0, 0)==0);
   assert(row_indx2indx(1, 0)==1);
   assert(row_indx2indx(2, 0)==3);
   assert(row_indx2indx(2, 1)==4);
   assert(row_indx2indx(2, 3)==6);
   assert(row_indx2indx(3, 4)==11);
   assert(row_indx2indx(3, 7)==14);
   assert(row_indx2indx(4, 4)==19);
   assert(row_indx2indx(5, 31)==62);
   assert(row_indx2indx(6, 63)==126);
   assert(row_indx2indx(7, 127)==254);
   assert(row_indx2indx(8, 255)==510);
   assert(row_indx2indx(9, 511)==1022);
   assert(row_indx2indx(10, 1023)==2046);
   assert(row_indx2indx(11, 2047)==4094);
   assert(row_indx2indx(12, 4095)==8190);
   assert(row_indx2indx(13, 8191)==16382);
   assert(row_indx2indx(14, 16383)==32766);
   assert(row_indx2indx(15, 32767)==65534);
   assert(row_indx2indx(16, 65535)==131070);
   assert(row_indx2indx(17, 131071)==262142);
   assert(row_indx2indx(18, 262143)==524286);
   assert(row_indx2indx(19, 524287)==1048574);
   assert(row_indx2indx(20, 1048575)==2097150);
   assert(row_indx2indx(21, 2097151)==4194302);
   assert(row_indx2indx(22, 4194303)==8388606);
   assert(row_indx2indx(23, 8388607)==16777214);
   assert(row_indx2indx(24, 16777215)==33554430);
   assert(row_indx2indx(25, 33554431)==67108862);
   assert(row_indx2indx(26, 67108863)==134217726);
   assert(row_indx2indx(27, 134217727)==268435454);
   assert(row_indx2indx(28, 268435455)==536870910);
   assert(row_indx2indx(29, 536870911)==1073741822);

   //IS_ROW_EMPTY
   bsa* e = bsa_init();
   row_alloc(e, 3); //allocate row 3
   assert(is_row_empty(e, 3)==true);
   bsa_set(e, 8, 10);
   assert(is_row_empty(e, 3)==false);
   bsa_free(e);

   //MAX_SET_ROW_INDX
   bsa* l = bsa_init();
   assert(l);
   bsa_set(l, 8, 4); //set index 8 to 4
   bsa_set(l, 10, 5); //set index 10 to 5
   assert(max_set_row_indx(l, 3)==3); // max set index in row 3 is row index 3 i.e. abs index 10
   bsa_set(l, 13, 7); //set index 13 to 6
   assert(max_set_row_indx(l, 3)==6); // max set index in row 3 is row index 6 i.e. abs index 13
   bsa_set(l, 14, 12); //set index 14 to 12
   assert(max_set_row_indx(l, 3)==7); // max set index in row 3 is row index 7 i.e. abs index 14
   bsa_set(l, 20, 300);
   assert(max_set_row_indx(l, 4)==5); // max set index in row 4 is row index 5
   bsa_set(l, 262142, 12);
   assert(max_set_row_indx(l, 17)==131071); // max set index in row 17 is row index 131071
   bsa_set(l, 1073741822, 55);
   assert(max_set_row_indx(l, 29)==536870911); // max set index in row 17 is row index 131071
   bsa_free(l);
  
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
 
   //ROW_IS_NULL
   bsa* t = bsa_init();
   assert(t);
   for(int row = 0; row < BSA_ROWS; row++){
      assert(row_is_null(t, row));
   }
   bsa_set(t, 20, 300); //set value in row 4
   assert(!row_is_null(t, 4)); //check row 4 is not null
   bsa_set(t, 4, 3); //set value in row 2
   assert(!row_is_null(t, 2)); //check row 2 is not null
   bsa_free(t);
   
}


