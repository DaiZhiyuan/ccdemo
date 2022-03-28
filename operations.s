@ Copyright (c) 2010 ARM Ltd
@
@ Permission is hereby granted, free of charge, to any person obtaining a copy
@ of this software and associated documentation files (the "Software"), to deal
@ in the Software without restriction, including without limitation the rights
@ to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
@ copies of the Software, and to permit persons to whom the Software is
@ furnished to do so, subject to the following conditions:
@
@ The above copyright notice and this permission notice shall be included in
@ all copies or substantial portions of the Software.
@
@ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
@ IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
@ FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
@ AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
@ LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
@ OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
@ THE SOFTWARE.
@
@ --------------------------------

    .syntax unified

@ The prototype of each of these functions looks like this:
@ extern int32_t do<op>(int32_t arg1, int32_t arg2, uint32_t * flags);

@ The EABI procedure-call standard specifies that arguments go in registers
@ r0-r3 if possible (and that we can clobber those registers if we need to).
@
@ r0: arg1
@ r1: arg2
@ r2: &flags
@
@ r0 is also used to return the result.

    @ --------------------------------
    .global doADDS
doADDS:
    @ Perform the operation, leaving the result in r0 to return it.
    adds    r0, r0, r1

    @ Get the APSR flags and dump them into flags (*r2).
    mrs     r3, CPSR
    str     r3, [r2]

    @ The recommended (interworking-compatible) return method:
    bx      lr

    @ --------------------------------
    .global doSUBS
doSUBS:
    subs    r0, r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

    @ --------------------------------
    .global doANDS
doANDS:
    ands    r0, r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

    @ --------------------------------
    .global doORRS
doORRS:
    orrs    r0, r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

    @ --------------------------------
    .global doEORS
doEORS:
    eors    r0, r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

    @ --------------------------------
    .global doBICS
doBICS:
    bics    r0, r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

    @ --------------------------------
    .global doASRS
doASRS:
    asrs    r0, r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

    @ --------------------------------
    .global doLSLS
doLSLS:
    lsls    r0, r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

    @ --------------------------------
    .global doLSRS
doLSRS:
    lsrs    r0, r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

    @ --------------------------------
    .global doRORS
doRORS:
    rors    r0, r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

    @ --------------------------------
    .global doMULS
doMULS:
    muls    ip, r0, r1
    mov     r0, ip
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

@ The following operations return no result, and look like this:
@ extern void do<op>(int32_t arg1, int32_t arg2, uint32_t * flags);

    @ --------------------------------
    .global doCMP
doCMP:
    cmp     r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

    @ --------------------------------
    .global doCMN
doCMN:
    cmn     r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

    @ --------------------------------
    .global doTST
doTST:
    tst     r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

    @ --------------------------------
    .global doTEQ
doTEQ:
    teq     r0, r1
    mrs     r3, CPSR
    str     r3, [r2]
    bx      lr

@ --------------------------------
@ uint32_t tryConditionCodes(uint32_t apsr);
@
@ Each result bit represents a condition code:
@   EQ (0x0) is bit 0.
@   NE (0x1) is bit 1.
@   CS (0x2) is bit 2.
@   ... etc ...
@
    .global tryConditionCodes
tryConditionCodes:
    @ Manually write the specified condition flags into the APSR.
    msr     CPSR_f, r0
    @ Initialize the result.
    mov     r0, #0

    @ Execute conditional instructions to populate the result. The IT
    @ instructions are for Thumb-2 compatibility and do not generate any code
    @ in ARM mode. See "Condition Codes 3: Conditional Execution in Thumb-2"
    @ for an explanation.
    ite     eq
    orreq   r0, #(1<<0x0)
    orrne   r0, #(1<<0x1)
    ite     cs
    orrcs   r0, #(1<<0x2)
    orrcc   r0, #(1<<0x3)
    ite     mi
    orrmi   r0, #(1<<0x4)
    orrpl   r0, #(1<<0x5)
    ite     vs
    orrvs   r0, #(1<<0x6)
    orrvc   r0, #(1<<0x7)
    ite     hi
    orrhi   r0, #(1<<0x8)
    orrls   r0, #(1<<0x9)
    ite     ge
    orrge   r0, #(1<<0xa)
    orrlt   r0, #(1<<0xb)
    ite     gt
    orrgt   r0, #(1<<0xc)
    orrle   r0, #(1<<0xd)

    @ The standard interworking return sequence.
    bx      lr
