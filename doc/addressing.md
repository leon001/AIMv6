Addressing guidelines
------

This documents explains the addressing throughout AIMv6. Related behavior is
also described here.

### Terms

1. **Symbol** is a name associated with an address, which may come from C
   definitions, assembly definitions or linker script assignments. In some
   formats, symbols and their values can be directly accessed.
2. **Section** is a part of an executable. These parts are divided for different
   access methods and addressing.
3. **Segment** is a loading unit of an executable, and may contain zero to many
   sections. When available, corresponding headers state how and where they
   should be loaded.
4. **Load** is the action that writes executable into memory. The
   executable may come in different formats, from `plain binary` to `ELF`.
5. **Loader** is the executable that performs a load.
6. **Load address** is the address that executables are loaded to, seen during
   load time, from the loader's point of view. Load address is set during link
   time, and **MUST NOT** be violated. In `ELF` format, load address is stored
   in `PHDR` as `p_paddr`, which does not mean physical address.
7. **Execution address** is the address the program uses during its execution.
   In `ELF` format, load address is stored in `PHDR` as `v_paddr`, but it may
   be violated. In certain periods during initialization, the kernel may even
   have more than one execution address available. However, for each memory
   access, only one can be used.

### Booting
* 

