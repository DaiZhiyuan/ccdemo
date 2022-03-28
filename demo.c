// Copyright (c) 2010 ARM Ltd
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// --------------------------------
//
// This utility takes two input arguments, performs the selected operation,
// then prints the result, along with information about which flags were set
// and the effect that this has on the conditional execution flags.
//
// Usage: ccdemo op arg1 arg2
// Use 'ccdemo' with no arguments to show detailed usage information.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern int32_t doADDS(int32_t arg1, int32_t arg2, uint32_t * flags);
extern int32_t doSUBS(int32_t arg1, int32_t arg2, uint32_t * flags);
extern int32_t doANDS(int32_t arg1, int32_t arg2, uint32_t * flags);
extern int32_t doORRS(int32_t arg1, int32_t arg2, uint32_t * flags);
extern int32_t doEORS(int32_t arg1, int32_t arg2, uint32_t * flags);
extern int32_t doBICS(int32_t arg1, int32_t arg2, uint32_t * flags);
extern int32_t doASRS(int32_t arg1, int32_t arg2, uint32_t * flags);
extern int32_t doLSLS(int32_t arg1, int32_t arg2, uint32_t * flags);
extern int32_t doLSRS(int32_t arg1, int32_t arg2, uint32_t * flags);
extern int32_t doRORS(int32_t arg1, int32_t arg2, uint32_t * flags);
extern int32_t doMULS(int32_t arg1, int32_t arg2, uint32_t * flags);
extern void    doCMP (int32_t arg1, int32_t arg2, uint32_t * flags);
extern void    doCMN (int32_t arg1, int32_t arg2, uint32_t * flags);
extern void    doTST (int32_t arg1, int32_t arg2, uint32_t * flags);
extern void    doTEQ (int32_t arg1, int32_t arg2, uint32_t * flags);

extern uint32_t tryConditionCodes(uint32_t apsr);

#define BIT(word,bit)   ((((uint32_t)word) >> (bit)) & 1)

void usage(char const * app_name)
{
    printf("Usage: %s op arg1 arg2\n", app_name);
    printf("Arguments:\n");
    printf("    op: One of the following ARM instructions:\n");
    printf("            adds\n");
    printf("            subs\n");
    printf("            ands\n");
    printf("            orrs\n");
    printf("            eors\n");
    printf("            bics\n");
    printf("            asrs\n");
    printf("            lsls\n");
    printf("            lsrs\n");
    printf("            rors\n");
    printf("            muls\n");
    printf("            cmp\n");
    printf("            cmn\n");
    printf("            tst\n");
    printf("            teq\n");
    printf("  arg1: The first operand, either as an unsigned hexadecimal\n");
    printf("        (with prefix '0x'), unsigned octal (with prefix '0'),\n");
    printf("        or signed decimal value.\n");
    printf("  arg2: The second operand, in the same format as arg1.\n");
}

// A rather crude (but simple) integer parser, supporting unsigned hexadecimal
// and octal and signed decimal inputs.
int32_t parse_int(char const * arg)
{
    if (arg[0] == '0') {
        // The input is either octal or hexadecimal.
        if ((arg[1] == 'x') || (arg[1] == 'X')) {
            // Hexadecimal (unsigned).
            return (int32_t)strtoul(arg, NULL, 16);
        } else {
            // Octal (unsigned).
            return (int32_t)strtoul(arg, NULL, 8);
        }
    } else {
        // The input is assumed to be signed decimal.
        return (int32_t)strtol(arg, NULL, 10);
    }
}

