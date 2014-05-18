/*
 * @free code
   @author 
    - surojit.pathak@gmail.com
 * sol.c => shared offset locking implementation
 */

#include "sol.h"
#include "sol_priv.h"

/* Stats for errors */
static struct sol_global_stats_ {
    unsigned long long          sgs_no_mem;
    unsigned long long          sgs_lock_init_err;
    unsigned long long          sgs_lock_destroy_err;
} sgs;


extern sol_result_t
sol_init (sol_index_t start, 
          sol_index_t end, 
          size_t      lock_ratio,
          sol_handle_pub_t **hdlp) 
{
    sol_handle_t        *hdl = NULL;
    sol_index_t          abs_range = 0;
    int                  i = 0;
    int                  j = 0;
    int                  rs = 0;
    
    /* Validate the parameters */
    if (hdlp == NULL) {
        /* Pointer is wrong */
        return SOL_RESULT_WRONG_PARAM;
    }

    if (end < start) {
        /* Pointer is wrong */
        return SOL_RESULT_WRONG_PARAM;
    }

    hdl = SOL_ALLOC(sizeof(sol_handle_t));
    if (hdl == NULL) {
        sgs.sgs_no_mem++;
        return SOL_RESULT_NO_MEM;
    }
    memset(hdl, 0, sizeof(sol_handle_t));

    if (lock_ratio == 0) {
        lock_ratio = SOL_DEAFULT_LOCK_TO_RANGE_RATIO;
    }

    abs_range = end - start + 1;
    hdl->sh_num_lock = (abs_range / lock_ratio) + 
                       (abs_range % lock_ratio ? 1 : 0); 
    hdl->sh_lock_arr = malloc(sizeof(pthread_rwlock_t) * hdl->sh_num_lock);
    if (hdl->sh_lock_arr == NULL) {
        SOL_FREE(hdl);
        sgs.sgs_no_mem++;
        return SOL_RESULT_NO_MEM;
    }
    hdl->sh_start = start;
    hdl->sh_end = end;
    hdl->sh_ratio = lock_ratio;
    
    for (i = 0; i < hdl->sh_num_lock; i++) {
        /* Using NULL attribute for lock, as of now */
        rs = pthread_rwlock_init(&hdl->sh_lock_arr[i], NULL);
        if (rs) {
            sgs.sgs_lock_init_err++;
            for (j = i - 1; j >= 0; j--) {
                rs = pthread_rwlock_destroy(&hdl->sh_lock_arr[j]);
                if (rs) {
                    SOL_ASSERT(0);
                    /* We will just account this error. */
                    sgs.sgs_lock_destroy_err++;
                }
            }
            SOL_FREE(hdl->sh_lock_arr);
            SOL_FREE(hdl);
            return SOL_RESULT_LOCK_INIT_FAILURE;
        }
    }
    
    *hdlp = (sol_handle_pub_t) hdl;
    return SOL_RESULT_SUCCESS;
}
   
static sol_result_t 
sol_validate_range (sol_handle_t     *hdl,
                    sol_index_t offset, 
                    sol_index_t length) 
{
    if (hdl == NULL) return SOL_RESULT_WRONG_PARAM;

    if (offset < hdl->sh_start || offset > hdl->sh_end) {
        /* Wrong offset specified */
        return SOL_RESULT_WRONG_OFFSET;
    }

    if (length > (hdl->sh_end - hdl->sh_start)) {
        /* Wrong Length specified */
        return SOL_RESULT_WRONG_LENGTH;
    }

    if (offset > hdl->sh_end - length) {
        /* This is to check, if off + len > end, but avoiding overflow. */
        return SOL_RESULT_WRONG_OFFSET_LENGTH;
    }

    return SOL_RESULT_SUCCESS;
}
      
                 
static sol_result_t
sol_lock_internal (sol_handle_pub_t *phdl,
                   sol_index_t offset, 
                   sol_index_t length, 
                   bool        lock,
                   bool        shared) 
{
    sol_index_t            end;
    sol_result_t           srs;
    sol_handle_t          *hdl;
    int                    lstarti = 0;
    int                    lendi = 0;
    int                    i = 0;
    int                    rs = 0;

    hdl = (sol_handle_t *)phdl;
    
    srs = sol_validate_range(hdl, offset, length);
    if (srs != SOL_RESULT_SUCCESS) return srs;

    end = offset + length;

    lstarti = (offset - hdl->sh_start) / hdl->sh_num_lock;
    lendi = (offset + length - hdl->sh_start) / hdl->sh_num_lock;

    SOL_ASSERT(lendi <= hdl->sh_num_lock);
    
    for (i = lstarti; i < lendi; i++) {
        if (lock) {
            rs = shared ? pthread_rwlock_rdlock(&hdl->sh_lock_arr[i]) 
                : pthread_rwlock_wrlock(&hdl->sh_lock_arr[i]);
            /* lock() may fail for deadlock, as we are using non-recursive */
        } else {
            rs = pthread_rwlock_unlock(&hdl->sh_lock_arr[i]);
            /* Unlock fail indicates, wrong lock */
        }

        if (rs != 0) {
            SOL_ASSERT(0);
        }
    }

    return SOL_RESULT_SUCCESS;
}

extern sol_result_t
sol_lock_shared  (sol_handle_pub_t *phdl,
                  sol_index_t offset, 
                  sol_index_t length) 
{
    return sol_lock_internal(phdl, offset, length, true /* LOCK */,
                             true /* SHARED */);
}

extern sol_result_t
sol_unlock_shared  (sol_handle_pub_t *phdl,
                    sol_index_t offset, 
                    sol_index_t length) 
{
    return sol_lock_internal(phdl, offset, length, false /* UNLOCK */,
                             true /* SHARED */);
}

extern sol_result_t
sol_lock_exclusive (sol_handle_pub_t *phdl,
                    sol_index_t offset, 
                    sol_index_t length) 
{
    return sol_lock_internal(phdl, offset, length, true /* LOCK */,
                             false /* EXCLUSIVE */);
}

extern sol_result_t
sol_unlock_exclusive (sol_handle_pub_t *phdl,
                      sol_index_t offset, 
                      sol_index_t length) 
{
    return sol_lock_internal(phdl, offset, length, false /* UNLOCK */,
                             false /* EXCLUSIVE */);
}

extern sol_result_t 
sol_fini (sol_handle_pub_t *phdl)
{
    sol_handle_t          *hdl;
    int                    i = 0;
    int                    rs = 0;

    hdl = (sol_handle_t *)phdl;
    if (hdl == NULL) return SOL_RESULT_WRONG_PARAM;

    for (i = 0; i < hdl->sh_num_lock; i++) {
        rs = pthread_rwlock_destroy(&hdl->sh_lock_arr[i]);
        if (rs) {
            SOL_ASSERT(0);
            sgs.sgs_lock_destroy_err++;
            return SOL_RESULT_LOCK_DESTROY_FAILURE;
        }
    }

    SOL_FREE(hdl->sh_lock_arr);
    SOL_FREE(hdl);
 
    return SOL_RESULT_SUCCESS;
}
