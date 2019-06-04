#include <stdio.h>
#include <stdlib.h>

#define BITSPERSEG (8*256*sizeof(int))

//creating the struct for a segment of bits
//for use in the bitmap
typedef struct _seg {
   int bits[256];
   struct _seg *next, *prev;
} seg;

//global pointer to head
seg *head;

//global pointer for general use
//and then for use in whichseg
seg *pt;

//amount of nodes required in the linked list
int nodes;

//the number of the last node used in whichseg
int prevNode;

//the input from either the user or the file
int input;

//returns a pointer to the seg which contains the
//bit that represents the input j in the bitmap
seg* whichseg(int j){

   j = (j-3)/2;
   int nodeNum = j / BITSPERSEG;
   int i;
   seg *p = pt;

   //if the node we want is the same as last one
   if(nodeNum == prevNode){
      return p;
    //if the node we want is bigger, we traverse with next
   }else if(nodeNum > prevNode){
      for(i = prevNode;i<nodeNum;i++){
         p=p->next;
      }
    //if the node we want is smaller, we traverse with prev
   }else{
      for(i = prevNode;i>nodeNum;i--){
         p=p->prev;
      }
   }

   //since whichseg is called very often using increasing
   //values of j, if we store the previous result, the
   //function is a lot more efficient
   pt = p;
   prevNode = nodeNum;
   return p;

}

//returns the index of bits[] in which j is
int whichint(int j){

   j = (j-3)/2;
   return (j/(8*sizeof(int)))%256;

}

//returns the position of j in the int which it
//is stored in. Goes from right to left.
int whichbit(int j){

   j = (j-3)/2;;
   return j % (8*sizeof(int));

}

//uses bitwise OR operation to make a bit 1
//which means it is not a prime
int marknonprime(int n){

	int index = whichint(n);
	int pos = whichbit(n);
   seg *p = whichseg(n);

   p->bits[index] = (p->bits[index] | (1 << pos));

   return 1;
}

//uses bitwise NOR to check if the bit corresponding to n
//is a 0 (prime) or 1 (not prime) and returns 1 if it is a prime
int testprime(int n){

   int pos = whichbit(n);
   int index = whichint(n);
   seg *p = whichseg(n);

   if(~(p->bits[index] | ~(1 << pos)) != 0){
      return 1;
   }

   return 0;
}

//goes through the bitmap and counts how many primes there
//are by using the testprime function on each bit. Returns
//the amount of primes.
int countPrimes(){
   int i;
   int counter = 0;

   for(i = 3;i<input;i+=2){
      if(testprime(i) == 1){
         counter++;
      }
   }

   return counter;

}

//the 'Goldbach Test' method, decomposes the input from either
//user or file into two primes lower than input while trying
//to make the two numbers as close as possible to the middle
void gb(){
   int value, i, k;
   int answer[3];
   int i_index, i_pos, k_index, k_pos;
   seg *ipt, *kpt;

   //continuous while loop that performs the goldbach test
   while(1){
      answer[0] = 0;
      answer[1] = 0;
      //break when you reach eof (end of file or ctrl+d)
      if(scanf("%d",&value) == EOF){break;}
      if(value % 2 != 0 || value < 5){continue;}

      //variables used during goldbach test
      i = 3;
      ipt = whichseg(i);
      i_index = whichint(i);
      i_pos = whichbit(i);
      k = value-i;
      kpt = whichseg(k);
      k_index = whichint(k);
      k_pos = whichbit(k);

      while( i <= value / 2){
         //check for primality when i+k equal the desired input
         if( i + k == value 
               && (~(ipt->bits[i_index] | ~(1 << i_pos)) != 0)
               && (~(kpt->bits[k_index] | ~(1 << k_pos)) != 0)){

            if(k < input && i < input){
               answer[0]++; //count solutions
               if(i > answer[1]){
                  //always save the 'best' solution
                  answer[1] = i;
                  answer[2] = k;
               }
            }

         }

         //update i coordinate variables
         if(i_pos == 31){
            i_pos = 0;
            if(i_index == 255){
               i_index = 0;
               ipt = ipt->next;
            }else{
               i_index++;
            }
         }else{
            i_pos++;
         }

         //update k coordinate variables
         if(k_pos == 0){
            k_pos = 31;
            if(k_index == 0){
               k_index = 255;
               kpt = kpt->prev;
            }else{
               k_index--;
            }
         }else{
            k_pos--;
         }

         //update i and k
         i+=2;
         k-=2;

      }

   //print out the best answer and the amount of solutions
   if(answer[0] != 0){
            printf("Largest %d = %d + %d out of %d solutions\n",value,answer[1],answer[2],answer[0]);
         }   
   }
}

//dynamically allocates space for the bitmap
//clears all the bits in the bitmap
//performs the sieve
void main(int argc, char *argv[]){

   //handling input
   if(argc == 2){
      sscanf(argv[1],"%d",&input);
   }else{
      scanf("%d",&input);
   }   

   //calculate number of nodes needed
   nodes = ((input - 2)/2 + BITSPERSEG -1) / BITSPERSEG;
   head = (seg*) malloc(sizeof(seg));
   pt = head;

   printf("Calculating odd primes up to %d...\n", input);

   int i, z;

   //dynamically add nodes to linked list
   for(i=1;i<nodes;i++){
      pt->next = (seg*) malloc(sizeof(seg));
      pt->next->prev = pt;      
      pt = pt->next;
   }

   //reset global pointer to head of linked list
   pt = head;

   //clearing all bits
   for(i=0;i<nodes;i++){
      for(z=0;z<256;z++){
         pt->bits[z] = 0b00000000000000000000000000000000;
      }
      pt=pt->next;
   }

   int k = 3;
   int j;
   pt = head;
   prevNode = 0;

   //sieve finds all primes under input
   //uses square root of input for efficiency
   while(k*k <= input){
      //find primes
		for(i = k;i <= input; i+=2){
         if(testprime(i) == 1){
            break;
         }
		}
      //mark multiples of primes as non prime
		for(j = (3*i); j < input; j += 2*i){
   	   marknonprime(j);
		}

		k = i + 2;
	}

   printf("The number of odd primes less than or equal to %d is: %d\nEnter Even Numbers >5 for Goldbach Tests\n", input, countPrimes());

   //start goldbach test method
   gb();
}