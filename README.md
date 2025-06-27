# Brainfuck With Threads

Start a thread by using curly braces:

```bf
{
    threaded code goes here
}
```

...and output from a thread using a period, as if outputting to the terminal:

```bf
{
    output ten from this thread
    +++++++++
    .
}
```

... and take input from a thread using a comma, as if taking input from the terminal:

```bf
{
    output ten from this thread
    ++++++++++
    .
}

then take ten from the thread into the current cell
,
```

This also allows syscalls on linux, using `!`, with the type, param count and parameters in subsequent cells!

```bf
cell layout for this
| type | params | type | data | type | data                          | type | data       |
| 5    | 3      | 1    | 2    | 2    | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 2    | address... |

example syscall five
+++++ >
which takes three parameters
+++ >
the first being an integer 2
+ > ++ >
the second being a long 0x0101010101010101
++ > + > + > + > + > + > + > + > + >
the third being a pointer to the sixth cell
++ > &

then go back to first cell and call
<<<<<<<<<<<<<< !
```

...as well as other functions cross-platform using `$`:
```bf
| address of string of library name | address of string of function name | params | type | data |

need to write example for this probably

```

Types of parameters you can send to functions are:
- char (1), takes up one cell
- long (2), takes up four or eight cells depending on platform, used for pointers

Examples are in the examples directory!

Build with CMake as you normally would, zero dependencies or setup or anything needed, but prebuilt binaries are available for Windows in the [releases tab](https://github.com/undefined06855/Brainfuck-With-Threads/releases), if you trust I haven't put malware into them.
