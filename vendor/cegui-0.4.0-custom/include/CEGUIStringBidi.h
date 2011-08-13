/************************************************************************
 * $Id$
 *
 * ------------
 * Description:
 * ------------
 * This is an implemention of Unicode's Bidirectional Algorithm
 * (known as UAX #9).
 *
 *   http://www.unicode.org/reports/tr9/
 * 
 * Author: Ahmad Khalifa
 *
 * -----------------
 * Revision Details:    (Updated by Revision Control System)
 * -----------------
 *  $Date$
 *  $Author$
 *  $Revision$
 *  $Source$
 *
 * (www.arabeyes.org - under MIT license)
 *
 ************************************************************************/
#ifndef _MINIBIDI_h_
#define _MINIBIDI_h_

#include "CEGUIBase.h"
 
#include <malloc.h>		/* malloc() and free() definition */
#ifdef _LINUX
#include <wchar.h>
#endif

/*
 * Datatype Extension Macros
 */
#define BLOCKTYPE CEGUI::utf32 *
#define CHARTYPE CEGUI::utf32
#define GETCHAR(from,x) from[x]

#define GetType(x) getType(x)
/*=====TESTING mode===========*/
//#define GetType(x) getCAPRtl(x)

#define lenof(x) sizeof(x) / sizeof(x[0])
#define MAX_STACK 60

/* character types */
enum
{
  /* Strong Char Types */
  L,		/* Left-to-Right char */
  LRE,	/* Left-to-Right Embedding */
  LRO,	/* Left-to-Right Override */
  R,		/* Right-to-Left char */
  AL,		/* Right-to-Left Arabic char */
  RLE,	/* Right-to-Left Embedding */
  RLO,	/* Right-to-Left Override */
	/* Weak Char Types */
  PDF,	/* Pop Directional Format */
  EN,		/* European Number */
  ES,		/* European Number Separator */
  ET,		/* European Number Terminator */
  AN,		/* Arabic Number */
  CS,		/* Common Number Separator */
  NSM,	/* Non Spacing Mark */
  BN,		/* Boundary Neutral */
  /* Neutral Char Types */
  B,		/* Paragraph Separator */
  S,		/* Segment Separator */
  WS,		/* Whitespace */
  ON,		/* Other Neutrals */
};

/* Shaping Types */
enum
{
  SL, /* Left-Joining, doesnt exist in U+0600 - U+06FF */
  SR, /* Right-Joining, ie has Isolated, Final */
  SD, /* Dual-Joining, ie has Isolated, Final, Initial, Medial */
  SU, /* Non-Joining */
  SC  /* Join-Causing, like U+0640 (TATWEEL) */
};

#define odd(x) (x%2)
/* Returns the first odd/even value greater than x */
#define leastGreaterOdd(x) odd(x) ? (x+2) : (x+1)
#define leastGreaterEven(x) odd(x) ? (x+1) : (x+2)

/* Shaping Helpers */
#define STYPE(xh) ((xh >= SHAPE_FIRST) && (xh <= SHAPE_LAST)) ? \
                   shapetypes[xh-SHAPE_FIRST].type : SU
#define SISOLATED(xh) shapetypes[xh-SHAPE_FIRST].form_b
#define SFINAL(xh) xh+1
#define SINITIAL(xh) xh+2
#define SMEDIAL(xh) xh+3


/* function declarations */
int findIndexOfRun(unsigned char* level , int start, int count, int tlevel);
unsigned char getType(CHARTYPE ch);
unsigned char getCAPRtl(CHARTYPE ch);
void doMirror(CHARTYPE* ch);

typedef struct{
	unsigned char type;
	unsigned short form_b;
} shape_node;

/* Kept near the actual table, for verification. */
#define SHAPE_FIRST 0x621
#define SHAPE_LAST 0x64A
/* very bad Memory alignment for 32-bit machines
 * could split it to 2 arrays or promote type to 2 bytes type
 */
shape_node shapetypes[] = {
/* index, Typ, Iso */
/* 621 */ {SU, 0xFE80},
/* 622 */ {SR, 0xFE81},
/* 623 */ {SR, 0xFE83},
/* 624 */ {SR, 0xFE85},
/* 625 */ {SR, 0xFE87},
/* 626 */ {SD, 0xFE89},
/* 627 */ {SR, 0xFE8D},
/* 628 */ {SD, 0xFE8F},
/* 629 */ {SR, 0xFE93},
/* 62A */ {SD, 0xFE95},
/* 62B */ {SD, 0xFE99},
/* 62C */ {SD, 0xFE9D},
/* 62D */ {SD, 0xFEA1},
/* 62E */ {SD, 0xFEA5},
/* 62F */ {SR, 0xFEA9},
/* 630 */ {SR, 0xFEAB},
/* 631 */ {SR, 0xFEAD},
/* 632 */ {SR, 0xFEAF},
/* 633 */ {SD, 0xFEB1},
/* 634 */ {SD, 0xFEB5},
/* 635 */ {SD, 0xFEB9},
/* 636 */ {SD, 0xFEBD},
/* 637 */ {SD, 0xFEC1},
/* 638 */ {SD, 0xFEC5},
/* 639 */ {SD, 0xFEC9},
/* 63A */ {SD, 0xFECD},
/* 63B */ {SU, 0x0},
/* 63C */ {SU, 0x0},
/* 63D */ {SU, 0x0},
/* 63E */ {SU, 0x0},
/* 63F */ {SU, 0x0},
/* 640 */ {SC, 0x0},
/* 641 */ {SD, 0xFED1},
/* 642 */ {SD, 0xFED5},
/* 643 */ {SD, 0xFED9},
/* 644 */ {SD, 0xFEDD},
/* 645 */ {SD, 0xFEE1},
/* 646 */ {SD, 0xFEE5},
/* 647 */ {SD, 0xFEE9},
/* 648 */ {SR, 0xFEED},
/* 649 */ {SR, 0xFEEF}, /* SD */
/* 64A */ {SD, 0xFEF1}
};

/*
 * Flips the text buffer, according to max level, and
 * all higher levels
 * 
 * Input:
 * from: text buffer, on which to apply flipping
 * level: resolved levels buffer
 * max: the maximum level found in this line (should be unsigned char)
 * count: line size in wchar_t
 */
void flipThisRun(BLOCKTYPE &from, unsigned char* level, int max, int count)
{
   int i, j, rcount, tlevel;
   CHARTYPE temp;

   j = i = 0;
   while(i<count && j<count)
   {
      /* find the start of the run of level=max */
      tlevel = max;
      i = j = findIndexOfRun(level, i, count, max);
      /* find the end of the run */
      while((tlevel <= level[i]) && (i < count))
      {
	 i++;
      }
      rcount = i-j;
      for(; rcount>((i-j)/2); rcount--)
      {
	 temp = GETCHAR(from, j+rcount-1);
	 GETCHAR(from, j+rcount-1) = GETCHAR(from, i-rcount);
	 GETCHAR(from, i-rcount) = temp;
      }
   }
}

/*
 * Finds the index of a run with level equals tlevel
 */
int findIndexOfRun(unsigned char* level , int start, int count, int tlevel)
{
   int i;
   for(i=start; i<count; i++)
   {
      if(tlevel <= level[i])
      {
	 return i;
      }
   }
   return count;
}

unsigned char GetParagraphLevel(BLOCKTYPE &line, int count)
{
	int i;
	for( i=0; i<count ; i++)
	{
		if(GetType(GETCHAR(line, i)) == R || GetType(/*line[i]*/ GETCHAR(line, i)) == AL)
			return 1;
		else if(GetType(GETCHAR(line, i)) == L)
			return 0;
	}
	return 0;		/* Compiler Nag-Stopper */
}

/*
 * Returns character type of ch, by calling RLE table lookup
 * function
 */
unsigned char getCAPRtl(CHARTYPE ch)
{
/* CAPRtl Charset */
int TypesFromChar[]  =
{
//0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
  ON, ON, ON, ON,  L,  R, ON, ON, ON, ON, ON, ON, ON, B, RLO,RLE, /*00-0f*/
  LRO,LRE,PDF,WS, ON, ON, ON, ON, ON, ON, ON, ON, ON, ON, ON, ON, /*10-1f*/
  WS, ON, ON, ON, ET, ON, ON, ON, ON, ON, ON, ET, CS, ON, ES, ES, /*20-2f*/
  EN, EN, EN, EN, EN, EN, AN, AN, AN, AN, LRE, RLE, RLO, PDF, LRO, ON, /*30-3f*/
//  EN, EN, EN, EN, EN, EN, AN, AN, AN, AN, CS, ON, ON, ON, ON, ON, /*30-3f*/
  R, AL, AL, AL, AL, AL, AL,  R,  R,  R,  R,  R,  R,  R,  R,  R, /*40-4f*/
  R,  R,  R,  R,  R,  R,  R,  R,  R,  R,  R, ON,  B, ON, ON, ON, /*50-5f*/
  NSM, L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L, /*60-6f*/
  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L, ON,  S, ON, ON, ON, /*70-7f*/
};

 if(ch <0x7f)
   return TypesFromChar[ch];
 else 
   return R;
}

