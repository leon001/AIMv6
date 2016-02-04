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
different from `target`, programs built are cross compiling tools.

## Native Toolchain

AIMv6 can be built natively if your `build` platform is supported. In this
case, install basic development packages from your distribution's repository,
and you can proceed to building.

## Cross Toolchain

Tools on your `build` platform only produce machine code for the same platform,
so a seperate toolchain is needed to cross build AIMv6. This toolchain can be
installed from various sources, and may or may not work well. We recommend
building one with `crosstool-ng`, or building one from sources.

### Building with `crosstool-ng`

`crosstool-ng` is an individual project, and shipped scripts will fetch, build
and install a toolchain according to given parameters. If you are working on
MIPS architecture, this may be your best choice, as we're going to patch
`binutils`, and it's not quite finished yet.

Building toolchains by hand require a *LOT* of knowledge, while building with
scripts limits your control over the product. Please make your choices
carefully.

### Sources

To continue, you need sources for all programs involved in the toolchain. For
example, `gcc` as compiler, `GNU binutils` as machine code tools, and `newlib`
as C library. In this example, `gcc` depends on `gmp`, `mpc`, and `mpfr`.
Always use latest released version when possible. The rest of this guide
assumes that you follow this example.

As unprivileged user, create a directory in your home folder as the install
prefix for your toolchain.

### `Binutils`

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
 * `/install-prefix`: path to your install prefix directory.
 * `your-target`: triplet of your desired target, like `i386-linux-gnu` or
   `arm-unknown-eabi` or `mips64-unknown-elf`
 * `--some-extra-flags`: extra parameters to pass to configure. Frequently used
   ones include:
   - `--enable-thumb`: turn on thumb instruction set support on ARM platform
   - `--enable-interwork`: turn on processor mode interworking support on some
     platforms.
   - `--enable-multilib`: turn on multilib support. It builds libraries for
     multiple instruction sets, and may prove to be useful someday.

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

### `gcc` Pass 1

Just like `binutils`, extract `gcc` source, and extract `gmp`, `mpc`, and `mpfr`
sources *into* `gcc` source tree, and remove version numbers in their folder
names.

Run the `configure` script just like above, but adding more parameters:

 * `--enable-languages=c`: build only the C compiler.
 * `--without-headers`: we don't have any headers yet.
 * `--with-newlib`: we don't have a working libc yet.
 * `--disable-libssp`: `libssp` will (likely) fail, so disable it now. If you
   come into any other subpackage that fails to build, disable them as well.
 * `--with-system-zlib`: if you are cross compiling, the shipped `zlib` will
   (likely) fail. It's okay to use the one installed on your system.
 * Parameters controlling thumb, interwork and multilib still apply.

Still, run `make` and then `make install` after configuration.

### Adjusting `$PATH`

`$PATH` is a variable for your shell to find executables. There is a `bin`
folder inside your install prefix, add it to `$PATH`. If you're in `bash`, run:

```bash
export PATH=$PATH:/install-dir/bin
```

Where `/install-dir` is your install prefix.

You can now compile bare-metal programs.

### `newlib`

Similar to `binutils`, but configure it with less options. Passing `prefix`,
`target` and `enable-multilib` would be good enough.

Still, run `make` and `make install`.

### `gcc` Pass 2

Now that we have a C library, gcc will build better. In a *new* building
directory, configure it *without* `--without-headers`, `--with-newlib`,
 and all the
subproject-disabling parameters, while other parameters stay the same.
You'll still need to apply `with-system-zlib` if you enable multilib.

`make` and `make install`, and your toolchain should be sane enough for AIMv6.

