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

This also allows syscalls on linux, using `!`, with the type, param count and parameters in subsequent cells, though this is so far untested!

Examples are in the examples directory!

Build with CMake as you normally would, zero dependencies or setup or anything needed.
