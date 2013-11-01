/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkParseString.c

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*-------------------------------------------------------------------------
  Copyright (c) 2012 David Gobbi.

  Contributed to the VisualizationToolkit by the author in April 2012
  under the terms of the Visualization Toolkit 2008 copyright.
-------------------------------------------------------------------------*/

#include "vtkParseString.h"
#include <stdlib.h>
#include <string.h>


/*----------------------------------------------------------------
 * String tokenization methods
 *
 * Strings must be broken into C++ tokens.
 * A hash is computed for ids, but not for other tokens.
 * Comments are generally considered to be whitespace, but
 * WS_COMMENT can be used to consider comments as tokens.
 */

/** Array for quick lookup of char types */
unsigned char parse_charbits[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0,
  CPRE_HSPACE, /* tab */
  CPRE_VSPACE, CPRE_VSPACE, CPRE_VSPACE, /* newline, vtab, form feed */
  CPRE_HSPACE, /* carriage return */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  CPRE_HSPACE, /* ' ' */
  0, CPRE_QUOTE, 0, 0, 0, 0, CPRE_QUOTE, 0, 0, /* !"#$%&'() */
  0, CPRE_SIGN, 0, CPRE_SIGN, 0, 0, /* *+,-./ */
  CPRE_DIGIT|CPRE_HEX, /* 0 */
  CPRE_DIGIT|CPRE_HEX, CPRE_DIGIT|CPRE_HEX,
  CPRE_DIGIT|CPRE_HEX, CPRE_DIGIT|CPRE_HEX,
  CPRE_DIGIT|CPRE_HEX, CPRE_DIGIT|CPRE_HEX,
  CPRE_DIGIT|CPRE_HEX, CPRE_DIGIT|CPRE_HEX,
  CPRE_DIGIT|CPRE_HEX, /* 9 */
  0, 0, 0, 0, 0, 0, 0, /* :;<=>?@ */
  CPRE_ID|CPRE_HEX, /* A */
  CPRE_ID|CPRE_HEX, CPRE_ID|CPRE_HEX, CPRE_ID|CPRE_HEX, /* BCD */
  CPRE_ID|CPRE_HEX|CPRE_EXP, /* E */
  CPRE_ID|CPRE_HEX, CPRE_ID, CPRE_ID, CPRE_ID, /* FGHI */
  CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, /* JKLM */
  CPRE_ID, CPRE_ID, CPRE_ID|CPRE_EXP, CPRE_ID, /* NOPQ */
  CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, /* RSTU */
  CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, /* VWXY */
  CPRE_ID, /* Z */
  0, 0, 0, 0, /* [\\]^ */
  CPRE_ID, /* _ */
  0, /* ` */
  CPRE_ID|CPRE_HEX, /* a */
  CPRE_ID|CPRE_HEX, CPRE_ID|CPRE_HEX, CPRE_ID|CPRE_HEX, /* bcd */
  CPRE_ID|CPRE_HEX|CPRE_EXP, /* e */
  CPRE_ID|CPRE_HEX, CPRE_ID, CPRE_ID, CPRE_ID, /* fghi */
  CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, /* jklm */
  CPRE_ID, CPRE_ID, CPRE_ID|CPRE_EXP, CPRE_ID, /* nopq */
  CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, /* rstu */
  CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, /* vwxy */
  CPRE_ID, /* z */
  0, 0, 0, 0, /* {|}~ */
  0, /* '\x7f' */
  CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID,
  CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID,
  CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID,
  CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID, CPRE_ID,
};

#define parse_chartype(c, bits) \
  ((parse_charbits[(unsigned char)(c)] & (bits)) != 0)

