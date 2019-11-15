
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "defs.h"
#include "hash.h"

#define SAMPLES_TO_COLLECT   10000000
#define RAND_NUM_UPPER_BOUND   100000
#define NUM_SEED_STREAMS            4

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "noot",                  /* Team name */

    "Elizabeth Binks",                    /* Member full name */
    "1001274676",                 /* Member student number */
    "elizabeth.binks@mail.utoronto.ca",                 /* Member email address */
};

unsigned num_threads;
unsigned samples_to_skip;

class sample;

class sample {
  unsigned my_key;
 public:
  sample *next;
  unsigned count;

  sample(unsigned the_key){my_key = the_key; count = 0;};
  unsigned key(){return my_key;}
  void print(FILE *f){printf("%d %d\n",my_key,count);}
};

// This instantiates an empty hash table
// it is a C++ template, which means we define the types for
// the element and key value here: element is "class sample" and
// key value is "unsigned".  
hash<sample,unsigned> h;

void * thread_func(void *vargs);

pthread_mutex_t lock; 

typedef struct {
  int num_thread_seed_streams;
  int starting_index;
} args_t;

int main (int argc, char* argv[]){
  int i,j,k;
  int rnum;
  unsigned key;
  sample *s;

  // Print out team information
  printf( "Team Name: %s\n", team.team );
  printf( "\n" );
  printf( "Student 1 Name: %s\n", team.name1 );
  printf( "Student 1 Student Number: %s\n", team.number1 );
  printf( "Student 1 Email: %s\n", team.email1 );
  printf( "\n" );

  // Parse program arguments
  if (argc != 3){
    printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
    exit(1);  
  }
  sscanf(argv[1], " %d", &num_threads); // not used in this single-threaded version
  sscanf(argv[2], " %d", &samples_to_skip);

  // initialize a 16K-entry (2**14) hash of empty lists
  h.setup(14);

  if (pthread_mutex_init(&lock, NULL) != 0) 
  { 
      printf("failed to init mutex :(\n"); 
      return 1; 
  }

  pthread_t thread_ids[num_threads];
  int num_thread_seed_streams = NUM_SEED_STREAMS/num_threads;

  args_t *args = (args_t *)malloc(num_threads*sizeof(args_t *));

  for(i = 0; i < num_threads; i++) {
    args[i].num_thread_seed_streams = num_thread_seed_streams;
    args[i].starting_index = num_thread_seed_streams*i;

    //printf("starting_index %d\n", args[i].starting_index);

    int error = pthread_create(&thread_ids[i], NULL, thread_func, (void *)&args[i]);
    if (error != 0) {
      printf("failed to create thread %d\n", i);
    }
  }

  for(i = 0; i < num_threads; i++) {
    pthread_join(thread_ids[i], NULL);
  }

  free(args);

  pthread_mutex_destroy(&lock); 

  // print a list of the frequency of all samples
  h.print();
}

void * thread_func(void *vargs) {
  unsigned key;
  int rnum;
  sample *s;

  args_t *args = (args_t *)vargs;

  // process streams starting with different initial numbers
  for (int i = args->starting_index; i < args->starting_index+args->num_thread_seed_streams; i++){
    rnum = i;

    // collect a number of samples
    for (int j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (int k=0; k<samples_to_skip; k++){
       rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

      __transaction_atomic {
        // if this sample has not been counted before
        if (!(s = h.lookup(key))){

          // insert a new element for it into the hash table
          s = new sample(key);
          h.insert(s);
        }

        // increment the count for the sample
        s->count++;
      }

    }
  }

  return NULL;
}