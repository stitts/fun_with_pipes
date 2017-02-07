Redirecting stdout to a buffer:
    
    > g++ fun_with_pipes.cpp
    > ./a.out
    1: writing to stdout
    2: printing 'wow some text' to stdout
    2: buffer contents: '1: wow some text'
    2: closing buffered pipe
    1: printing to stdout again

If you try to write too much without reading you can deadlock.
Pipe capacity: https://linux.die.net/man/7/pipe