unsigned char getType(CHARTYPE ch)
{
    static const struct {
	CHARTYPE first, last, type;
    } lookup[] = {
        {0x0000, 0x0008, BN},
        {0x0009, 0x0009, S},
        {0x000a, 0x000a, B},
        {0x000b, 0x000b, S},
        {0x000c, 0x000c, WS},
        {0x000d, 0x000d, B},
        {0x000e, 0x001b, BN},
        {0x001c, 0x001e, B},
        {0x001f, 0x001f, S},
        {0x0020, 0x0020, WS},
        {0x0023, 0x0025, ET},
        {0x002b, 0x002b, ES},
        {0x002c, 0x002c, CS},
        {0x002d, 0x002d, ES},
        {0x002e, 0x002f, CS},
        {0x0030, 0x0039, EN},
        {0x003a, 0x003a, CS},
        {0x0041, 0x005a, L},
        {0x0061, 0x007a, L},
        {0x007f, 0x0084, BN},
        {0x0085, 0x0085, B},
        {0x0086, 0x009f, BN},
        {0x00a0, 0x00a0, CS},
        {0x00a2, 0x00a5, ET},
        {0x00aa, 0x00aa, L},
        {0x00ad, 0x00ad, BN},
        {0x00b0, 0x00b1, ET},
        {0x00b2, 0x00b3, EN},
        {0x00b5, 0x00b5, L},
        {0x00b9, 0x00b9, EN},
        {0x00ba, 0x00ba, L},
        {0x00c0, 0x00d6, L},
        {0x00d8, 0x00f6, L},
        {0x00f8, 0x0236, L},
        {0x0250, 0x02b8, L},
        {0x02bb, 0x02c1, L},
        {0x02d0, 0x02d1, L},
        {0x02e0, 0x02e4, L},
        {0x02ee, 0x02ee, L},
        {0x0300, 0x0357, NSM},
        {0x035d, 0x036f, NSM},
        {0x037a, 0x037a, L},
        {0x0386, 0x0386, L},
        {0x0388, 0x038a, L},
        {0x038c, 0x038c, L},
        {0x038e, 0x03a1, L},
        {0x03a3, 0x03ce, L},
        {0x03d0, 0x03f5, L},
        {0x03f7, 0x03fb, L},
        {0x0400, 0x0482, L},
        {0x0483, 0x0486, NSM},
        {0x0488, 0x0489, NSM},
        {0x048a, 0x04ce, L},
        {0x04d0, 0x04f5, L},
        {0x04f8, 0x04f9, L},
        {0x0500, 0x050f, L},
        {0x0531, 0x0556, L},
        {0x0559, 0x055f, L},
        {0x0561, 0x0587, L},
        {0x0589, 0x0589, L},
        {0x0591, 0x05a1, NSM},
        {0x05a3, 0x05b9, NSM},
        {0x05bb, 0x05bd, NSM},
        {0x05be, 0x05be, R},
        {0x05bf, 0x05bf, NSM},
        {0x05c0, 0x05c0, R},
        {0x05c1, 0x05c2, NSM},
        {0x05c3, 0x05c3, R},
        {0x05c4, 0x05c4, NSM},
        {0x05d0, 0x05ea, R},
        {0x05f0, 0x05f4, R},
        {0x0600, 0x0603, AL},
        {0x060c, 0x060c, CS},
        {0x060d, 0x060d, AL},
        {0x0610, 0x0615, NSM},
        {0x061b, 0x061b, AL},
        {0x061f, 0x061f, AL},
        {0x0621, 0x063a, AL},
        {0x0640, 0x064a, AL},
        {0x064b, 0x0658, NSM},
        {0x0660, 0x0669, AN},
        {0x066a, 0x066a, ET},
        {0x066b, 0x066c, AN},
        {0x066d, 0x066f, AL},
        {0x0670, 0x0670, NSM},
        {0x0671, 0x06d5, AL},
        {0x06d6, 0x06dc, NSM},
        {0x06dd, 0x06dd, AL},
        {0x06de, 0x06e4, NSM},
        {0x06e5, 0x06e6, AL},
        {0x06e7, 0x06e8, NSM},
        {0x06ea, 0x06ed, NSM},
        {0x06ee, 0x06ef, AL},
        {0x06f0, 0x06f9, EN},
        {0x06fa, 0x070d, AL},
        {0x070f, 0x070f, BN},
        {0x0710, 0x0710, AL},
        {0x0711, 0x0711, NSM},
        {0x0712, 0x072f, AL},
        {0x0730, 0x074a, NSM},
        {0x074d, 0x074f, AL},
        {0x0780, 0x07a5, AL},
        {0x07a6, 0x07b0, NSM},
        {0x07b1, 0x07b1, AL},
        {0x0901, 0x0902, NSM},
        {0x0903, 0x0939, L},
        {0x093c, 0x093c, NSM},
        {0x093d, 0x0940, L},
        {0x0941, 0x0948, NSM},
        {0x0949, 0x094c, L},
        {0x094d, 0x094d, NSM},
        {0x0950, 0x0950, L},
        {0x0951, 0x0954, NSM},
        {0x0958, 0x0961, L},
        {0x0962, 0x0963, NSM},
        {0x0964, 0x0970, L},
        {0x0981, 0x0981, NSM},
        {0x0982, 0x0983, L},
        {0x0985, 0x098c, L},
        {0x098f, 0x0990, L},
        {0x0993, 0x09a8, L},
        {0x09aa, 0x09b0, L},
        {0x09b2, 0x09b2, L},
        {0x09b6, 0x09b9, L},
        {0x09bc, 0x09bc, NSM},
        {0x09bd, 0x09c0, L},
        {0x09c1, 0x09c4, NSM},
        {0x09c7, 0x09c8, L},
        {0x09cb, 0x09cc, L},
        {0x09cd, 0x09cd, NSM},
        {0x09d7, 0x09d7, L},
        {0x09dc, 0x09dd, L},
        {0x09df, 0x09e1, L},
        {0x09e2, 0x09e3, NSM},
        {0x09e6, 0x09f1, L},
        {0x09f2, 0x09f3, ET},
        {0x09f4, 0x09fa, L},
        {0x0a01, 0x0a02, NSM},
        {0x0a03, 0x0a03, L},
        {0x0a05, 0x0a0a, L},
        {0x0a0f, 0x0a10, L},
        {0x0a13, 0x0a28, L},
        {0x0a2a, 0x0a30, L},
        {0x0a32, 0x0a33, L},
        {0x0a35, 0x0a36, L},
        {0x0a38, 0x0a39, L},
        {0x0a3c, 0x0a3c, NSM},
        {0x0a3e, 0x0a40, L},
        {0x0a41, 0x0a42, NSM},
        {0x0a47, 0x0a48, NSM},
        {0x0a4b, 0x0a4d, NSM},
        {0x0a59, 0x0a5c, L},
        {0x0a5e, 0x0a5e, L},
        {0x0a66, 0x0a6f, L},
        {0x0a70, 0x0a71, NSM},
        {0x0a72, 0x0a74, L},
        {0x0a81, 0x0a82, NSM},
        {0x0a83, 0x0a83, L},
        {0x0a85, 0x0a8d, L},
        {0x0a8f, 0x0a91, L},
        {0x0a93, 0x0aa8, L},
        {0x0aaa, 0x0ab0, L},
        {0x0ab2, 0x0ab3, L},
        {0x0ab5, 0x0ab9, L},
        {0x0abc, 0x0abc, NSM},
        {0x0abd, 0x0ac0, L},
        {0x0ac1, 0x0ac5, NSM},
        {0x0ac7, 0x0ac8, NSM},
        {0x0ac9, 0x0ac9, L},
        {0x0acb, 0x0acc, L},
        {0x0acd, 0x0acd, NSM},
        {0x0ad0, 0x0ad0, L},
        {0x0ae0, 0x0ae1, L},
        {0x0ae2, 0x0ae3, NSM},
        {0x0ae6, 0x0aef, L},
        {0x0af1, 0x0af1, ET},
        {0x0b01, 0x0b01, NSM},
        {0x0b02, 0x0b03, L},
        {0x0b05, 0x0b0c, L},
        {0x0b0f, 0x0b10, L},
        {0x0b13, 0x0b28, L},
        {0x0b2a, 0x0b30, L},
        {0x0b32, 0x0b33, L},
        {0x0b35, 0x0b39, L},
        {0x0b3c, 0x0b3c, NSM},
        {0x0b3d, 0x0b3e, L},
        {0x0b3f, 0x0b3f, NSM},
        {0x0b40, 0x0b40, L},
        {0x0b41, 0x0b43, NSM},
        {0x0b47, 0x0b48, L},
        {0x0b4b, 0x0b4c, L},
        {0x0b4d, 0x0b4d, NSM},
        {0x0b56, 0x0b56, NSM},
        {0x0b57, 0x0b57, L},
        {0x0b5c, 0x0b5d, L},
        {0x0b5f, 0x0b61, L},
        {0x0b66, 0x0b71, L},
        {0x0b82, 0x0b82, NSM},
        {0x0b83, 0x0b83, L},
        {0x0b85, 0x0b8a, L},
        {0x0b8e, 0x0b90, L},
        {0x0b92, 0x0b95, L},
        {0x0b99, 0x0b9a, L},
        {0x0b9c, 0x0b9c, L},
        {0x0b9e, 0x0b9f, L},
        {0x0ba3, 0x0ba4, L},
        {0x0ba8, 0x0baa, L},
        {0x0bae, 0x0bb5, L},
        {0x0bb7, 0x0bb9, L},
        {0x0bbe, 0x0bbf, L},
        {0x0bc0, 0x0bc0, NSM},
        {0x0bc1, 0x0bc2, L},
        {0x0bc6, 0x0bc8, L},
        {0x0bca, 0x0bcc, L},
        {0x0bcd, 0x0bcd, NSM},
        {0x0bd7, 0x0bd7, L},
        {0x0be7, 0x0bf2, L},
        {0x0bf9, 0x0bf9, ET},
        {0x0c01, 0x0c03, L},
        {0x0c05, 0x0c0c, L},
        {0x0c0e, 0x0c10, L},
        {0x0c12, 0x0c28, L},
        {0x0c2a, 0x0c33, L},
        {0x0c35, 0x0c39, L},
        {0x0c3e, 0x0c40, NSM},
        {0x0c41, 0x0c44, L},
        {0x0c46, 0x0c48, NSM},
        {0x0c4a, 0x0c4d, NSM},
        {0x0c55, 0x0c56, NSM},
        {0x0c60, 0x0c61, L},
        {0x0c66, 0x0c6f, L},
        {0x0c82, 0x0c83, L},
        {0x0c85, 0x0c8c, L},
        {0x0c8e, 0x0c90, L},
        {0x0c92, 0x0ca8, L},
        {0x0caa, 0x0cb3, L},
        {0x0cb5, 0x0cb9, L},
        {0x0cbc, 0x0cbc, NSM},
        {0x0cbd, 0x0cc4, L},
        {0x0cc6, 0x0cc8, L},
        {0x0cca, 0x0ccb, L},
        {0x0ccc, 0x0ccd, NSM},
        {0x0cd5, 0x0cd6, L},
        {0x0cde, 0x0cde, L},
        {0x0ce0, 0x0ce1, L},
        {0x0ce6, 0x0cef, L},
        {0x0d02, 0x0d03, L},
        {0x0d05, 0x0d0c, L},
        {0x0d0e, 0x0d10, L},
        {0x0d12, 0x0d28, L},
        {0x0d2a, 0x0d39, L},
        {0x0d3e, 0x0d40, L},
        {0x0d41, 0x0d43, NSM},
        {0x0d46, 0x0d48, L},
        {0x0d4a, 0x0d4c, L},
        {0x0d4d, 0x0d4d, NSM},
        {0x0d57, 0x0d57, L},
        {0x0d60, 0x0d61, L},
        {0x0d66, 0x0d6f, L},
        {0x0d82, 0x0d83, L},
        {0x0d85, 0x0d96, L},
        {0x0d9a, 0x0db1, L},
        {0x0db3, 0x0dbb, L},
        {0x0dbd, 0x0dbd, L},
        {0x0dc0, 0x0dc6, L},
        {0x0dca, 0x0dca, NSM},
        {0x0dcf, 0x0dd1, L},
        {0x0dd2, 0x0dd4, NSM},
        {0x0dd6, 0x0dd6, NSM},
        {0x0dd8, 0x0ddf, L},
        {0x0df2, 0x0df4, L},
        {0x0e01, 0x0e30, L},
        {0x0e31, 0x0e31, NSM},
        {0x0e32, 0x0e33, L},
        {0x0e34, 0x0e3a, NSM},
        {0x0e3f, 0x0e3f, ET},
        {0x0e40, 0x0e46, L},
        {0x0e47, 0x0e4e, NSM},
        {0x0e4f, 0x0e5b, L},
        {0x0e81, 0x0e82, L},
        {0x0e84, 0x0e84, L},
        {0x0e87, 0x0e88, L},
        {0x0e8a, 0x0e8a, L},
        {0x0e8d, 0x0e8d, L},
        {0x0e94, 0x0e97, L},
        {0x0e99, 0x0e9f, L},
        {0x0ea1, 0x0ea3, L},
        {0x0ea5, 0x0ea5, L},
        {0x0ea7, 0x0ea7, L},
        {0x0eaa, 0x0eab, L},
        {0x0ead, 0x0eb0, L},
        {0x0eb1, 0x0eb1, NSM},
        {0x0eb2, 0x0eb3, L},
        {0x0eb4, 0x0eb9, NSM},
        {0x0ebb, 0x0ebc, NSM},
        {0x0ebd, 0x0ebd, L},
        {0x0ec0, 0x0ec4, L},
        {0x0ec6, 0x0ec6, L},
        {0x0ec8, 0x0ecd, NSM},
        {0x0ed0, 0x0ed9, L},
        {0x0edc, 0x0edd, L},
        {0x0f00, 0x0f17, L},
        {0x0f18, 0x0f19, NSM},
        {0x0f1a, 0x0f34, L},
        {0x0f35, 0x0f35, NSM},
        {0x0f36, 0x0f36, L},
        {0x0f37, 0x0f37, NSM},
        {0x0f38, 0x0f38, L},
        {0x0f39, 0x0f39, NSM},
        {0x0f3e, 0x0f47, L},
        {0x0f49, 0x0f6a, L},
        {0x0f71, 0x0f7e, NSM},
        {0x0f7f, 0x0f7f, L},
        {0x0f80, 0x0f84, NSM},
        {0x0f85, 0x0f85, L},
        {0x0f86, 0x0f87, NSM},
        {0x0f88, 0x0f8b, L},
        {0x0f90, 0x0f97, NSM},
        {0x0f99, 0x0fbc, NSM},
        {0x0fbe, 0x0fc5, L},
        {0x0fc6, 0x0fc6, NSM},
        {0x0fc7, 0x0fcc, L},
        {0x0fcf, 0x0fcf, L},
        {0x1000, 0x1021, L},
        {0x1023, 0x1027, L},
        {0x1029, 0x102a, L},
        {0x102c, 0x102c, L},
        {0x102d, 0x1030, NSM},
        {0x1031, 0x1031, L},
        {0x1032, 0x1032, NSM},
        {0x1036, 0x1037, NSM},
        {0x1038, 0x1038, L},
        {0x1039, 0x1039, NSM},
        {0x1040, 0x1057, L},
        {0x1058, 0x1059, NSM},
        {0x10a0, 0x10c5, L},
        {0x10d0, 0x10f8, L},
        {0x10fb, 0x10fb, L},
        {0x1100, 0x1159, L},
        {0x115f, 0x11a2, L},
        {0x11a8, 0x11f9, L},
        {0x1200, 0x1206, L},
        {0x1208, 0x1246, L},
        {0x1248, 0x1248, L},
        {0x124a, 0x124d, L},
        {0x1250, 0x1256, L},
        {0x1258, 0x1258, L},
        {0x125a, 0x125d, L},
        {0x1260, 0x1286, L},
        {0x1288, 0x1288, L},
        {0x128a, 0x128d, L},
        {0x1290, 0x12ae, L},
        {0x12b0, 0x12b0, L},
        {0x12b2, 0x12b5, L},
        {0x12b8, 0x12be, L},
        {0x12c0, 0x12c0, L},
        {0x12c2, 0x12c5, L},
        {0x12c8, 0x12ce, L},
        {0x12d0, 0x12d6, L},
        {0x12d8, 0x12ee, L},
        {0x12f0, 0x130e, L},
        {0x1310, 0x1310, L},
        {0x1312, 0x1315, L},
        {0x1318, 0x131e, L},
        {0x1320, 0x1346, L},
        {0x1348, 0x135a, L},
        {0x1361, 0x137c, L},
        {0x13a0, 0x13f4, L},
        {0x1401, 0x1676, L},
        {0x1680, 0x1680, WS},
        {0x1681, 0x169a, L},
        {0x16a0, 0x16f0, L},
        {0x1700, 0x170c, L},
        {0x170e, 0x1711, L},
        {0x1712, 0x1714, NSM},
        {0x1720, 0x1731, L},
        {0x1732, 0x1734, NSM},
        {0x1735, 0x1736, L},
        {0x1740, 0x1751, L},
        {0x1752, 0x1753, NSM},
        {0x1760, 0x176c, L},
        {0x176e, 0x1770, L},
        {0x1772, 0x1773, NSM},
        {0x1780, 0x17b6, L},
        {0x17b7, 0x17bd, NSM},
        {0x17be, 0x17c5, L},
        {0x17c6, 0x17c6, NSM},
        {0x17c7, 0x17c8, L},
        {0x17c9, 0x17d3, NSM},
        {0x17d4, 0x17da, L},
        {0x17db, 0x17db, ET},
        {0x17dc, 0x17dc, L},
        {0x17dd, 0x17dd, NSM},
        {0x17e0, 0x17e9, L},
        {0x180b, 0x180d, NSM},
        {0x180e, 0x180e, WS},
        {0x1810, 0x1819, L},
        {0x1820, 0x1877, L},
        {0x1880, 0x18a8, L},
        {0x18a9, 0x18a9, NSM},
        {0x1900, 0x191c, L},
        {0x1920, 0x1922, NSM},
        {0x1923, 0x1926, L},
        {0x1927, 0x192b, NSM},
        {0x1930, 0x1931, L},
        {0x1932, 0x1932, NSM},
        {0x1933, 0x1938, L},
        {0x1939, 0x193b, NSM},
        {0x1946, 0x196d, L},
        {0x1970, 0x1974, L},
        {0x1d00, 0x1d6b, L},
        {0x1e00, 0x1e9b, L},
        {0x1ea0, 0x1ef9, L},
        {0x1f00, 0x1f15, L},
        {0x1f18, 0x1f1d, L},
        {0x1f20, 0x1f45, L},
        {0x1f48, 0x1f4d, L},
        {0x1f50, 0x1f57, L},
        {0x1f59, 0x1f59, L},
        {0x1f5b, 0x1f5b, L},
        {0x1f5d, 0x1f5d, L},
        {0x1f5f, 0x1f7d, L},
        {0x1f80, 0x1fb4, L},
        {0x1fb6, 0x1fbc, L},
        {0x1fbe, 0x1fbe, L},
        {0x1fc2, 0x1fc4, L},
        {0x1fc6, 0x1fcc, L},
        {0x1fd0, 0x1fd3, L},
        {0x1fd6, 0x1fdb, L},
        {0x1fe0, 0x1fec, L},
        {0x1ff2, 0x1ff4, L},
        {0x1ff6, 0x1ffc, L},
        {0x2000, 0x200a, WS},
        {0x200b, 0x200d, BN},
        {0x200e, 0x200e, L},
        {0x200f, 0x200f, R},
        {0x2028, 0x2028, WS},
        {0x2029, 0x2029, B},
        {0x202a, 0x202a, LRE},
        {0x202b, 0x202b, RLE},
        {0x202c, 0x202c, PDF},
        {0x202d, 0x202d, LRO},
        {0x202e, 0x202e, RLO},
        {0x202f, 0x202f, WS},
        {0x2030, 0x2034, ET},
        {0x2044, 0x2044, CS},
        {0x205f, 0x205f, WS},
        {0x2060, 0x2063, BN},
        {0x206a, 0x206f, BN},
        {0x2070, 0x2070, EN},
        {0x2071, 0x2071, L},
        {0x2074, 0x2079, EN},
        {0x207a, 0x207b, ET},
        {0x207f, 0x207f, L},
        {0x2080, 0x2089, EN},
        {0x208a, 0x208b, ET},
        {0x20a0, 0x20b1, ET},
        {0x20d0, 0x20ea, NSM},
        {0x2102, 0x2102, L},
        {0x2107, 0x2107, L},
        {0x210a, 0x2113, L},
        {0x2115, 0x2115, L},
        {0x2119, 0x211d, L},
        {0x2124, 0x2124, L},
        {0x2126, 0x2126, L},
        {0x2128, 0x2128, L},
        {0x212a, 0x212d, L},
        {0x212e, 0x212e, ET},
        {0x212f, 0x2131, L},
        {0x2133, 0x2139, L},
        {0x213d, 0x213f, L},
        {0x2145, 0x2149, L},
        {0x2160, 0x2183, L},
        {0x2212, 0x2213, ET},
        {0x2336, 0x237a, L},
        {0x2395, 0x2395, L},
        {0x2488, 0x249b, EN},
        {0x249c, 0x24e9, L},
        {0x2800, 0x28ff, L},
        {0x3000, 0x3000, WS},
        {0x3005, 0x3007, L},
        {0x3021, 0x3029, L},
        {0x302a, 0x302f, NSM},
        {0x3031, 0x3035, L},
        {0x3038, 0x303c, L},
        {0x3041, 0x3096, L},
        {0x3099, 0x309a, NSM},
        {0x309d, 0x309f, L},
        {0x30a1, 0x30fa, L},
        {0x30fc, 0x30ff, L},
        {0x3105, 0x312c, L},
        {0x3131, 0x318e, L},
        {0x3190, 0x31b7, L},
        {0x31f0, 0x321c, L},
        {0x3220, 0x3243, L},
        {0x3260, 0x327b, L},
        {0x327f, 0x32b0, L},
        {0x32c0, 0x32cb, L},
        {0x32d0, 0x32fe, L},
        {0x3300, 0x3376, L},
        {0x337b, 0x33dd, L},
        {0x33e0, 0x33fe, L},
        {0x3400, 0x4db5, L},
        {0x4e00, 0x9fa5, L},
        {0xa000, 0xa48c, L},
        {0xac00, 0xd7a3, L},
        {0xd800, 0xfa2d, L},
        {0xfa30, 0xfa6a, L},
        {0xfb00, 0xfb06, L},
        {0xfb13, 0xfb17, L},
        {0xfb1d, 0xfb1d, R},
        {0xfb1e, 0xfb1e, NSM},
        {0xfb1f, 0xfb28, R},
        {0xfb29, 0xfb29, ET},
        {0xfb2a, 0xfb36, R},
        {0xfb38, 0xfb3c, R},
        {0xfb3e, 0xfb3e, R},
        {0xfb40, 0xfb41, R},
        {0xfb43, 0xfb44, R},
        {0xfb46, 0xfb4f, R},
        {0xfb50, 0xfbb1, AL},
        {0xfbd3, 0xfd3d, AL},
        {0xfd50, 0xfd8f, AL},
        {0xfd92, 0xfdc7, AL},
        {0xfdf0, 0xfdfc, AL},
        {0xfe00, 0xfe0f, NSM},
        {0xfe20, 0xfe23, NSM},
        {0xfe50, 0xfe50, CS},
        {0xfe52, 0xfe52, CS},
        {0xfe55, 0xfe55, CS},
        {0xfe5f, 0xfe5f, ET},
        {0xfe62, 0xfe63, ET},
        {0xfe69, 0xfe6a, ET},
        {0xfe70, 0xfe74, AL},
        {0xfe76, 0xfefc, AL},
        {0xfeff, 0xfeff, BN},
        {0xff03, 0xff05, ET},
        {0xff0b, 0xff0b, ET},
        {0xff0c, 0xff0c, CS},
        {0xff0d, 0xff0d, ET},
        {0xff0e, 0xff0e, CS},
        {0xff0f, 0xff0f, ES},
        {0xff10, 0xff19, EN},
        {0xff1a, 0xff1a, CS},
        {0xff21, 0xff3a, L},
        {0xff41, 0xff5a, L},
        {0xff66, 0xffbe, L},
        {0xffc2, 0xffc7, L},
        {0xffca, 0xffcf, L},
        {0xffd2, 0xffd7, L},
        {0xffda, 0xffdc, L},
        {0xffe0, 0xffe1, ET},
        {0xffe5, 0xffe6, ET},
    };

    int i, j, k;

    i = -1;
    j = lenof(lookup);

    while (j - i > 1) {
	k = (i + j) / 2;
	if (ch < lookup[k].first)
	    j = k;
	else if (ch > lookup[k].last)
	    i = k;
	else
	    return (unsigned char)lookup[k].type;
    }

    /*
     * If we reach here, the character was not in any of the
     * intervals listed in the lookup table. This means we return
     * ON (`Other Neutrals'). This is the appropriate code for any
     * character genuinely not listed in the Unicode table, and
     * also the table above has deliberately left out any
     * characters _explicitly_ listed as ON (to save space!).
     */
    return ON;
}