/** Skip over a comment. */
size_t vtkParse_SkipComment(const char *text)
{
  const char *cp = text;

  if (cp[0] == '/')
    {
    if (cp[1] == '/')
      {
      cp += 2;
      while (*cp != '\n' && *cp != '\0')
        {
        if (cp[0] == '\\')
          {
          if (cp[1] == '\n') { cp++; }
          else if (cp[1] == '\r' && cp[2] == '\n') { cp += 2; }
          }
        cp++;
        }
      }
    else if (cp[1] == '*')
      {
      cp += 2;
      while (*cp != '\0')
        {
        if (cp[0] == '*' && cp[1] == '/') { cp += 2; break; }
        cp++;
        }
      }
    }

  return cp - text;
}

/** Skip over whitespace. */
size_t vtkParse_SkipWhitespace(const char *text, parse_space_t spacetype)
{
  const char *cp = text;

  for (;;)
    {
    if (parse_chartype(*cp, spacetype))
      {
      do
        {
        cp++;
        }
      while (parse_chartype(*cp, spacetype));
      }
    if (cp[0] == '\\')
      {
      if (cp[1] == '\n')
        {
        cp += 2;
        }
      else if (cp[1] == '\r' && cp[2] == '\n')
        {
        cp += 3;
        }
      else
        {
        break;
        }
      }
    else if (cp[0] == '/' && (spacetype & WS_COMMENT) != WS_COMMENT)
      {
      if (cp[1] == '/' || cp[1] == '*')
        {
        cp += vtkParse_SkipComment(cp);
        }
      else
        {
        break;
        }
      }
    else
      {
      break;
      }
    }

  return cp - text;
}

/** Skip over string and char literals. */
size_t vtkParse_SkipQuotes(const char *text)
{
  const char *cp = text;
  const char qc = *cp;

  if (parse_chartype(*cp, CPRE_QUOTE))
    {
    cp++;
    while (*cp != qc && *cp != '\n' && *cp != '\0')
      {
      if (*cp++ == '\\')
        {
        if (cp[0] == '\r' && cp[1] == '\n') { cp += 2; }
        else if (*cp != '\0') { cp++; }
        }
      }
    }
  if (*cp == qc)
    {
    cp++;
    }

  return cp - text;
}

/** Skip over a number. */
size_t vtkParse_SkipNumber(const char *text)
{
  const char *cp = text;

  if (parse_chartype(cp[0], CPRE_DIGIT) ||
      (cp[0] == '.' && parse_chartype(cp[1], CPRE_DIGIT)))
    {
    do
      {
      char c = *cp++;
      if (parse_chartype(c, CPRE_EXP) &&
          parse_chartype(*cp, CPRE_SIGN))
        {
        cp++;
        }
      }
    while (parse_chartype(*cp, CPRE_IDGIT) || *cp == '.');
    }

  return cp - text;
}

/** Skip over a name. */
size_t vtkParse_SkipId(const char *text)
{
  const char *cp = text;

  if (parse_chartype(*cp, CPRE_ID))
    {
    do
      {
      cp++;
      }
    while (parse_chartype(*cp, CPRE_IDGIT));
    }

  return cp - text;
}

/** A simple 32-bit hash function based on "djb2". */
#define parse_hash_name(cp, h) \
  h = 5381; \
  do { h = (h << 5) + h + (unsigned char)*cp++; } \
  while (parse_chartype(*cp, CPRE_IDGIT));

unsigned int vtkParse_HashId(const char *cp)
{
  unsigned int h = 0;

  if (parse_chartype(*cp, CPRE_ID))
    {
    parse_hash_name(cp, h);
    }

  return h;
}

/** Skip a string or */
size_t parse_skip_quotes_with_suffix(const char *cp)
{
  size_t l = vtkParse_SkipQuotes(cp);
  if (l && cp[l] == '_')
    {
    l += vtkParse_SkipId(cp + l);
    }
  return l;
}

