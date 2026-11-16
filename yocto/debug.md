---
title: "Yocto debug"
tags: [debug, yocto]
draft: false
date = "2026-11-16"
---

# Yocto Debug

Retrieve the detailed build logs and dependency information.

+ IMAGE_INSTALL: Defines the base packages installed in the target image. By default, the SDK  
  derives the corresponding development headers (-dev) and libraries from these packages.
+ SDKIMAGE_FEATURES: Controls additional features included in the SDK image. Common values include  
  dev-pkgs (includes development headers and static libraries for all installed packages) and  
  dbg-pkgs (includes debugging symbols).
+ TOOLCHAIN_TARGET_TASK: Explicitly specifies additional packages to install into the SDK target  
  sysroot. It is commonly used to manually include specific development packages in the SDK.

```bash
# Export all effective variables
$ bitbake -e -c populate_sdk <image_recipe> > sdk_env.txt
$ grep -E "^(TOOLCHAIN_TARGET_TASK|SDKIMAGE_FEATURES|IMAGE_INSTALL)=" sdk_env.txt

# Record the dnf or opkg installation list during the target sysroot packaging stage.
$ bitbake -v -c populate_sdk <image_recipe> 2>&1 | tee sdk_build.log

# Generate the dependency files pn-buildlist and task-depends.dot.
$ bitbake -g -c populate_sdk <image_recipe>
```

Check whether a package has a runtime dependency on a specific library or package.

```bash
$ grep -E 'libfoo\.do_package' task-depends.dot
...
"bar.do_package" -> "libfoo.do_packagedata"
"bar.do_package_write_rpm" -> "libfoo.do_packagedata"
```

Check the runtime dependencies of a certain recipe.

```bash
$ oe-pkgdata-util read-value RDEPENDS <recipe_name>
```

Check recommended packages during the dnf installation stage.

```
# build/tmp/pkgdata/.../runtime
$ grep -rn "libfoo" bar*
...
bar-dev:11:RRECOMMENDS:bar-dev: ... foo-dev ...
...
```

Check the final resolved value of the recipe's `PACKAGECONFIG` variable and the complete history of  
how it was modified across configuration files.

```bash
$ bitbake-getvar -r <recipe> PACKAGECONFIG
```
