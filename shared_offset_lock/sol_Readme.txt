sol => Shared Offset Lock
. For design detail, please refer to sol_Design.txt
. For testing detail, please refer to sol_Testcases.txt
. For code, refer 
      libsol/sol.c - main implementation
      libsol/sol.h - public header to be used by application
      libsol/sol_priv.h - private header
      libsol/sol_test.c - Code to unit test the functionality.
. For compiling, go to libsol, and use 
      - make
. For running the application, use 
      ./libsol/test_sol -t <number_of_threads> -s <start_offset> -e <end_offset> -c <count_of_operation_by_each_threads> [-r <lock_to_resource_ratio>] [ -D(efect_mode) ] [ -i <iterations>]
      => e.g.
      We can run the program, as follows -
      ./libsol/test_sol -t 2 -s 100 -e 200 -c 500 
      => This makes the application have a shared resource to be accessed from offset 100 to 200. It spawns two threads, who does the following job 
         - Take exclusive lock, write into the resource, own pthread_id, unlock
         - Take shared lock, read the resource, and do a sanity check, unlock.
         - The above operation is repeated counted number of times.

      - In the above mentioned work, each thread operate on the entire range. Thus the chance for conflict is higher. So the lock provides the opportunity to do the atomically. If done atomically, at any point of time, only one pthread_id would be written on the shared memory between threads on heap segment. The sanity check, checks for the same, and aborts, if the condition is not met.

      - With -D option, i.e. in defect_mode, while reading the program allows for half the threads not to take the lock. This creates a violation in access synchronization of read/write. Sanity check detects the same and aborts the application. With higher amount of resource range, or higher number of threads, higher count of operation, increases the probability of.
      => 
      ubuntu@ip-172-31-1-235:~/git/myc/shared_offset_lock/libsol$ ./test_sol -t 16 -s 100 -e 2000 -c 1000 
      ubuntu@ip-172-31-1-235:~/git/myc/shared_offset_lock/libsol$ ./test_sol -t 16 -s 100 -e 2000 -c 1000 -D
      test_sol: sol_test.c:73: thread_work: Assertion `inst->data[j] == inst->data[j+1]' failed.
      Aborted (core dumped)
      ubuntu@ip-172-31-1-235:~/git/myc/shared_offset_lock/libsol$ 

      - With -r option, we may chose the desired ratio of lock to resource. i.e. with -r 10, we will get a 1:10 of lock:resource. By default, i.e. with -r 0 or unspecified, the library uses a 1:4 ratio.
     
      - With -i option, we may chose to repeat the entire process of the test number of iterations.
     
. For running various tests, run sol_test.sh