/** Return the next token, or 0 if none left. */
int vtkParse_NextToken(StringTokenizer *tokens)
{
  const char *cp = tokens->text + tokens->len;

  /* avoid extra function call for simple whitespace */
  if (parse_chartype(*cp, tokens->ws))
    {
    do { cp++; } while (parse_chartype(*cp, tokens->ws));
    }
  /* function call is necessary if slash encountered */
  if (*cp == '/' || *cp == '\\')
    {
    cp += vtkParse_SkipWhitespace(cp, tokens->ws);
    }

  if (parse_chartype(*cp, CPRE_ID))
    {
    const char *ep = cp;
    unsigned int h;

    /* use a macro to compute the hash */
    parse_hash_name(ep, h);

    tokens->tok = TOK_ID;
    tokens->hash = h;
    tokens->text = cp;
    tokens->len = ep - cp;

    /* check if this is a prefixed string */
    if (parse_chartype(*ep, CPRE_QUOTE) &&
        ((*ep == '\'' && tokens->len == 1 &&
          (*cp == 'u' || *cp == 'U' || *cp == 'L')) ||
         (*ep == '\"' && tokens->len == 1 &&
          (*cp == 'U' || *cp == 'u' || *cp == 'L')) ||
         (*ep == '\"' && tokens->len == 2 && cp[0] == 'u' && cp[1] == '8')))
      {
      tokens->tok = (*ep == '\"' ? TOK_STRING : TOK_CHAR);
      tokens->hash = 0;
      tokens->len += parse_skip_quotes_with_suffix(ep);
      }
    }
  else if (parse_chartype(*cp, CPRE_QUOTE))
    {
    tokens->tok = (*cp == '\"' ? TOK_STRING : TOK_CHAR);
    tokens->hash = 0;
    tokens->text = cp;
    tokens->len = parse_skip_quotes_with_suffix(cp);
    }
  else if (parse_chartype(*cp, CPRE_DIGIT) ||
           (cp[0] == '.' && parse_chartype(cp[1], CPRE_DIGIT)))
    {
    tokens->tok = TOK_NUMBER;
    tokens->hash = 0;
    tokens->text = cp;
    tokens->len = vtkParse_SkipNumber(cp);
    }
  else if (cp[0] == '/' && (cp[1] == '/' || cp[1] == '*'))
    {
    tokens->tok = TOK_COMMENT;
    tokens->hash = 0;
    tokens->text = cp;
    tokens->len = vtkParse_SkipComment(cp);
    }
  else
    {
    int t = cp[0];
    size_t l = 1;

    switch (cp[0])
      {
      case ':':
        if (cp[1] == ':') { l = 2; t = TOK_SCOPE; }
        break;
      case '.':
        if (cp[1] == '.' && cp[2] == '.') { l = 3; t = TOK_ELLIPSIS; }
        else if (cp[1] == '*') { l = 2; t = TOK_DOT_STAR; }
        break;
      case '=':
        if (cp[1] == '=') { l = 2; t = TOK_EQ; }
        break;
      case '!':
        if (cp[1] == '=') { l = 2; t = TOK_NE; }
        break;
      case '<':
        if (cp[1] == '<' && cp[2] == '=') { l = 3; t = TOK_LSHIFT_EQ; }
        else if (cp[1] == '<') { l = 2; t = TOK_LSHIFT; }
        else if (cp[1] == '=') { l = 2; t = TOK_LE; }
        break;
      case '>':
        if (cp[1] == '>' && cp[2] == '=') { l = 3; t = TOK_RSHIFT_EQ; }
        else if (cp[1] == '>') { l = 2; t = TOK_RSHIFT; }
        else if (cp[1] == '=') { l = 2; t = TOK_GE; }
        break;
      case '&':
        if (cp[1] == '=') { l = 2; t = TOK_AND_EQ; }
        else if (cp[1] == '&') { l = 2; t = TOK_AND; }
        break;
      case '|':
        if (cp[1] == '=') { l = 2; t = TOK_OR_EQ; }
        else if (cp[1] == '|') { l = 2; t = TOK_OR; }
        break;
      case '^':
        if (cp[1] == '=') { l = 2; t = TOK_XOR_EQ; }
        break;
      case '*':
        if (cp[1] == '=') { l = 2; t = TOK_MUL_EQ; }
        break;
      case '/':
        if (cp[1] == '=') { l = 2; t = TOK_DIV_EQ; }
        break;
      case '%':
        if (cp[1] == '=') { l = 2; t = TOK_MOD_EQ; }
        break;
      case '+':
        if (cp[1] == '+') { l = 2; t = TOK_INCR; }
        else if (cp[1] == '=') { l = 2; t = TOK_ADD_EQ; }
        break;
      case '-':
        if (cp[1] == '>' && cp[2] == '*') { l = 3; t = TOK_ARROW_STAR; }
        else if (cp[1] == '>') { l = 2; t = TOK_ARROW; }
        else if (cp[1] == '-') { l = 2; t = TOK_DECR; }
        else if (cp[1] == '=') { l = 2; t = TOK_SUB_EQ; }
        break;
      case '#':
        if (cp[1] == '#') { l = 2; t = TOK_DBLHASH; }
        break;
      case '\n':
      case '\0':
        { l = 0; t = 0; }
        break;
      }

    tokens->tok = t;
    tokens->hash = 0;
    tokens->text = cp;
    tokens->len = l;
    }

  return tokens->tok;
}

