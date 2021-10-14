# literaltypes
Proof of concept for creating iteratable struct-like objects from user defined literals.

## Motivation

In C++20 there is terse syntax available to create custom types from string literals. 
This allows creation of arbitrary types from string literals at compile time. 

Since one of the long-desired features missing from C++ until this day is support for reflection, using string literals it might be possible to emulate some form of reflection at compile time without resorting to macros.

## Example

```
struct Container
{
    float width;
    float length;
    double height;
    int num_windows;
};

// Create a container like "Container" using a literal
auto c = "float width;"
         "float length;"
         "double height;"
         "int num_windows;"_c;
```

## Issues

Lots of errors while using this implementation will not cause any compiler warning / error. (e.g. using the same name for a type twice)

Values will be default initialized.

Some compilers might not create structs that have the same layout as "real" structs, so the run-time efficiency might be worse than expected.

Please do not use this code for anything other than to satisfy your curiosity.