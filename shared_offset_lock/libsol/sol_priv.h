/*
 *
 * @free code
   @author 
    - surojit.pathak@gmail.com
 * sol_priv.h => shared offset locking - private header
 */
#ifndef __SOL_PRIV_H__
#define __SOL_PRIV_H__

/* MACRO-ization of the following helps get flexibility, extensibility */
#define SOL_ASSERT  assert
#define SOL_ASSERT  assert
#define SOL_ALLOC   malloc
#define SOL_FREE    free

typedef struct sol_handle_ {
    sol_index_t            sh_start;
    sol_index_t            sh_end;
    size_t                 sh_ratio;

    sol_index_t            sh_num_lock;
    pthread_rwlock_t      *sh_lock_arr;
} sol_handle_t;


#endif /* !__SOL_PRIV_H__ */
