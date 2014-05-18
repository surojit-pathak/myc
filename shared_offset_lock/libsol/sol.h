/*
 * @free code
   @author 
    - surojit.pathak@gmail.com
 * sol.h => shared offset locking - public header
 *
 * README:
 *   - This file to be included as topmost header for <pthread.h>
 */
#ifndef __SOL_H__
#define __SOL_H__
#define __USE_UNIX98
#include <features.h>
#define __USE_UNIX98
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>

typedef enum {
    SOL_RESULT_SUCCESS = 0,
    SOL_RESULT_FAILURE,
    SOL_RESULT_WRONG_PARAM,
    SOL_RESULT_WRONG_OFFSET,
    SOL_RESULT_WRONG_LENGTH,
    SOL_RESULT_WRONG_OFFSET_LENGTH,
    SOL_RESULT_NO_MEM,
    SOL_RESULT_LOCK_INIT_FAILURE,
    SOL_RESULT_LOCK_DESTROY_FAILURE,
} sol_result_t;

typedef struct sol_param_ {
} sol_param_t;

typedef void * sol_handle_pub_t;
typedef uint32_t sol_index_t;

#define SOL_DEAFULT_LOCK_TO_RANGE_RATIO 4
/* Public APIs */
extern sol_result_t
sol_init(sol_index_t start, 
         sol_index_t end, 
         size_t      lock_ratio,
         sol_handle_pub_t **hdlp);
extern sol_result_t
sol_lock_shared(sol_handle_pub_t *phdl,
                sol_index_t offset, 
                sol_index_t length); 
extern sol_result_t
sol_unlock_shared(sol_handle_pub_t *phdl,
                  sol_index_t offset, 
                  sol_index_t length);
extern sol_result_t
sol_lock_exclusive(sol_handle_pub_t *phdl,
                   sol_index_t offset, 
                   sol_index_t length); 
extern sol_result_t
sol_unlock_exclusive(sol_handle_pub_t *phdl,
                     sol_index_t offset, 
                     sol_index_t length); 
extern sol_result_t 
sol_fini(sol_handle_pub_t *phdl);

#endif /* !__SOL_H__ */
