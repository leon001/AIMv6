# Toolchain

This guide shows how to get a toolchain to build AIMv6 with. We highly
recommend that you go through this guide before you go on.

To build AIMv6, your toolchain should at least contain a C compiler, a set of
utilities to work with machine code, and a simple C library.

## Cross Building

At most three platforms are involved when cross building happens, `build`,
`host`, and `target`. This means `build` is compiling programs who run on `host`
and work with the machine code of `target`.

If `build` is different from `host`, we are cross compiling. If `host` is
different from `target`, programs are built by cross compiling tools.

## Native Toolchain

AIMv6 can be built natively if your `build` platform is supported. In this
case, install basic development packages from your distribution's repository,
and you can proceed to building.

## Cross Toolchain

Tools on your `build` platform only produce machine code for the same platform,
so a seperate toolchain is needed to cross build AIMv6. This toolchain can be
installed from various sources, and may or may not work well.

Those who don't wish to build from sources and exercise dark magic can refer
to [Crosstool-NG](http://crosstool-ng.org/) instead.  This is an easy-to-use,
all-in-one cross toolchain builder which automatically builds `binutils`, `gcc`,
and C library for you.

### Building with Crosstool-NG

Fetch a release from Crosstool-NG official website, and execute

```
./configure
make
make install
```

Run `ct-ng` to see usage.

#### MIPS developers

Crosstool-NG already ships a MIPS cross toolchain
configuration.  I tried `mips64el-n64-linux-uclibc` and it works for our purpose.
Other configurations are not tested.

Run

```
ct-ng list-samples
```

to see other available default configurations.

Run

```
ct-ng mips64el-n64-linux-uclibc
```

to select such configuration.

Run

```
ct-ng build
```

and everything will be set up at `~/x-tools/mips64el-n64-linux-uclibc`.

Your host machine triplet for our project will be then `mips64el-n64-linux-uclibc`.

Add the `bin` directory there to your `PATH` variable, either via

```
export PATH=$PATH:$HOME/x-tools/mipes64el-n64-linux-uclibc
```

or via configuration files such as `~/.bashrc` or `~/.profile`.

You are done.

#### ARM developers

Crosstool-NG also ships an `arm-unknown-eabi` configuration.  The installation
steps are mostly similar to those for MIPS developers, except that the target
names are changed to `arm-unknown-eabi`.

**Not yet tested, though.**

### Building From Scratch

Building a good toolchain involves *LOTS* of knowledge, and often *LOTS* of
failures as well. For MIPS64 users, this involves patching binutils by hand.
This guide can't help you all the way, but if you're on ARM, it should work
just fine.

#### Fetch Sources

To continue, you need sources for all programs involved in the toolchain. For
example, `gcc` as compiler, `GNU binutils` as machine code tools, and `newlib`
as C library. In this example, `gcc` depends on `gmp`, `mpc`, and `mpfr`.
Always use latest released version when possible. The rest of this guide
assumes that you follow this example.

As unprivileged user, create a directory in your home folder as the install
prefix for your toolchain.

#### `Binutils`

As an unprivileged user, unpack binutils tarball and create a seperate building
directory for it. While you're in the building directory, configure `binutils`:

```bash
/path-to-source/configure	\
--prefix=/install-prefix	\
--target=your-target		\
--some-extra-parameters
```

Rewrite the code above with your desired values.

 * `/path-to-source`: path pointing to `binutils` source.
 * `/install-prefix`: path to your install prefix directory.  You can leave this
option out to make it default (i.e. `/usr/local`).
 * `your-target`: triplet of your desired target, like `i386-linux-gnu` or
   `arm-unknown-eabi` or `mips64-unknown-elf`
   + You can figure one out through `config.sub` script:
```
/path-to-source/config.sub
```
 * `--some-extra-flags`: extra parameters to pass to configure. Frequently used
   ones include:
   - `--enable-thumb`: turn on thumb instruction set support on ARM platform
   - `--enable-interwork`: turn on processor mode interworking support on some
     platforms.
   - `--enable-multilib`: turn on multilib support. It builds libraries for
     multiple instruction sets, and may prove to be useful someday.

A sample configuration for ARM could look like

```
/path-to-source/configure	\
--prefix=/usr/local		\
--target=arm-unknown-eabi	\
--enable-thumb			\
--enable-interwork		\
--enable-multilib
```

`configure` includes a lot of tests, and may fail if something is missing. In
that case, install corresponding packages and try again. When everything works
well, go on and compile:

```bash
make
```

You can add `-jn`, to allow multiple jobs running in parallel, where n is the
number of jobs you desire. This option makes the build log hard to analyze at
the same time.

If building succeeds, go on to install:

```bash
make install
```

This should not produce any problems. If you want to run tests, run them
before you install:

```bash
make tests
```

#### `gcc` Pass 1

Just like `binutils`, extract `gcc` source, and extract `gmp`, `mpc`, and `mpfr`
sources *into* `gcc` source tree, and remove version numbers in their folder
names.

Run the `configure` script just like above, but adding more parameters:

```
./configure			\
--prefix=/usr/local		\
--target=arm-unknown-eabi	\
--enable-thumb			\
--enable-interwork		\
--enable-multilib		\
--enable-languages=c		\
--without-headers		\
--with-newlib			\
--disable-libssp		\
--with-system-zlib
```

 * `--enable-languages=c`: build only the C compiler.
 * `--without-headers`: we don't have any headers yet.
 * `--with-newlib`: we don't have a working libc yet.
 * `--disable-libssp`: `libssp` will (likely) fail, so disable it now. If you
   come into any other subpackage that fails to build, disable them as well.
 * `--with-system-zlib`: if you are cross compiling, the shipped `zlib` will
   (likely) fail. It's okay to use the one installed on your system.
 * Parameters controlling thumb, interwork and multilib still apply.

Still, run `make` and then `make install` after configuration.

#### Adjusting `$PATH`

`$PATH` is a variable for your shell to find executables. There is a `bin`
folder inside your install prefix, add it to `$PATH`. If you're in `bash`, run:

```bash
export PATH=$PATH:/install-dir/bin
```

Where `/install-dir` is your install prefix. You may want to add this line to
some configuration files such as `~/.bashrc` or `~/.profile`.

You can now compile bare-metal programs.

#### `newlib`

Similar to `binutils`, but configure it with less options. Passing `prefix`,
`target` and `enable-multilib` would be good enough.

So a `newlib` working on ARM can have the following configuration:

```bash
./configure --prefix=/usr/local --target=arm-unknown-eabi --enable-multilib
```

By setting `target` instead of `host`, the building system will build libraries
suitable for your developing environment (eg. `x86_64-pc-linux-gnu` as `host`).
Libraries will be installed into `$prefix/$target` folder, and will not
conflict with anything else under the `prefix` tree.

Still, run `make` and `make install`.

#### `gcc` Pass 2

Now that we have a C library, gcc will build better. In a *new* building
directory, configure it *without* `--without-headers`, `--with-newlib`,
 and all the
subproject-disabling parameters, while other parameters stay the same.
You'll still need to apply `with-system-zlib` if you enable multilib.

```
./configure			\
--prefix=/usr/local		\
--target=arm-unknown-eabi	\
--enable-thumb			\
--enable-interwork		\
--enable-multilib		\
--enable-languages=c		\
--with-newlib			\
--with-system-zlib
```

`make` and `make install`, and your toolchain should be sane enough for AIMv6.

