Copyright (c) 2010 ARM Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

--------------------------------

This bundle includes a Linux demonstration program which executes a specified
ARM instruction and presents the result along with the state of the APSR (or
CPSR) condition flags and a list of conditional execution codes which match.

Usage: ./ccdemo op arg1 arg2
Arguments:
    op: One of the following ARM instructions:
            adds
            subs
            ands
            orrs
            eors
            bics
            asrs
            lsls
            lsrs
            rors
            muls
            cmp
            cmn
            tst
            teq
  arg1: The first operand, either as an unsigned hexadecimal
        value, unsigned octal or signed decimal.
  arg2: The second operand, in the same format as arg0.


----

It should be possible to build the program by running "build.sh". You'll need
GCC to do this. In addition, if you're cross-compiling, you'll need to edit the
build script to point to whichever compiler you're using. (This is likely to be
something like "arm-none-linux-gnueabi-gcc".)