/* The Main shaping function, and the only one to be used
 * by the outside world.
 *
 * line: buffer to apply shaping to. this must be passed by doBidi() first
 * to: output buffer for the shaped data
 * from: start bidi at this index
 * count: number of characters in line
 */
int doShape(BLOCKTYPE line, CHARTYPE* to, int from, int count)
{
	int i, j, ligFlag;
	unsigned char prevTemp, nextTemp;

	ligFlag = 0;
	prevTemp = SU;
	nextTemp = SU;
	for(i=from; i<count; i++)
	{
		/* Get Previous and next Characters type */
		j=i;
		while(--j >= 0)
		{
			if(GetType(GETCHAR(line, j)) != NSM)
			{
				prevTemp = STYPE(GETCHAR(line, j));
				break;
			}
		}
		j=i;
		while(++j < count)
		{
			if(GetType(GETCHAR(line, j)) != NSM)
			{
				nextTemp = STYPE(GETCHAR(line, j));
				break;
			}else if(j == count-1)
			{
				nextTemp = SU;
				break;
			}
		}

		switch(STYPE(GETCHAR(line, i)))
		{
		case SC:
		case SU:
			to[i] = GETCHAR(line, i);
			break;

		case SR:
			if(prevTemp == SD || prevTemp == SC)
				to[i] = SFINAL(SISOLATED(GETCHAR(line, i)));
			else
				to[i] = SISOLATED(GETCHAR(line, i));
			break;

		case SD:
      /* Make Ligatures */
			if(GETCHAR(line, i) == 0x644)
			{
				j=i;
	            CHARTYPE tempChar = 0;
				while(j++<count)
				{
					if(GetType(GETCHAR(line, j)) != NSM)
					{
						tempChar = GETCHAR(line, j);
						break;
					}
				}
				switch(tempChar)
				{
				case 0x622:
					ligFlag = 1;
					if(prevTemp == SD || prevTemp == SC)
						to[i] = 0xFEF6;
					else
						to[i] = 0xFEF5;
					break;
				case 0x623:
					ligFlag = 1;
					if(prevTemp == SD || prevTemp == SC)
						to[i] = 0xFEF8;
					else
						to[i] = 0xFEF7;
					break;
				case 0x625:
					ligFlag = 1;
					if(prevTemp == SD || prevTemp == SC)
						to[i] = 0xFEFA;
					else
						to[i] = 0xFEF9;
					break;
				case 0x627:
					ligFlag = 1;
					if(prevTemp == SD || prevTemp == SC)
						to[i] = 0xFEFC;
					else
						to[i] = 0xFEFB;
					break;
				}
				if(ligFlag)
				{
					to[j] = 0x20;
					i = j;
					ligFlag = 0;
					break;
				}
			}
			if((prevTemp == SD) || (prevTemp == SC))
			{
				if(nextTemp == SR || nextTemp == SD || nextTemp == SC)
					to[i] = SMEDIAL(SISOLATED(GETCHAR(line, i)));
				else 
					to[i] = SFINAL(SISOLATED(GETCHAR(line, i)));
				break;
			}else
			{
				if(nextTemp == SR || nextTemp == SD || nextTemp == SC)
					to[i] = SINITIAL(SISOLATED(GETCHAR(line, i)));
				else
					to[i] = SISOLATED(GETCHAR(line, i));
				break;
			}

		}
		nextTemp = SU;
	}
	return 1;
}

