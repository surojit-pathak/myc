/*
 * @free code
   @author 
    - surojit.pathak@gmail.com
 * sol_test.c => shared offset locking - test code
 */

#include "sol.h"

/* Instance data structure for each sol testing activity. */
typedef struct shared_data_access_block {
    sol_handle_pub_t            *solh;
    sol_index_t                  start;
    sol_index_t                  end;
    int                          nthr;
    int                          count;
    int                          data_len;
    int                         *thr_data;
    pthread_t                   *tid_arr;
    int                         *data;
} shared_data_access_block_t;

/* following helps to ensure catch memory failure and also initializes with 0 
 * Not ideal for real applications, as they may have to do graceful handling of 
 * memory failure. Here it helps to quick check and fail for UT code
 */
static inline void *
test_alloc (size_t size)
{   
    void *__mem = malloc((size));
    if (__mem == NULL) assert(0);
    memset(__mem, 0, size);
    return __mem;
}

#define TEST_ALLOC   test_alloc

/* Introduce bug :) */
bool defect_mode = false;

void *
thread_work (void *arg)
{
   int       i, j;
   shared_data_access_block_t *inst = (shared_data_access_block_t *)arg;
   pthread_t tid = pthread_self();
   int       my_tidx = 0;

   /* find out i-th thread instance  - used for introducing bug to test */
   for (i = 0; i < inst->nthr; i++) {
       if (inst->tid_arr[i] == tid) {
           my_tidx = i; 
           break; 
       }
   }
     

   for (i = 0; i < inst->count; i++) {
       assert(!sol_lock_exclusive(inst->solh, inst->start, inst->end - inst->start));
       /* Critical Section - 1 - write */
       for (j = 0; j <= inst->end - inst->start ; j++) {
           inst->data[j] = tid;
       }
       assert(!sol_unlock_exclusive(inst->solh, inst->start, inst->end - inst->start));

       /* 
        * Basic Debugging Log 
          printf("\n After writing, at thread %d with %u - data %d\n", 
		       my_tidx, (unsigned int)tid, inst->data[0]);
        */
   
       if (defect_mode) {
           /* In defect_mode, take lock for half the threads, so that we run into
              synchronization issue.
            */
           if (my_tidx % 2) assert(!sol_lock_shared(inst->solh, inst->start, inst->end - inst->start));
       } else {
           assert(!sol_lock_shared(inst->solh, inst->start, inst->end - inst->start));
       }

       /* Critical Section - 2 - read and sanity check */
       for (j = 0; j < inst->end - inst->start ; j++) {
           assert(inst->data[j] == inst->data[j+1]);
       }

       if (defect_mode) {
	       if (my_tidx % 2) assert(!sol_unlock_shared(inst->solh, inst->start, inst->end - inst->start));
       } else {
	       assert(!sol_unlock_shared(inst->solh, inst->start, inst->end - inst->start));
       }
   }
   
}

int main(int argc, char *argv[])
{
    sol_handle_pub_t *hdl = NULL;
    int               opt;
    sol_index_t       start = -1;
    sol_index_t       end = -1;
    size_t            nthr = 0;
    size_t            count = 0;
    size_t            iter = 1;
    int               i = 0;
    int               ratio = 0;

    while ((opt = getopt(argc, argv, "c:De:i:r:s:t:")) != -1) {
        switch(opt) {
        case 'c':
            count = strtoul(optarg, NULL, 10);
            break;
        case 'D':
            defect_mode = true;
            break;
        case 'e':
            end = strtoul(optarg, NULL, 10);
            break;
        case 'i':
            iter = strtoul(optarg, NULL, 10);
            break;
        case 'r':
            ratio = strtoul(optarg, NULL, 10);
            break;
        case 's':
            start = strtoul(optarg, NULL, 10);
            break;
        case 't':
            nthr = strtoul(optarg, NULL, 10);
            break;
        default:
            printf("\nError, wrong option!\n");
            break;
        }
    }

    do { /* repeat the test */
	    if (nthr != 0 && start != -1 && end != -1 && count != 0) {

		    shared_data_access_block_t *inst =
			    TEST_ALLOC(sizeof(shared_data_access_block_t));
		    inst->nthr = nthr;
		    inst->start = start;
		    inst->end = end;
		    inst->count = count;
		    inst->data_len = sizeof(int) * (end - start + 1);
		    inst->data = TEST_ALLOC(inst->data_len);
		    inst->thr_data = TEST_ALLOC(sizeof(int) * nthr);
		    inst->tid_arr = TEST_ALLOC(sizeof(pthread_t) * nthr);

		    assert(!sol_init(start, end, ratio, &inst->solh));

		    for (i = 0; i < nthr; i++) {
			    inst->thr_data[i] = i;
			    pthread_create(&inst->tid_arr[i], NULL, thread_work, 
					    (void *)inst);
		    }

		    for (i = 0; i < nthr; i++) {
			    pthread_join(inst->tid_arr[i], NULL);
		    }

		    assert(!sol_fini(inst->solh));

                    free(inst->tid_arr);
                    free(inst->thr_data);
                    free(inst->data);
                    free(inst);
	    } else {
		    printf("Not enough param, module not tested\n");
		    return -1;
	    }
    } while (--iter);
       
    return 0;
}