/** Initialize the tokenizer. */
void vtkParse_InitTokenizer(
  StringTokenizer *tokens, const char *text, parse_space_t wstype)
{
  tokens->tok = 0;
  tokens->hash = 0;
  tokens->text = text;
  tokens->len = 0;
  tokens->ws = wstype;
  vtkParse_NextToken(tokens);
}


/*----------------------------------------------------------------
 * String allocation methods
 *
 * Strings are centrally allocated and are const.  They should not
 * be freed until the parse is complete and all the data structures
 * generated by the parse have been freed.
 */

/* allocate a string of n+1 bytes */
void vtkParse_InitStringCache(StringCache *cache)
{
  cache->NumberOfChunks = 0;
  cache->Chunks = NULL;
  cache->ChunkSize = 0;
  cache->Position = 0;
}

/* allocate a string of n+1 bytes */
char *vtkParse_NewString(StringCache *cache, size_t n)
{
  size_t nextPosition;
  char *cp;

  if (cache->ChunkSize == 0)
    {
    cache->ChunkSize = 8176;
    }

  // align next start position on an 8-byte boundary
  nextPosition = (((cache->Position + n + 8) | 7 ) - 7);

  if (cache->NumberOfChunks == 0 || nextPosition > cache->ChunkSize)
    {
    if (n + 1 > cache->ChunkSize)
      {
      cache->ChunkSize = n + 1;
      }
    cp = (char *)malloc(cache->ChunkSize);

    /* if empty, alloc for the first time */
    if (cache->NumberOfChunks == 0)
      {
      cache->Chunks = (char **)malloc(sizeof(char *));
      }
    /* if count is power of two, reallocate with double size */
    else if ((cache->NumberOfChunks & (cache->NumberOfChunks-1)) == 0)
      {
      cache->Chunks = (char **)realloc(
        cache->Chunks, (2*cache->NumberOfChunks)*sizeof(char *));
      }

    cache->Chunks[cache->NumberOfChunks++] = cp;

    cache->Position = 0;
    nextPosition = (((n + 8) | 7) - 7);
    }

  cp = &cache->Chunks[cache->NumberOfChunks-1][cache->Position];
  cp[0] = '\0';

  cache->Position = nextPosition;

  return cp;
}

/* free all allocated strings */
void vtkParse_FreeStringCache(StringCache *cache)
{
  unsigned long i;

  for (i = 0; i < cache->NumberOfChunks; i++)
    {
    free(cache->Chunks[i]);
    }
  if (cache->Chunks)
    {
    free(cache->Chunks);
    }

  cache->Chunks = NULL;
  cache->NumberOfChunks = 0;
}

/* duplicate the first n bytes of a string and terminate it */
const char *vtkParse_CacheString(StringCache *cache, const char *in, size_t n)
{
  char *res = NULL;

  res = vtkParse_NewString(cache, n);
  strncpy(res, in, n);
  res[n] = '\0';

  return res;
}
