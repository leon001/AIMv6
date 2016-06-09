System calls
-----

The system calls are designed as modules here: each system call has exactly one module handling it.

1. Each system call (or each family of system calls) *SHOULD* be handled in one source file.
2. No system call can receive more than 8 arguments.
3. `int sysno, int *errno` should be prepended to the system call argument list in the kernel side system call handler,
  where the error number is stored.
  * E.g. the kernel side handler `sys_execve` for system call
  
    ```C
    execve(const char *, const char *[], const char *[])`;
    ```
    
    should look like
    
    ```C
    int sys_execve(int sysno, int *errno, const char *fn, const char *argv[], const char *envp[]);
    ```
4. After each handler the following statement should be added for registering the handler:

  ```C
  ADD_SYSCALL(syscall_handler, syscall_number);
  ```
  
  where `syscall_number` should match the system call number in C library.
  
The system call registration framework exploits initcalls to work.  See the definition of `ADD_SYSCALL`
in `include/syscall.h` for details, if you are interested.