/* Rule (X1), (X2), (X3), (X4), (X5), (X6), (X7), (X8), (X9) 
 * returns the length of the string without explicit marks
 */
int doTypes(BLOCKTYPE line, unsigned char paragraphLevel, unsigned char* types,
			 unsigned char* levels, int count, int fX)
{
  
  unsigned char tempType;
  unsigned char currentEmbedding = paragraphLevel;
  unsigned char currentOverride = ON;
  int i, j;
  unsigned char levelStack[MAX_STACK+2];
  unsigned char overrideStack[MAX_STACK+2];
  int stackTop = 0;
  
  if(fX)
    {
      for( i=0, j=0; i<count; i++)
	{
	  GETCHAR(line, j) = GETCHAR(line, i);
	  tempType = GetType(GETCHAR(line, i));
	  switch(tempType)
	    {
	    case RLE:
	      if(stackTop < MAX_STACK)
		{
		  levelStack[stackTop] = currentEmbedding;
		  overrideStack[stackTop] = currentOverride;
		  stackTop++;
		  currentEmbedding = leastGreaterOdd(currentEmbedding);
		  currentOverride = ON;
		}
	      break;
	    case LRE:
	      if(stackTop < MAX_STACK)
		{
		  levelStack[stackTop] = currentEmbedding;
		  overrideStack[stackTop] = currentOverride;
		  stackTop++;
		  currentEmbedding = leastGreaterEven(currentEmbedding);
		  currentOverride = ON;
		}
	      break;
	    case RLO:
	      if(stackTop < MAX_STACK)
		{
		  levelStack[stackTop] = currentEmbedding;
		  overrideStack[stackTop] = currentOverride;
		  stackTop++;
		  currentEmbedding = leastGreaterOdd(currentEmbedding);
		  currentOverride = R;
		}
	      break;
	    case LRO:
	      if(stackTop <= MAX_STACK)
		{
		  levelStack[stackTop] = currentEmbedding;
		  overrideStack[stackTop] = currentOverride;
		  stackTop++;
		  currentEmbedding = leastGreaterEven(currentEmbedding);
		  currentOverride = L;
		}
	      break;
	    case PDF:
	      if(stackTop > 0)
		{
		  currentEmbedding = levelStack[stackTop-1];
		  currentOverride = overrideStack[stackTop-1];
		  stackTop--;
		}
	      break;
	      /* Whitespace is treated as neutral for now */
	    case WS:
	    case B:
	    case S:
	      levels[j] = currentEmbedding;
	      tempType = ON;
	      if(currentOverride != ON)
		tempType = currentOverride;
	      types[j] = tempType;
	      j++;
	      break;
	    default:
	      levels[j] = currentEmbedding;
	      if(currentOverride != ON)
		tempType = currentOverride;
	      types[j] = tempType;
	      j++;
	      break;
	    }
	}
    }else
      {
	j = count;
	for( i=0; i<count; i++)
	  {
	    tempType = GetType(GETCHAR(line, i));
	    switch(tempType)
	      {
	      case WS:
	      case B:
	      case S:
		levels[i] = currentEmbedding;
		tempType = ON;
		if(currentOverride != ON)
		  tempType = currentOverride;
		break;
	      default:
		levels[i] = currentEmbedding;
		if(currentOverride != ON)
		  tempType = currentOverride;
		break;
	      }
	    types[i] = tempType;
	  }
      }
  return j;
}

