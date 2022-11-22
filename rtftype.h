/*
 * This file was adapted from Microsoft Rich Text Format Specification 1.6
 * http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnrtfspec/html/rtfspec.asp
*/

#include "SDL.h"
#include "SDL_rtf.h"

typedef char bool;
#define fTrue 1
#define fFalse 0

typedef struct char_prop
{
    int  fFont;
    int  fFontSize;
    char fBgColor;
    char fFgColor;
    char fBold;
    char fUnderline;
    char fItalic;
} CHP;                  /* CHaracter Properties */

typedef enum {justL, justR, justC, justF } JUST;
typedef struct para_prop
{
    int xaLeft;                 /* left indent in twips */
    int xaRight;                /* right indent in twips */
    int xaFirst;                /* first line indent in twips */
    JUST just;                  /* justification */
} PAP;                  /* PAragraph Properties */

typedef enum {sbkNon, sbkCol, sbkEvn, sbkOdd, sbkPg} SBK;
typedef enum {pgDec, pgURom, pgLRom, pgULtr, pgLLtr} PGN;
typedef struct sect_prop
{
    int cCols;                  /* number of columns */
    SBK sbk;                    /* section break type */
    int xaPgn;                  /* x position of page number in twips */
    int yaPgn;                  /* y position of page number in twips */
    PGN pgnFormat;              /* how the page number is formatted */
} SEP;                  /* SEction Properties */

typedef struct doc_prop
{
    int xaPage;                 /* page width in twips */
    int yaPage;                 /* page height in twips */
    int xaLeft;                 /* left margin in twips */
    int yaTop;                  /* top margin in twips */
    int xaRight;                /* right margin in twips */
    int yaBottom;               /* bottom margin in twips */
    int pgnStart;               /* starting page number in twips */
    char fFacingp;              /* facing pages enabled? */
    char fLandscape;            /* landscape or portrait?? */
} DOP;                  /* DOcument Properties */

typedef enum { rdsNorm, rdsSkip, rdsFontTable, rdsColorTable, rdsInfo, rdsTitle, rdsSubject, rdsAuthor } RDS;              /* Rtf Destination State */
typedef enum { risNorm, risBin, risHex } RIS;       /* Rtf Internal State */

typedef struct save             /* property save structure */
{
    struct save *pNext;         /* next save */
    CHP chp;
    PAP pap;
    SEP sep;
    DOP dop;
    RDS rds;
    RIS ris;
} SAVE;

/* What types of properties are there? */
typedef enum {ipropFontFamily, ipropColorRed, ipropColorGreen, ipropColorBlue,
              ipropFont, ipropFontSize, ipropBgColor, ipropFgColor,
              ipropBold, ipropItalic, ipropUnderline, ipropLeftInd,
              ipropRightInd, ipropFirstInd, ipropCols, ipropPgnX,
              ipropPgnY, ipropXaPage, ipropYaPage, ipropXaLeft,
              ipropXaRight, ipropYaTop, ipropYaBottom, ipropPgnStart,
              ipropSbk, ipropPgnFormat, ipropFacingp, ipropLandscape,
              ipropJust, ipropPard, ipropPlain, ipropSectd,
              ipropMax } IPROP;

typedef enum {actnSpec, actnByte, actnWord} ACTN;
typedef enum {propChp, propPap, propSep, propDop} PROPTYPE;

typedef struct propmod
{
    ACTN actn;              /* size of value */
    PROPTYPE prop;          /* structure containing value */
    int  offset;            /* offset of value from base of structure */
} PROP;

typedef enum {ipfnBin, ipfnHex, ipfnSkipDest } IPFN;
typedef enum {idestFontTable, idestColorTable, idestInfo, idestTitle, idestSubject, idestAuthor, idestPict, idestSkip } IDEST;
typedef enum {kwdChar, kwdDest, kwdProp, kwdSpec} KWD;

typedef struct symbol
{
    char *szKeyword;        /* RTF keyword */
    int  dflt;              /* default value to use */
    bool fPassDflt;         /* true to use default value from this table */
    KWD  kwd;               /* base action to take */
    int  idx;               /* index into property table if kwd == kwdProp */
                            /* index into destination table if kwd == kwdDest */
                            /* character to print if kwd == kwdChar */
} SYM;

typedef enum {fnil,froman,fswiss,fmodern,fscript,fdecor,ftech,fbidi} FFAM;

typedef struct _RTF_Font
{
    void *font;
    int size;
    int style;
    struct _RTF_Font *next;
} RTF_Font;

typedef struct _RTF_FontEntry
{
    int number;
    char *name;
    RTF_FontFamily family;
    RTF_Font *fonts;
    struct _RTF_FontEntry *next;
} RTF_FontEntry;

typedef struct _RTF_TextBlock
{
    void *font;
    SDL_Color color;
    int   tabs;
    char *text;
    int   numChars;
    int  *byteOffsets;
    int  *pixelOffsets;
    int   lineHeight;
    struct _RTF_TextBlock *next;
} RTF_TextBlock;

typedef struct _RTF_Surface
{
    int x, y;
    SDL_Surface *surface;
    struct _RTF_Surface *next;
} RTF_Surface;

typedef struct _RTF_Line
{
    PAP pap;
    int lineWidth;
    int lineHeight;
    int tabs;
    RTF_TextBlock *start;
    RTF_TextBlock *last;
    RTF_Surface *startSurface;
    RTF_Surface *lastSurface;
    struct _RTF_Line *next;
} RTF_Line;

struct _RTF_Context
{
    RTF_FontEngine fontEngine;

    /* Storage for parsing data */
    char *data;
    int  datapos;
    int  datamax;
    int  values[4];

    RTF_FontEntry *fontTable;
    SDL_Palette colorTable;

    char *title;
    char *subject;
    char *author;

    int cGroup;
    RDS rds;
    RIS ris;

    CHP chp;
    PAP pap;
    SEP sep;
    DOP dop;

    SAVE *psave;
    long cbBin;
    long lParam;
    bool fSkipDestIfUnk;

    /* Input data stream (can be non-seekable) */
    SDL_RWops *stream;
    int nextch;

    /* Display information */
    int displayWidth;
    int displayHeight;
    RTF_Line *start;    
    RTF_Line *last;
};