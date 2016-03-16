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
```C
void foo(void);
```
  The keyword `void` inside the parentheses **MUST NOT** be omitted, as nothing
  inside the parentheses means it can take *any* number of parameters, which
  is equivalent to
```C
void foo(...);
```

### Functions

1. Internal functions **MAY** be prepended with double underscores `__`.
  * Code outside the source file **MUST NOT** call the internal functions,
    which are, functions whose names are prepended with double underscores `__`.
  * The internal functions is **RECOMMENDED** to be declared as `static`.

### `early_console_init()`

Each driver **MAY** provide a weak `early_console_init()` function for a default
console initialization.  Architecture-specific or machine-specific kernel code
**MAY** override the driver-provided default initialization routine with its
own, in which case `early_console_init()` **MUST** be a strong function.
