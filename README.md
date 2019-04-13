# fbputs
```
A program to show unicode strings on the linux framebuffer.

This program is to simulate the "cat" to output file content onto the 
linux framebuffer. Since the repository name "fbcat" has already been 
taken by another project, I have to give my repository another name.
The main purpose of my program is to "puts" strings onto the linux 
framebuffer, so I use "fb" as the prefix of "puts" and name the repository
as "fbputs".

The code in this program can be used as a reference to display unicode characters,
ex. in a GUI.

```

## Features
```
-- Use FreeType2 lib to render fonts
-- Use UTF-16 encoding internally to display unicode characters
-- Does Not reply on linux locale to get decoding unicode characters correctly
-- Support Hard linefeeds, soft linefeeds
-- Support both LF(Linux), CRLF(Windows) end of line
-- Auto detect UTF-8 & UTF-16 encoded files
```

## Cloning
```
git clone https://github.com/grassroot72/fbputs.git
```

## Building
```
make

If you want to see some demo characters of this program, you can add "-D DEMO"
to the CFLAGS in the Makefile,

If you want to see more info of loading unicode files, you can add "-D UNICODE_INFO"
to the CFLAGS in the Makefile,

Of course, you can add both "-D DEMO -D UNICODE_INFO".
```

## Testing
```
./fbputs

If you run this program with no arguments, it will show you some demo contents of the following files:
  -- UTF-8-LF.txt
  -- UTF-8-CRLF.txt
  -- UTF-16-LF.txt
  -- UTF-16-CRLF.txt


./fbputs <filename>
If you run this program with a filename, it will display the file content.
For example: ./fbputs UTF-8-Soft-LF.txt
```

## About Unicode
```
In Asia, most of the time, double width characters are used, UTF-16 encoding 
is more efficient than UTF-8 encoding. It has less memory footprint and use 
less CPU time to process.

Is it possible to create a default UTF-16 encoded linux kernel for Asia countries?
...... Just joking :) 
```
