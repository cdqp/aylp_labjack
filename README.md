Anyloop plugin for LabJack devices
==================================

Supports the LabJack U3 only for now, but the code tries to be as generic as
possible.

When cloning this repository, make sure to `--recurse-submodules` so you grab
the exodriver dependency.


aylp_ljtdac.so
--------------

Types and units: `[T_VECTOR, U_V] -> [T_UNCHANGED, U_UNCHANGED]`.

This device interprets the state vector as a pair of voltages, writing them to
an LJTick-DAC connected to a LabJack. The first voltage is written to DACA, and
the second to DACB. The LJTick-DAC is assumed to be connected to pins FIO5 and
FIO4 on the LabJack.

### Parameters

- `host` (string) (required)
  - The model name of the LabJack. Must be "U3" for now.
- `square_hz` (integer) (optional)
  - Frequency in Hz to optionally clock FIO6 with a square wave at.
- `fast` (boolean) (optional)
  - Whether or not to skip the `LJUSB_Read` call after writing each voltage,
    roughly cutting latency in half. Might break things! Defaults to false.


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


