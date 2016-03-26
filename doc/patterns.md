Design patterns notes
------

Add whatever reasonable code style & programming practices here as reminders.

### Terms definition

1. **MUST**, **MUST NOT**, **SHOULD**, **SHOULD NOT** and **MAY** are key words
  which follow definition in [RFC2119](https://www.ietf.org/rfc/rfc2119.txt).
  For those who are not familiar with these terms, here are definitions copied
  from that reference:
  * The word **MUST** (or **REQUIRED**, **SHALL**) means that the definition
    is an *absolute requirement*.
  * The phrase **MUST NOT** (or **SHALL NOT**) means that the definition
    is an *absolute prehibition*.
  * The word **SHOULD** (or **RECOMMENDED**) means that there may exist
    valid reasons in particular circumstances to ignore a particular item, but
    the full implications must be understood and carefully weighed before
    choosing a different course.
    - In case of choosing such different course, an explanation is needed.
  * The word **SHOULD NOT** (or **NOT RECOMMENDED**) means that
    there may exist valid reasons in particular circumstances when the
    particular behavior is acceptable or even useful, but the full
    implications should be understood and the case carefully weighed
    before implementing any behavior described with this label.
    - In case of implementing such behavior, an explanation is needed.
  * The word **MAY** (or **OPTIONAL**) means that an item is truly optional.
    An implementation which does not include that particular option **MUST** be
    prepared to interoperate with another implementation which does
    include the option, though perhaps with reduced functionality. In the
    same vein an implementation which does include a particular option
    **MUST** be prepared to interoperate with another implementation which
    does not include the option (except, of course, for the feature the
    option provides.)

### Code style notes

1. A function without arguments **MUST** be declared like

        void foo(void);

  The keyword `void` inside the parentheses **MUST NOT** be omitted, as nothing
  inside the parentheses means it can take *any* number of parameters, which
  is equivalent to

      void foo(...);

2. Internal functions **MAY** be prepended with double underscores `__`.
  * Code outside the source file **MUST NOT** call the internal functions,
    which are, functions whose names are prepended with double underscores `__`.
  * The internal functions is **RECOMMENDED** to be declared as `static`.

### Source organization notes

1. The file names in `include` directory, `include/arch/$ARCH` directory, and
  `include/arch/$ARCH/mach-$MACH` directory **MUST** be unique, as `gcc`
  looks for headers in all three directories, and will be confused if there
  are duplicates.

### Design notes

#### Driver framework

**Please review and update as needed.**

##### Principles

1. Every concrete hardware is a device.
2. A device could be a *character* device, a *block* device, a *network*
  device.  Driver framework *SHOULD* reserve for future weird types of
  device.
3. A device is always connected to a bus.
  * Memory-mapped I/O can be viewed as a memory bus.
  * So does port-mapped I/O.
4. A bus is a device.
5. Kernel communicates with buses either directly or via other buses,
  depending on physical connection.
  * E.g., kernel communicates with memory bus directly.
  * E.g., kernel can communicate with PCI bus via a memory bus.
6. Kernel *SHOULD* communicate with devices other than buses via buses.

##### Styles

Each driver **MUST** be designed so that it can be compiled and loaded as
kernel module(s).

1. Drivers **MUST** be compiled into PIC.

2. Internal functions **MUST** be prepended with double underscores `__`, and
   **MUST** be declared as `static`, so that drivers will not interfere with
   each other.

3. Each driver (or any other kernel module) **MUST** provide exactly one single
   routine to register its interface (structs or pointers) to the kernel. It
   **SHOULD** initialize the module, and **MAY** probe for device, but it
   **SHOULD NOT** initialize the driver or the device.

#### `early_console_init()`

Each driver **MAY** provide a weak `early_console_init()` function for a default
console initialization.  Architecture-specific or machine-specific kernel code
**MAY** override the driver-provided default initialization routine with its
own, in which case `early_console_init()` **MUST** be a strong function.

