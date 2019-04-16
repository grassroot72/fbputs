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

Currently, fbputs can only display unicode files with Byte Order Mark (BOM).

```

## Features
```
-- Use FreeType2 lib to render fonts
-- Use UTF-16 encoding internally to display unicode characters
-- Does Not reply on linux locale to get decoding unicode characters correctly
-- Support Hard linefeeds, soft linefeeds
-- Support both LF(Linux), CRLF(Windows) end of line
-- Auto detect UTF-8 & UTF-16 encoded files (with BOM)
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
To run fbputs, you need to temporarily exit the X windows desktop and enter the
console by pressing CTRL + ALT + F2.

./fbputs
If you build with "-D DEMO" and run this program with no arguments, it will show 
you some demo contents of the following files:
  -- UTF-8-LF.txt
  -- UTF-8-CRLF.txt
  -- UTF-16-LF.txt
  -- UTF-16-CRLF.txt


./fbputs <filename>
If you run this program with a filename, it will display the file content.
For example: ./fbputs UTF-8-Soft-LF.txt

You may encounter "failed to initialize the framebuffer" problem, which means you
don't have the authorization to open framebuffer device. You can do either the
followings to solve the issue:

(a). add your user to video group (sudo gpasswd -a <your user> video), then
     run 'fbputs' by your own user
(b). use root to run 'fbputs'
```

## About Unicode
```
In Asia, most of the time, double width characters are used, UTF-16 encoding 
is more efficient than UTF-8 encoding. It has less memory footprint and use 
less CPU time to process.

Is it possible to create a default UTF-16 encoded linux kernel for Asia countries?
...... Just joking :) 
```