/* Rule (W3) */
void doALtoR(unsigned char* types, int count)
{
  int i=0;
  for(; i<count; i++)
    {
      if(types[i] == AL)
	types[i] = R;
    }
}

/*
 * The Main Bidi Function, and the only function that should
 * be used by the outside world.
 *
 * line: a buffer of size count containing text to apply
 * the Bidirectional algorithm to.
 */
int doBidi(BLOCKTYPE line, int count, bool applyShape, bool reorderCombining)
{
   unsigned char* types;
   unsigned char* levels;
   unsigned char paragraphLevel;
   unsigned char tempType, tempTypeSec;
   int i, j, imax, fX, fAL, fET, fNSM;
   CHARTYPE* shapeTo;


	fX = fAL = fET = fNSM = 0;
	for(i=0; i<count; i++)
	{
		switch(GetType(line[i]))
		{
		case AL:
		case R:
			fAL = 1;
			break;
		case LRE:
		case LRO:
		case RLE:
		case RLO:
		case PDF:
		case BN:
			fX = 1;
			break;
		case ET:
			fET = 1;
			break;
		case NSM:
			fNSM = 1;
			break;
		}
	}

	if(!fAL && !fX)
		return 0;

   /* Initialize types, levels */
   types = (unsigned char*)malloc(sizeof(unsigned char) * count);
   levels = (unsigned char*)malloc(sizeof(unsigned char) * count);
   if(applyShape)
	   shapeTo = (CHARTYPE*)malloc(sizeof(CHARTYPE) * count);

   /* Rule (P1)  NOT IMPLEMENTED
    * P1. Split the text into separate paragraphs. A paragraph separator is
    * kept with the previous paragraph. Within each paragraph, apply all the
    * other rules of this algorithm.
    */

   /* Rule (P2), (P3)
    * P2. In each paragraph, find the first character of type L, AL, or R.
    * P3. If a character is found in P2 and it is of type AL or R, then set
    * the paragraph embedding level to one; otherwise, set it to zero.
    */
   paragraphLevel = GetParagraphLevel(line, count);

   /* Rule (X1), (X2), (X3), (X4), (X5), (X6), (X7), (X8), (X9)
    * X1. Begin by setting the current embedding level to the paragraph
	*     embedding level. Set the directional override status to neutral.
    * X2. With each RLE, compute the least greater odd embedding level.
    * X3. With each LRE, compute the least greater even embedding level.
    * X4. With each RLO, compute the least greater odd embedding level.
    * X5. With each LRO, compute the least greater even embedding level.
    * X6. For all types besides RLE, LRE, RLO, LRO, and PDF:
    *		a. Set the level of the current character to the current
    *		    embedding level.
    *		b.  Whenever the directional override status is not neutral,
    *               reset the current character type to the directional
    *               override status.
    * X7. With each PDF, determine the matching embedding or override code.
    * If there was a valid matching code, restore (pop) the last
    * remembered (pushed) embedding level and directional override.
    * X8. All explicit directional embeddings and overrides are completely
    * terminated at the end of each paragraph. Paragraph separators are not
    * included in the embedding. (Useless here) NOT IMPLEMENTED
    * X9. Remove all RLE, LRE, RLO, LRO, PDF, and BN codes.
    * Here, they're converted to BN.
    */

   count = doTypes(line, paragraphLevel, types, levels, count, fX);
   GETCHAR(line, count) = 0;


   /* Rule (W1)
    * W1. Examine each non-spacing mark (NSM) in the level run, and change
    * the type of the NSM to the type of the previous character. If the NSM
    * is at the start of the level run, it will get the type of sor.
    */
	
   if(fNSM)
     {
       if(types[0] == NSM)
	 types[0] = paragraphLevel;
       
       for(i=1; i<count; i++)
	 {
	   if(types[i] == NSM)
	     types[i] = types[i-1];
	   /* Is this a safe assumption? 
	    * I assumed the previous, IS a character.
	    */
	 }
     }
   
   /* Rule (W2)
    * W2. Search backwards from each instance of a European number until the
    * first strong type (R, L, AL, or sor) is found.  If an AL is found,
    * change the type of the European number to Arabic number.
    */
   for(i=0; i<count; i++)
     {
       if(types[i] == EN)
	 {
	   tempType = levels[i];
	   j=i;
	   while(--j >= 0 && levels[j] == tempType)
	     {
	       if(types[j] == AL)
		 {
		   types[i] = AN;
		   break;
		 }
	       else if(types[j] == R || types[j] == L)
		 {
		   break;
		 }
	     }
	 }
     }

   /* Rule (W3)
    * W3. Change all ALs to R.
    * 
    * Optimization: on Rule Xn, we might set a flag on AL type
    * to prevent this loop in L R lines only...
    */
   doALtoR(types, count);

   /* Rule (W4)
    * W4. A single European separator between two European numbers changes
    * to a European number. A single common separator between two numbers
    * of the same type changes to that type.
    */
   for( i=0; i<(count-1); i++)
     {
       if(types[i] == ES)
	 {
	   if(types[i-1] == EN && types[i+1] == EN)
	     types[i] = EN;
	 }else if(types[i] == CS)
	   {
	     if(types[i-1] == EN && types[i+1] == EN)
	       types[i] = EN;
	     else if(types[i-1] == AN && types[i+1] == AN)
	       types[i] = AN;
	   }
     }
   
   /* Rule (W5)
    * W5. A sequence of European terminators adjacent to European numbers
    * changes to all European numbers.
    *
    * Optimization: lots here... else ifs need rearrangement
    */
   if(fET)
     {
       for(i=0; i<count; i++)
	 {
	   if(types[i] == ET)
	     {
	       if(types[i-1] == EN)
		 {
		   types[i] = EN;
		   continue;
		 }else if(types[i+1] == EN)
		   {
		     types[i] = EN;
		     continue;
		   }else if(types[i+1] == ET)
		     {
		       j=i;
		       while(j <count && types[j] == ET)
			 {
			   j++;
			 }
		       if(types[j] == EN)
			 types[i] = EN;
		     }
	     }
	 }
     }
   
   /* Rule (W6)
    * W6. Otherwise, separators and terminators change to Other Neutral:
    */
   for(i=0; i<count; i++)
     {
       switch(types[i])
	 {
	 case ES:
	 case ET:
	 case CS:
	   types[i] = ON;
	   break;
	 }
     }
   
   /* Rule (W7)
    * W7. Search backwards from each instance of a European number until
    * the first strong type (R, L, or sor) is found. If an L is found,
    * then change the type of the European number to L.
    */
	
   for(i=0; i<count; i++)
     {
       if(types[i] == EN)
	 {
	   tempType = levels[i];
	   j=i;
	   while(--j >= 0 && levels[j] == tempType)
	     {
	       if(types[j] == L)
		 {
		   types[i] = L;
		   break;
		 }
	       else if(types[j] == R || types[j] == AL)
		 {
		   break;
		 }
	       
	     }
	 }
     }
   
	
   /* Rule (N1)
    * N1. A sequence of neutrals takes the direction of the surrounding
    * strong text if the text on both sides has the same direction. European
    * and Arabic numbers are treated as though they were R.
    */
   tempType = paragraphLevel;
   for(i=0; i<count; i++)
     {
       if(types[i] == ON)
	 {
	   if(types[i-1] == R || types[i-1] == EN || types[i-1] == AN)
	     tempType = R;
	   else
	     tempType = L;
	   j=i;
	   
	   while(j < count)
	     {
	       tempTypeSec = types[j];
	       if(tempTypeSec == ON)
		 j++;
	       else
		 break;
	     }
	   if(j == count)
	     tempTypeSec = odd(paragraphLevel) ? R : L;
	   
	   
	   if(((tempTypeSec == L || tempTypeSec == LRE) && (tempType == L)) ||
	      (((tempTypeSec == R) || (tempTypeSec == EN) || (tempTypeSec == AN)) && (tempType == R)))
	     {
	       while(i<j)
		 {
		   types[i++] = tempType;
		 }
	     }else
	       i = j;
	   
	 }
     }

   /* Rule (N2)
    * N2. Any remaining neutrals take the embedding direction.
    */
   for(i=0; i<count; i++)
     {
       if(types[i] == ON)
	 {
	   if((levels[i] % 2) == 0)
	     types[i] = L;
	   else
	     types[i] = R;
	 }
     }

   /* Rule (I1)
    * I1. For all characters with an even (left-to-right) embedding
    * direction, those of type R go up one level and those of type AN or
    * EN go up two levels.
    */
   for(i=0; i<count; i++)
     {
       if((levels[i] % 2) == 0)
	 {
	   if(types[i] == R)
	     levels[i] += 1;
	   else if((types[i] == AN) || (types[i] == EN))
	     levels[i] += 2;
	 }else
	   {
	     if((types[i] == L) ||
		(types[i] == EN) ||
		(types[i] == AN))
	       levels[i] += 1;
	   }
     }
   
   /* Rule (I2)
    * I2. For all characters with an odd (right-to-left) embedding direction,
    * those of type L, EN or AN go up one level.
    */

   for(i=0; i<count; i++)
   {
      if((levels[i] % 2) == 1)
      {
	 if(types[i] == L || types[i] == EN || types[i] == AN)
	    levels[i] += 1;
      }
   }

   /* Rule (L1)
    * L1. On each line, reset the embedding level of the following characters
    * to the paragraph embedding level:
    *		(1)segment separators, (2)paragraph separators,
    *           (3)any sequence of whitespace characters preceding
    *           a segment separator or paragraph separator,
    *           (4)and any sequence of white space characters
    *           at the end of the line.
    * The types of characters used here are the original types, not those
    * modified by the previous phase. 
    */

   j=count-1;
   while(j>0 && (GetType(GETCHAR(line, j)) == WS))
     {
       j--;
     }
   if(j < (count-1))
     {
       for(j++; j<count; j++)
	 levels[j] = paragraphLevel;
     }
   
   for(i=0; i<count; i++)
     {
       tempType = GetType(GETCHAR(line, i));
       if(tempType == WS)
      {
	j=i;
	while((++j < count) && ((tempType == WS) || (tempType == RLE)) )
	  {
	    tempType = GetType(line[j]);
	  }
	
	if(GetType(GETCHAR(line, j)) == B || GetType(GETCHAR(line, j)) == S)
	  {
	    for(j--; j>=i ; j--)
	      {
		levels[j] = paragraphLevel;
	      }
	  }
      }else if(tempType == B || tempType == S)
	levels[i] = paragraphLevel;
   }
   
   /* Rule (L4)
    * L4. A character that possesses the mirrored property as specified by
    * Section 4.7, Mirrored, must be depicted by a mirrored glyph if the
    * resolved directionality of that character is R.
    */
   /* Note: this is implemented before L2 for efficiency */
   for(i=0; i<count; i++)
     {
       if((levels[i] % 2) == 1)
	 doMirror(&GETCHAR(line, i));
     }
   
   /* Rule (L3)
    * L3. Combining marks applied to a right-to-left base character will at
    * this point precede their base character. If the rendering engine
    * expects them to follow the base characters in the final display
    * process, then the ordering of the marks and the base character must
    * be reversed.
	* Combining marks are reordered to the right of each character on an
	* odd level.
    */

   if(fNSM && reorderCombining)
   {
     CHARTYPE temp;
     int it;
     for(i=1; i<count; i++)     // Changed 0 to 1 to fix a crash - Don't know if there will be other consequences
       {
	 if(GetType(GETCHAR(line, i)) == NSM && odd(levels[i]))
	   {
	     j=i;
	     while((++j < count) && (GetType(GETCHAR(line, j)) == NSM));
	     j--; i--;
	     for(it=j; j>i; i++, j--)
	       {
		 temp = GETCHAR(line, i);
		 GETCHAR(line, i) = GETCHAR(line, j);
		 GETCHAR(line, j) = temp;
	       }
	     i=it+1;
	   }
       }
   }
   
   /* Shaping 
    * Shaping is Applied to each run of levels separately....
    */
   
   if(applyShape)
     {
       
       for(i=0; i<count; i++)
	 {
	   shapeTo[i] = GETCHAR(line, i);
	 }
       
       j=i=0;
       while(j < count)
	 {
	   if(GetType(GETCHAR(line, j)) == AL)
	     {
	       if(j<count && j >= i )
		 {
		   tempType = levels[j];
		   i=j;
		   while((i++ < count) && (levels[i] == tempType));
		   doShape(line, shapeTo, j, i);
		   j=i;
		   tempType = levels[j];
		   
		 }
	     }
	   j++;
	 }
       for(i=0; i<count; i++)
	 {
	   GETCHAR(line, i) = shapeTo[i];
	 }
       free(shapeTo);		
     }
      
   /* Rule (L2)
    * L2. From the highest level found in the text to the lowest odd level on
    * each line, including intermediate levels not actually present in the
    * text, reverse any contiguous sequence of characters that are at that
    * level or higher
    */
   /* we flip the character string and leave the level array */
   imax = 0;
   i=0;
   tempType = levels[0];
   while(i < count)
     {
       if(levels[i] > tempType)
	 {
	   tempType = levels[i];
	   imax=i;
	 }
       i++;
     }
   /* maximum level in tempType, its index in imax. */
   while(tempType > 0)		/* loop from highest level to the least odd, */
     {				/* which i assume is 1 */
       flipThisRun(line, levels, tempType, count);
       tempType--;						
     }
   
   free(types);
   free(levels);
   
   return count;
}