int main(int argc, char * argv[])
{
    char const *    op;
    int32_t         arg1;
    int32_t         arg2;
    int32_t         result;
    uint32_t        apsr;   // Bits [31:28] are (respectively): N,Z,C,V
    // Each bit represents a condition code:
    //   EQ (0x0) is bit 0.
    //   NE (0x1) is bit 1.
    //   CS (0x2) is bit 2.
    //   ... etc ...
    uint32_t        cc;

    enum {
        ARITHMETIC,
        LOGICAL,
        MULTIPLICATION,
    } type;

    // For pure flag-setting instructions (such as cmp), indicate the
    // equivalent operation that also stores the result (or leave as 0 if the
    // selected operation stores a result anyway).
    char const *    op_equiv = 0;

    // Check that we got the right number of arguments.
    if (argc != 4) {
        usage(argv[0]);
        return -1;
    }

    op = argv[1];

    // Perform some basic number parsing of the operands.
    arg1 = parse_int(argv[2]);
    arg2 = parse_int(argv[3]);

    // React based on the selected operation.
    if (strcmp(op, "adds") == 0) {
        result = doADDS(arg1, arg2, &apsr);
        type=ARITHMETIC;
    } else if (strcmp(op, "subs") == 0) {
        result = doSUBS(arg1, arg2, &apsr);
        type=ARITHMETIC;
    } else if (strcmp(op, "ands") == 0) {
        result = doANDS(arg1, arg2, &apsr);
        type=LOGICAL;
    } else if (strcmp(op, "orrs") == 0) {
        result = doORRS(arg1, arg2, &apsr);
        type=LOGICAL;
    } else if (strcmp(op, "eors") == 0) {
        result = doEORS(arg1, arg2, &apsr);
        type=LOGICAL;
    } else if (strcmp(op, "bics") == 0) {
        result = doBICS(arg1, arg2, &apsr);
        type=LOGICAL;
    } else if (strcmp(op, "asrs") == 0) {
        result = doASRS(arg1, arg2, &apsr);
        type=LOGICAL;
    } else if (strcmp(op, "lsls") == 0) {
        result = doLSLS(arg1, arg2, &apsr);
        type=LOGICAL;
    } else if (strcmp(op, "lsrs") == 0) {
        result = doLSRS(arg1, arg2, &apsr);
        type=LOGICAL;
    } else if (strcmp(op, "rors") == 0) {
        result = doRORS(arg1, arg2, &apsr);
        type=LOGICAL;
    } else if (strcmp(op, "muls") == 0) {
        result = doMULS(arg1, arg2, &apsr);
        type=MULTIPLICATION;
    } else if (strcmp(op, "cmp") == 0) {
        doCMP(arg1, arg2, &apsr);
        type=ARITHMETIC;
        op_equiv = "subs";
    } else if (strcmp(op, "cmn") == 0) {
        doCMN(arg1, arg2, &apsr);
        type=ARITHMETIC;
        op_equiv = "adds";
    } else if (strcmp(op, "tst") == 0) {
        doTST(arg1, arg2, &apsr);
        type=LOGICAL;
        op_equiv = "ands";
    } else if (strcmp(op, "teq") == 0) {
        doTEQ(arg1, arg2, &apsr);
        type=LOGICAL;
        op_equiv = "eors";
    } else {
        printf("Unknown operation (%s).\n", op);
        return -1;
    }

    // Get the condition code flag array by conditionally executing bit-setting
    // instructions.
    cc = tryConditionCodes(apsr);

    // Print the results and exit.
    if (!op_equiv) {
        printf("The results (in various formats):\n");
        printf("       Signed: %10d %s %10d = %10d\n", arg1, op, arg2, result);
        printf("     Unsigned: %10u %s %10u = %10u\n", (uint32_t)arg1, op, (uint32_t)arg2, (uint32_t)result);
        printf("  Hexadecimal: 0x%08x %s 0x%08x = 0x%08x\n", (uint32_t)arg1, op, (uint32_t)arg2, (uint32_t)result);
    } else {
        printf(" +--------------------------------\n");
        printf(" | Note that '%s' does not store a result,\n", op);
        printf(" | but sets the same flags as '%s'.\n", op_equiv);
        printf(" +--------------------------------\n");
    }
    printf("Flags:\n");
    switch (type) {
      case ARITHMETIC:
        printf("  N (negative): %u\n", BIT(apsr,31));
        printf("  Z (zero)    : %u\n", BIT(apsr,30));
        printf("  C (carry)   : %u\n", BIT(apsr,29));
        printf("  V (overflow): %u\n", BIT(apsr,28));
        printf("Condition Codes:\n");
        printf("  EQ: %u    NE: %u\n", BIT(cc,0x0), BIT(cc,0x1));
        printf("  CS: %u    CC: %u\n", BIT(cc,0x2), BIT(cc,0x3));
        printf("  MI: %u    PL: %u\n", BIT(cc,0x4), BIT(cc,0x5));
        printf("  VS: %u    VC: %u\n", BIT(cc,0x6), BIT(cc,0x7));
        printf("  HI: %u    LS: %u\n", BIT(cc,0x8), BIT(cc,0x9));
        printf("  GE: %u    LT: %u\n", BIT(cc,0xa), BIT(cc,0xb));
        printf("  GT: %u    LE: %u\n", BIT(cc,0xc), BIT(cc,0xd));
        break;
      case LOGICAL:
        printf("  N (negative): %u\n", BIT(apsr,31));
        printf("  Z (zero)    : %u\n", BIT(apsr,30));
        printf("  C (carry)   : %u\n", BIT(apsr,29));
        printf("  V (overflow): Unchanged by '%s'.\n", op);
        printf("Condition Codes:\n");
        printf("  EQ: %u    NE: %u\n", BIT(cc,0x0), BIT(cc,0x1));
        printf("  CS: %u    CC: %u\n", BIT(cc,0x2), BIT(cc,0x3));
        printf("  MI: %u    PL: %u\n", BIT(cc,0x4), BIT(cc,0x5));
        printf("  HI: %u    LS: %u\n", BIT(cc,0x8), BIT(cc,0x9));
        printf(" +--------------------\n");
        printf(" | Note that '%s' does not set the 'V' flag, so condition codes\n", op);
        printf(" | that depend on it have not been shown.\n");
        printf(" +--------------------\n");
        break;
      case MULTIPLICATION:
        printf("  N (negative): %u\n", BIT(apsr,31));
        printf("  Z (zero)    : %u\n", BIT(apsr,30));
        printf("  C (carry)   : Unchanged by '%s'.\n", op);
        printf("  V (overflow): Unchanged by '%s'.\n", op);
        printf("Condition Codes:\n");
        printf("  EQ: %u    NE: %u\n", BIT(cc,0x0), BIT(cc,0x1));
        printf("  MI: %u    PL: %u\n", BIT(cc,0x4), BIT(cc,0x5));
        printf(" +--------------------\n");
        printf(" | Note that '%s' does not set the 'V' or 'C' flags, so condition\n", op);
        printf(" | codes that depend on them have not been shown.\n");
        printf(" +--------------------\n");
        break;
    }

    return 0;
}
