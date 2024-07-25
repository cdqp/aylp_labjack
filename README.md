Anyloop plugin for LabJack devices
==================================

In-progress. We will focus on using I2C to control an LJTick-DAC.

When cloning this repository, make sure to `--recurse-submodules` so you grab
the exodriver dependency.


libaylp dependency
------------------

Symlink or copy the `libaylp` directory from anyloop to `libaylp`. For example:

```sh
ln -s $HOME/git/anyloop/libaylp libaylp
```


exodriver dependency
--------------------

Handled in a git submodule.


Building
--------

Use meson:

```sh
meson setup build
meson compile -C build
```