/*
 * takes a pointer to a character that is checked for
 * having a mirror glyph, and replaced on the spot
 */
void doMirror(CHARTYPE* ch)
{
  static const struct{
    CHARTYPE first, mirror;
  } lookup[] = {
  {0x0028, 0x0029},
  {0x0029, 0x0028},
  {0x003C, 0x003E},
  {0x003E, 0x003C},
  {0x005B, 0x005D},
  {0x005D, 0x005B},
  {0x007B, 0x007D},
  {0x007D, 0x007B},
  {0x00AB, 0x00BB},
  {0x00BB, 0x00AB},
  {0x2039, 0x203A},
  {0x203A, 0x2039},
  {0x2045, 0x2046},
  {0x2046, 0x2045},
  {0x207D, 0x207E},
  {0x207E, 0x207D},
  {0x208D, 0x208E},
  {0x208E, 0x208D},
  {0x2208, 0x220B},
  {0x2209, 0x220C},
  {0x220A, 0x220D},
  {0x220B, 0x2208},
  {0x220C, 0x2209},
  {0x220D, 0x220A},
  {0x2215, 0x29F5},
  {0x223C, 0x223D},
  {0x223D, 0x223C},
  {0x2243, 0x22CD},
  {0x2252, 0x2253},
  {0x2253, 0x2252},
  {0x2254, 0x2255},
  {0x2255, 0x2254},
  {0x2264, 0x2265},
  {0x2265, 0x2264},
  {0x2266, 0x2267},
  {0x2267, 0x2266},
  {0x2268, 0x2269},
  {0x2269, 0x2268},
  {0x226A, 0x226B},
  {0x226B, 0x226A},
  {0x226E, 0x226F},
  {0x226F, 0x226E},
  {0x2270, 0x2271},
  {0x2271, 0x2270},
  {0x2272, 0x2273},
  {0x2273, 0x2272},
  {0x2274, 0x2275},
  {0x2275, 0x2274},
  {0x2276, 0x2277},
  {0x2277, 0x2276},
  {0x2278, 0x2279},
  {0x2279, 0x2278},
  {0x227A, 0x227B},
  {0x227B, 0x227A},
  {0x227C, 0x227D},
  {0x227D, 0x227C},
  {0x227E, 0x227F},
  {0x227F, 0x227E},
  {0x2280, 0x2281},
  {0x2281, 0x2280},
  {0x2282, 0x2283},
  {0x2283, 0x2282},
  {0x2284, 0x2285},
  {0x2285, 0x2284},
  {0x2286, 0x2287},
  {0x2287, 0x2286},
  {0x2288, 0x2289},
  {0x2289, 0x2288},
  {0x228A, 0x228B},
  {0x228B, 0x228A},
  {0x228F, 0x2290},
  {0x2290, 0x228F},
  {0x2291, 0x2292},
  {0x2292, 0x2291},
  {0x2298, 0x29B8},
  {0x22A2, 0x22A3},
  {0x22A3, 0x22A2},
  {0x22A6, 0x2ADE},
  {0x22A8, 0x2AE4},
  {0x22A9, 0x2AE3},
  {0x22AB, 0x2AE5},
  {0x22B0, 0x22B1},
  {0x22B1, 0x22B0},
  {0x22B2, 0x22B3},
  {0x22B3, 0x22B2},
  {0x22B4, 0x22B5},
  {0x22B5, 0x22B4},
  {0x22B6, 0x22B7},
  {0x22B7, 0x22B6},
  {0x22C9, 0x22CA},
  {0x22CA, 0x22C9},
  {0x22CB, 0x22CC},
  {0x22CC, 0x22CB},
  {0x22CD, 0x2243},
  {0x22D0, 0x22D1},
  {0x22D1, 0x22D0},
  {0x22D6, 0x22D7},
  {0x22D7, 0x22D6},
  {0x22D8, 0x22D9},
  {0x22D9, 0x22D8},
  {0x22DA, 0x22DB},
  {0x22DB, 0x22DA},
  {0x22DC, 0x22DD},
  {0x22DD, 0x22DC},
  {0x22DE, 0x22DF},
  {0x22DF, 0x22DE},
  {0x22E0, 0x22E1},
  {0x22E1, 0x22E0},
  {0x22E2, 0x22E3},
  {0x22E3, 0x22E2},
  {0x22E4, 0x22E5},
  {0x22E5, 0x22E4},
  {0x22E6, 0x22E7},
  {0x22E7, 0x22E6},
  {0x22E8, 0x22E9},
  {0x22E9, 0x22E8},
  {0x22EA, 0x22EB},
  {0x22EB, 0x22EA},
  {0x22EC, 0x22ED},
  {0x22ED, 0x22EC},
  {0x22F0, 0x22F1},
  {0x22F1, 0x22F0},
  {0x22F2, 0x22FA},
  {0x22F3, 0x22FB},
  {0x22F4, 0x22FC},
  {0x22F6, 0x22FD},
  {0x22F7, 0x22FE},
  {0x22FA, 0x22F2},
  {0x22FB, 0x22F3},
  {0x22FC, 0x22F4},
  {0x22FD, 0x22F6},
  {0x22FE, 0x22F7},
  {0x2308, 0x2309},
  {0x2309, 0x2308},
  {0x230A, 0x230B},
  {0x230B, 0x230A},
  {0x2329, 0x232A},
  {0x232A, 0x2329},
  {0x2768, 0x2769},
  {0x2769, 0x2768},
  {0x276A, 0x276B},
  {0x276B, 0x276A},
  {0x276C, 0x276D},
  {0x276D, 0x276C},
  {0x276E, 0x276F},
  {0x276F, 0x276E},
  {0x2770, 0x2771},
  {0x2771, 0x2770},
  {0x2772, 0x2773},
  {0x2773, 0x2772},
  {0x2774, 0x2775},
  {0x2775, 0x2774},
  {0x27C3, 0x27C4},
  {0x27C4, 0x27C3},
  {0x27C5, 0x27C6},
  {0x27C6, 0x27C5},
  {0x27D5, 0x27D6},
  {0x27D6, 0x27D5},
  {0x27DD, 0x27DE},
  {0x27DE, 0x27DD},
  {0x27E2, 0x27E3},
  {0x27E3, 0x27E2},
  {0x27E4, 0x27E5},
  {0x27E5, 0x27E4},
  {0x27E6, 0x27E7},
  {0x27E7, 0x27E6},
  {0x27E8, 0x27E9},
  {0x27E9, 0x27E8},
  {0x27EA, 0x27EB},
  {0x27EB, 0x27EA},
  {0x2983, 0x2984},
  {0x2984, 0x2983},
  {0x2985, 0x2986},
  {0x2986, 0x2985},
  {0x2987, 0x2988},
  {0x2988, 0x2987},
  {0x2989, 0x298A},
  {0x298A, 0x2989},
  {0x298B, 0x298C},
  {0x298C, 0x298B},
  {0x298D, 0x2990},
  {0x298E, 0x298F},
  {0x298F, 0x298E},
  {0x2990, 0x298D},
  {0x2991, 0x2992},
  {0x2992, 0x2991},
  {0x2993, 0x2994},
  {0x2994, 0x2993},
  {0x2995, 0x2996},
  {0x2996, 0x2995},
  {0x2997, 0x2998},
  {0x2998, 0x2997},
  {0x29B8, 0x2298},
  {0x29C0, 0x29C1},
  {0x29C1, 0x29C0},
  {0x29C4, 0x29C5},
  {0x29C5, 0x29C4},
  {0x29CF, 0x29D0},
  {0x29D0, 0x29CF},
  {0x29D1, 0x29D2},
  {0x29D2, 0x29D1},
  {0x29D4, 0x29D5},
  {0x29D5, 0x29D4},
  {0x29D8, 0x29D9},
  {0x29D9, 0x29D8},
  {0x29DA, 0x29DB},
  {0x29DB, 0x29DA},
  {0x29F5, 0x2215},
  {0x29F8, 0x29F9},
  {0x29F9, 0x29F8},
  {0x29FC, 0x29FD},
  {0x29FD, 0x29FC},
  {0x2A2B, 0x2A2C},
  {0x2A2C, 0x2A2B},
  {0x2A2D, 0x2A2E},
  {0x2A2E, 0x2A2D},
  {0x2A34, 0x2A35},
  {0x2A35, 0x2A34},
  {0x2A3C, 0x2A3D},
  {0x2A3D, 0x2A3C},
  {0x2A64, 0x2A65},
  {0x2A65, 0x2A64},
  {0x2A79, 0x2A7A},
  {0x2A7A, 0x2A79},
  {0x2A7D, 0x2A7E},
  {0x2A7E, 0x2A7D},
  {0x2A7F, 0x2A80},
  {0x2A80, 0x2A7F},
  {0x2A81, 0x2A82},
  {0x2A82, 0x2A81},
  {0x2A83, 0x2A84},
  {0x2A84, 0x2A83},
  {0x2A8B, 0x2A8C},
  {0x2A8C, 0x2A8B},
  {0x2A91, 0x2A92},
  {0x2A92, 0x2A91},
  {0x2A93, 0x2A94},
  {0x2A94, 0x2A93},
  {0x2A95, 0x2A96},
  {0x2A96, 0x2A95},
  {0x2A97, 0x2A98},
  {0x2A98, 0x2A97},
  {0x2A99, 0x2A9A},
  {0x2A9A, 0x2A99},
  {0x2A9B, 0x2A9C},
  {0x2A9C, 0x2A9B},
  {0x2AA1, 0x2AA2},
  {0x2AA2, 0x2AA1},
  {0x2AA6, 0x2AA7},
  {0x2AA7, 0x2AA6},
  {0x2AA8, 0x2AA9},
  {0x2AA9, 0x2AA8},
  {0x2AAA, 0x2AAB},
  {0x2AAB, 0x2AAA},
  {0x2AAC, 0x2AAD},
  {0x2AAD, 0x2AAC},
  {0x2AAF, 0x2AB0},
  {0x2AB0, 0x2AAF},
  {0x2AB3, 0x2AB4},
  {0x2AB4, 0x2AB3},
  {0x2ABB, 0x2ABC},
  {0x2ABC, 0x2ABB},
  {0x2ABD, 0x2ABE},
  {0x2ABE, 0x2ABD},
  {0x2ABF, 0x2AC0},
  {0x2AC0, 0x2ABF},
  {0x2AC1, 0x2AC2},
  {0x2AC2, 0x2AC1},
  {0x2AC3, 0x2AC4},
  {0x2AC4, 0x2AC3},
  {0x2AC5, 0x2AC6},
  {0x2AC6, 0x2AC5},
  {0x2ACD, 0x2ACE},
  {0x2ACE, 0x2ACD},
  {0x2ACF, 0x2AD0},
  {0x2AD0, 0x2ACF},
  {0x2AD1, 0x2AD2},
  {0x2AD2, 0x2AD1},
  {0x2AD3, 0x2AD4},
  {0x2AD4, 0x2AD3},
  {0x2AD5, 0x2AD6},
  {0x2AD6, 0x2AD5},
  {0x2ADE, 0x22A6},
  {0x2AE3, 0x22A9},
  {0x2AE4, 0x22A8},
  {0x2AE5, 0x22AB},
  {0x2AEC, 0x2AED},
  {0x2AED, 0x2AEC},
  {0x2AF7, 0x2AF8},
  {0x2AF8, 0x2AF7},
  {0x2AF9, 0x2AFA},
  {0x2AFA, 0x2AF9},
  {0x2E02, 0x2E03},
  {0x2E03, 0x2E02},
  {0x2E04, 0x2E05},
  {0x2E05, 0x2E04},
  {0x2E09, 0x2E0A},
  {0x2E0A, 0x2E09},
  {0x2E0C, 0x2E0D},
  {0x2E0D, 0x2E0C},
  {0x2E1C, 0x2E1D},
  {0x2E1D, 0x2E1C},
  {0x3008, 0x3009},
  {0x3009, 0x3008},
  {0x300A, 0x300B},
  {0x300B, 0x300A},
  {0x300C, 0x300D},
  {0x300D, 0x300C},
  {0x300E, 0x300F},
  {0x300F, 0x300E},
  {0x3010, 0x3011},
  {0x3011, 0x3010},
  {0x3014, 0x3015},
  {0x3015, 0x3014},
  {0x3016, 0x3017},
  {0x3017, 0x3016},
  {0x3018, 0x3019},
  {0x3019, 0x3018},
  {0x301A, 0x301B},
  {0x301B, 0x301A},
  {0xFF08, 0xFF09},
  {0xFF09, 0xFF08},
  {0xFF1C, 0xFF1E},
  {0xFF1E, 0xFF1C},
  {0xFF3B, 0xFF3D},
  {0xFF3D, 0xFF3B},
  {0xFF5B, 0xFF5D},
  {0xFF5D, 0xFF5B},
  {0xFF5F, 0xFF60},
  {0xFF60, 0xFF5F},
  {0xFF62, 0xFF63},
  {0xFF63, 0xFF62},
  };
  
  int i, j, k;
  
  i = -1;
  j = lenof(lookup);
  
  while (j - i > 1) {
    k = (i + j) / 2;
    if (*ch < lookup[k].first)
      j = k;
    else if (*ch > lookup[k].first)
      i = k;
    else if(*ch == lookup[k].first)
      {
	//return (unsigned char)lookup[k].type;
	*ch = lookup[k].mirror;
	return;
      }
  }
}

#endif