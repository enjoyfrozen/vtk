/* A Bison parser, made by GNU Bison 3.2.3.  */

/* Skeleton implementation for Bison GLR parsers in C

   Copyright (C) 2002-2015, 2018 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C GLR parser skeleton written by Paul Hilfinger.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "glr.c"

/* Pure parsers.  */
#define YYPURE 0

/* First part of user prologue.  */

/*

This file must be translated to C and modified to build everywhere.

Run bison like this (use bison 3.2.3 or later)

  bison --no-lines -b vtkParse vtkParse.y

Modify vtkParse.tab.c:
  - replace all instances of "static inline" with "static"
  - replace "#if ! defined lint || defined __GNUC__" with "#if 1"
  - remove YY_ATTRIBUTE_UNUSED from yyfillin, yyfill, and yynormal
  - remove the "break;" after "return yyreportAmbiguity"
  - replace "(1-yyrhslen)" with "(1-(int)yyrhslen)"
  - replace "sizeof yynewStates[0] with "sizeof (yyGLRState*)"
  - replace "sizeof yynewLookaheadNeeds[0] with "sizeof (yybool)"
*/

/*
The purpose of this parser is to read C++ header files in order to
generate data structures that describe the C++ interface of a library,
one header file at a time.  As such, it is not a complete C++ parser.
It only parses what is relevant to the interface and skips the rest.

While the parser reads method definitions, type definitions, and
template definitions it generates a "signature" which is a string
that matches (apart from whitespace) the text that was parsed.

While parsing types, the parser creates an unsigned int that describes
the type as well as creating other data structures for arrays, function
pointers, etc.  The parser also creates a typeId string, which is either
a simple id that gives the class name or type name, or is "function" for
function pointer types, or "method" for method pointer types.
*/

/*
Conformance Notes:

This parser was designed empirically and incrementally.  It has been
refactored to make it more similar to the C++ 1998 grammar, but there
are still many very significant differences.

The most significant difference between this parser and a "standard"
parser is that it only parses declarations in detail.  All other
statements and expressions are parsed as arbitrary sequences of symbols,
without any syntactic analysis.

The "unqualified_id" does not directly include "operator_function_id" or
"conversion_function_id" (e.g. ids like "operator=" or "operator int*").
Instead, these two id types are used to allow operator functions to be
handled by their own rules, rather than by the generic function rules.
These ids can only be used in function declarations and using declarations.

Types are handled quite differently from the C++ BNF.  These differences
represent a prolonged (and ultimately successful) attempt to empirically
create a yacc parser without any shift/reduce conflicts.  The rules for
types are organized according to the way that types are usually defined
in working code, rather than strictly according to C++ grammar.

The declaration specifier "typedef" can only appear at the beginning
of a declaration sequence.  There are also restrictions on where class
and enum specifiers can be used: you can declare a new struct within a
variable declaration, but not within a parameter declaration.

The lexer returns each of "(scope::*", "(*", "(a::b::*", etc. as single
tokens.  The C++ BNF, in contrast, would consider these to be a "("
followed by a "ptr_operator".  The lexer concatenates these tokens in
order to eliminate shift/reduce conflicts in the parser.  However, this
means that this parser will only recognize "scope::*" as valid if it is
preceded by "(", e.g. as part of a member function pointer specification.

Variables that are initialized via constructor arguments, for example
"someclass variablename(arglist)", must take a literals as the first
argument.  If an identifier is used as the first argument, then the
parser will interpret the variable declaration as a function declaration,
since the parser will assume the identifier names a type.

An odd bit of C++ ambiguity is that y(x); can be interpreted variously
as declaration of variable "x" of type "y", as a function call if "y"
is the name of a function, or as a constructor if "y" is the name of
a class.  This parser always interprets this pattern as a constructor
declaration, because function calls are ignored by the parser, and
variable declarations of the form y(x); are exceedingly rare compared
to the more usual form y x; without parentheses.
*/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define yyerror(a) print_parser_error(a, NULL, 0)
#define yywrap() 1

/* Make sure yacc-generated code knows we have included stdlib.h.  */
#ifndef _STDLIB_H
#define _STDLIB_H
#endif
#define YYINCLUDED_STDLIB_H

/* MSVC does not define __STDC__ properly. */
#if !defined(__STDC__)
#if defined(_MSC_VER)
#define __STDC__ 1
#endif
#endif

/* Disable warnings in generated code. */
#if defined(_MSC_VER)
#pragma warning(disable : 4127) /* conditional expression is constant */
#pragma warning(disable : 4244) /* conversion to smaller integer type */
#endif

#include "vtkParse.h"
#include "vtkParseData.h"
#include "vtkParsePreprocess.h"

/* Define the kinds of [[attributes]] to collect */
enum
{
  VTK_PARSE_ATTRIB_NONE,
  VTK_PARSE_ATTRIB_DECL,  /* modify a declaration */
  VTK_PARSE_ATTRIB_ID,    /* modify an id */
  VTK_PARSE_ATTRIB_REF,   /* modify *, &, or && */
  VTK_PARSE_ATTRIB_FUNC,  /* modify a function or method */
  VTK_PARSE_ATTRIB_ARRAY, /* modify an array size specifier */
  VTK_PARSE_ATTRIB_CLASS  /* modify class, struct, union, or enum */
};

#define vtkParseDebug(s1, s2)                                                                      \
  if (parseDebug)                                                                                  \
  {                                                                                                \
    fprintf(stderr, "   %s %s\n", s1, s2);                                                         \
  }

/* the tokenizer */
int yylex(void);

/* global variables */
FileInfo* data = NULL;
int parseDebug;

/* globals for cacheing directory listings */
static StringCache system_strings = { 0, 0, 0, 0 };
static SystemInfo system_cache = { &system_strings, NULL, NULL };

/* the "preprocessor" */
PreprocessInfo* preprocessor = NULL;

/* whether to pre-define platform-specific macros */
int PredefinePlatformMacros = 1;

/* include dirs specified on the command line */
int NumberOfIncludeDirectories = 0;
const char** IncludeDirectories;

/* macros specified on the command line */
int NumberOfDefinitions = 0;
const char** Definitions;

/* include specified on the command line */
int NumberOfMacroIncludes = 0;
const char** MacroIncludes;

/* for dumping diagnostics about macros */
int DumpMacros = 0;
const char* DumpFileName = NULL;

/* options that can be set by the programs that use the parser */
int Recursive = 0;
const char* CommandName = NULL;

/* various state variables */
NamespaceInfo* currentNamespace = NULL;
ClassInfo* currentClass = NULL;
FunctionInfo* currentFunction = NULL;
TemplateInfo* currentTemplate = NULL;
const char* currentEnumName = NULL;
const char* currentEnumValue = NULL;
unsigned int currentEnumType = 0;
const char* deprecationReason = NULL;
const char* deprecationVersion = NULL;
parse_access_t access_level = VTK_ACCESS_PUBLIC;

/* functions from vtkParse.l */
void print_parser_error(const char* text, const char* cp, size_t n);

/* helper functions */
static const char* type_class(unsigned int type, const char* classname);
static void start_class(const char* classname, int is_struct_or_union);
static void end_class(void);
static void add_base_class(ClassInfo* cls, const char* name, int access_lev, unsigned int extra);
static void output_friend_function(void);
static void output_function(void);
static void reject_function(void);
static void set_return(
  FunctionInfo* func, unsigned int attributes, unsigned int type, const char* typeclass, int count);
static void add_template_parameter(unsigned int datatype, unsigned int extra, const char* funcSig);
static void add_using(const char* name, int is_namespace);
static void start_enum(const char* name, int is_scoped, unsigned int type, const char* basename);
static void add_enum(const char* name, const char* value);
static void end_enum(void);
static unsigned int guess_constant_type(const char* valstring);
static void add_constant(const char* name, const char* value, unsigned int attributes,
  unsigned int type, const char* typeclass, int flag);
static unsigned int guess_id_type(const char* cp);
static unsigned int add_indirection(unsigned int type1, unsigned int type2);
static unsigned int add_indirection_to_array(unsigned int type);
static void handle_complex_type(ValueInfo* val, unsigned int attributes, unsigned int datatype,
  unsigned int extra, const char* funcSig);
static void handle_attribute(const char* att, int pack);
static void add_legacy_parameter(FunctionInfo* func, ValueInfo* param);

/*----------------------------------------------------------------
 * String utility methods
 *
 * Strings are centrally allocated and are const, and they are not
 * freed until the program exits.  If they need to be freed before
 * then, vtkParse_FreeStringCache() can be called.
 */

/* duplicate the first n bytes of a string and terminate */
static const char* vtkstrndup(const char* in, size_t n)
{
  return vtkParse_CacheString(data->Strings, in, n);
}

/* duplicate a string */
static const char* vtkstrdup(const char* in)
{
  if (in)
  {
    in = vtkParse_CacheString(data->Strings, in, strlen(in));
  }

  return in;
}

/* helper function for concatenating strings */
static const char* vtkstrncat(size_t n, const char** str)
{
  char* cp;
  size_t i;
  size_t j[8];
  size_t m = 0;

  for (i = 0; i < n; i++)
  {
    j[i] = 0;
    if (str[i])
    {
      j[i] = strlen(str[i]);
      m += j[i];
    }
  }
  cp = vtkParse_NewString(data->Strings, m);
  m = 0;
  for (i = 0; i < n; i++)
  {
    if (j[i])
    {
      strncpy(&cp[m], str[i], j[i]);
      m += j[i];
    }
  }
  cp[m] = '\0';

  return cp;
}

/* concatenate strings */
static const char* vtkstrcat(const char* str1, const char* str2)
{
  const char* cp[2];

  cp[0] = str1;
  cp[1] = str2;
  return vtkstrncat(2, cp);
}

static const char* vtkstrcat3(const char* str1, const char* str2, const char* str3)
{
  const char* cp[3];

  cp[0] = str1;
  cp[1] = str2;
  cp[2] = str3;
  return vtkstrncat(3, cp);
}

static const char* vtkstrcat4(
  const char* str1, const char* str2, const char* str3, const char* str4)
{
  const char* cp[4];

  cp[0] = str1;
  cp[1] = str2;
  cp[2] = str3;
  cp[3] = str4;
  return vtkstrncat(4, cp);
}

/*----------------------------------------------------------------
 * Comments
 */

enum comment_enum
{
  ClosedComment = -2,
  StickyComment = -1,
  NoComment = 0,
  NormalComment = 1,
  NameComment = 2,
  DescriptionComment = 3,
  SeeAlsoComment = 4,
  CaveatsComment = 5,
  DoxygenComment = 6,
  TrailingComment = 7
};

/* "private" variables */
char* commentText = NULL;
size_t commentLength = 0;
size_t commentAllocatedLength = 0;
int commentState = 0;
int commentMemberGroup = 0;
int commentGroupDepth = 0;
parse_dox_t commentType = DOX_COMMAND_OTHER;
const char* commentTarget = NULL;

/* Struct for recognizing certain doxygen commands */
struct DoxygenCommandInfo
{
  const char* name;
  size_t length;
  parse_dox_t type;
};

/* List of doxygen commands (@cond is not handled yet) */
/* clang-format off */
struct DoxygenCommandInfo doxygenCommands[] = {
  { "def", 3, DOX_COMMAND_DEF },
  { "category", 8, DOX_COMMAND_CATEGORY },
  { "interface", 9, DOX_COMMAND_INTERFACE },
  { "protocol", 8, DOX_COMMAND_PROTOCOL },
  { "class", 5, DOX_COMMAND_CLASS },
  { "enum", 4, DOX_COMMAND_ENUM },
  { "struct", 6, DOX_COMMAND_STRUCT },
  { "union", 5, DOX_COMMAND_UNION },
  { "namespace", 9, DOX_COMMAND_NAMESPACE },
  { "typedef", 7, DOX_COMMAND_TYPEDEF },
  { "fn", 2, DOX_COMMAND_FN },
  { "property", 8, DOX_COMMAND_PROPERTY },
  { "var", 3, DOX_COMMAND_VAR },
  { "name", 4, DOX_COMMAND_NAME },
  { "defgroup", 8, DOX_COMMAND_DEFGROUP },
  { "addtogroup", 10, DOX_COMMAND_ADDTOGROUP },
  { "weakgroup", 9, DOX_COMMAND_WEAKGROUP },
  { "example", 7, DOX_COMMAND_EXAMPLE },
  { "file", 4, DOX_COMMAND_FILE },
  { "dir", 3, DOX_COMMAND_DIR },
  { "mainpage", 8, DOX_COMMAND_MAINPAGE },
  { "page", 4, DOX_COMMAND_PAGE },
  { "subpage", 7, DOX_COMMAND_SUBPAGE },
  { "internal", 8, DOX_COMMAND_INTERNAL },
  { "package", 7, DOX_COMMAND_PACKAGE },
  { "privatesection", 14, DOX_COMMAND_PRIVATESECTION },
  { "protectedsection", 16, DOX_COMMAND_PROTECTEDSECTION },
  { "publicsection", 13, DOX_COMMAND_PUBLICSECTION },
  { NULL, 0, DOX_COMMAND_OTHER }
};
/* clang-format on */

void closeComment(void);

/* Clear the comment buffer */
void clearComment(void)
{
  commentLength = 0;
  if (commentText)
  {
    commentText[commentLength] = '\0';
  }
  commentState = 0;
  commentType = DOX_COMMAND_OTHER;
}

/* This is called when entering or leaving a comment block */
void setCommentState(int state)
{
  switch (state)
  {
    case 0:
      closeComment();
      break;
    default:
      closeComment();
      clearComment();
      break;
  }

  commentState = state;
}

/* Get the text from the comment buffer */
static const char* getComment(void)
{
  const char* text = commentText;
  const char* cp = commentText;
  size_t l = commentLength;

  if (commentText != NULL && commentState != 0)
  {
    /* strip trailing blank lines */
    while (
      l > 0 && (cp[l - 1] == ' ' || cp[l - 1] == '\t' || cp[l - 1] == '\r' || cp[l - 1] == '\n'))
    {
      if (cp[l - 1] == '\n')
      {
        commentLength = l;
      }
      l--;
    }
    commentText[commentLength] = '\0';
    /* strip leading blank lines */
    while (*cp == ' ' || *cp == '\t' || *cp == '\r' || *cp == '\n')
    {
      if (*cp == '\n')
      {
        text = cp + 1;
      }
      cp++;
    }
    return text;
  }

  return NULL;
}

/* Check for doxygen commands that mark unwanted comments */
static parse_dox_t checkDoxygenCommand(const char* text, size_t n)
{
  struct DoxygenCommandInfo* info;
  for (info = doxygenCommands; info->name; info++)
  {
    if (info->length == n && strncmp(text, info->name, n) == 0)
    {
      return info->type;
    }
  }
  return DOX_COMMAND_OTHER;
}

/* This is called whenever a comment line is encountered */
void addCommentLine(const char* line, size_t n, int type)
{
  size_t i, j;
  parse_dox_t t = DOX_COMMAND_OTHER;

  if (type == DoxygenComment || commentState == DoxygenComment)
  {
    if (type == DoxygenComment)
    {
      /* search for '@' and backslash */
      for (i = 0; i + 1 < n; i++)
      {
        if (line[i] == '@' || line[i] == '\\')
        {
          j = ++i;
          while (i < n && line[i] >= 'a' && line[i] <= 'z')
          {
            i++;
          }
          if (line[i - 1] == '@' && (line[i] == '{' || line[i] == '}'))
          {
            if (line[i] == '{')
            {
              commentGroupDepth++;
            }
            else
            {
              --commentGroupDepth;
            }
            closeComment();
            return;
          }
          else
          {
            /* record the type of this comment */
            t = checkDoxygenCommand(&line[j], i - j);
            if (t != DOX_COMMAND_OTHER)
            {
              while (i < n && line[i] == ' ')
              {
                i++;
              }
              j = i;
              while (i < n && vtkParse_CharType(line[i], CPRE_XID))
              {
                i++;
              }
              commentTarget = vtkstrndup(&line[j], i - j);
              /* remove this line from the comment */
              n = 0;
            }
          }
        }
      }
    }
    else if (commentState == DoxygenComment)
    {
      return;
    }
    if (commentState != type)
    {
      setCommentState(type);
    }
    if (t != DOX_COMMAND_OTHER)
    {
      commentType = t;
    }
  }
  else if (type == TrailingComment)
  {
    if (commentState != type)
    {
      setCommentState(type);
    }
  }
  else if (commentState == 0 || commentState == StickyComment || commentState == ClosedComment)
  {
    clearComment();
    return;
  }

  if (commentText == NULL)
  {
    commentAllocatedLength = n + 80;
    commentText = (char*)malloc(commentAllocatedLength);
    commentLength = 0;
    commentText[0] = '\0';
  }
  else if (commentLength + n + 2 > commentAllocatedLength)
  {
    commentAllocatedLength = commentAllocatedLength + commentLength + n + 2;
    commentText = (char*)realloc(commentText, commentAllocatedLength);
    if (!commentText)
    {
      fprintf(stderr, "Wrapping: out of memory\n");
      exit(1);
    }
  }

  if (n > 0)
  {
    memcpy(&commentText[commentLength], line, n);
  }
  commentLength += n;
  commentText[commentLength++] = '\n';
  commentText[commentLength] = '\0';
}

/* Store a doxygen comment */
static void storeComment(void)
{
  CommentInfo* info = (CommentInfo*)malloc(sizeof(CommentInfo));
  vtkParse_InitComment(info);
  info->Type = commentType;
  info->Name = commentTarget;
  info->Comment = vtkstrdup(getComment());

  if (commentType >= DOX_COMMAND_DEFGROUP)
  {
    /* comment has no scope, it is global to the project */
    vtkParse_AddCommentToNamespace(data->Contents, info);
  }
  else
  {
    /* comment is scoped to current namespace */
    if (currentClass)
    {
      vtkParse_AddCommentToClass(currentClass, info);
    }
    else
    {
      vtkParse_AddCommentToNamespace(currentNamespace, info);
    }
  }
}

/* Apply a doxygen trailing comment to the previous item */
static void applyComment(ClassInfo* cls)
{
  int i;
  ItemInfo* item;
  const char* comment = vtkstrdup(getComment());

  i = cls->NumberOfItems;
  if (i > 0)
  {
    item = &cls->Items[--i];
    if (item->Type == VTK_NAMESPACE_INFO)
    {
      cls->Namespaces[item->Index]->Comment = comment;
    }
    else if (item->Type == VTK_CLASS_INFO || item->Type == VTK_STRUCT_INFO ||
      item->Type == VTK_UNION_INFO)
    {
      cls->Classes[item->Index]->Comment = comment;
    }
    else if (item->Type == VTK_ENUM_INFO)
    {
      cls->Enums[item->Index]->Comment = comment;
    }
    else if (item->Type == VTK_FUNCTION_INFO)
    {
      cls->Functions[item->Index]->Comment = comment;
    }
    else if (item->Type == VTK_VARIABLE_INFO)
    {
      cls->Variables[item->Index]->Comment = comment;
    }
    else if (item->Type == VTK_CONSTANT_INFO)
    {
      cls->Constants[item->Index]->Comment = comment;
    }
    else if (item->Type == VTK_TYPEDEF_INFO)
    {
      cls->Typedefs[item->Index]->Comment = comment;
    }
    else if (item->Type == VTK_USING_INFO)
    {
      cls->Usings[item->Index]->Comment = comment;
    }
  }
}

/* This is called when a comment block ends */
void closeComment(void)
{
  const char* cp;
  size_t l;

  switch (commentState)
  {
    case ClosedComment:
      clearComment();
      break;
    case NormalComment:
      /* Make comment persist until a new comment starts */
      commentState = StickyComment;
      break;
    case NameComment:
      /* For NameComment, strip the comment */
      cp = getComment();
      l = strlen(cp);
      while (l > 0 && (cp[l - 1] == '\n' || cp[l - 1] == '\r' || cp[l - 1] == ' '))
      {
        l--;
      }
      data->NameComment = vtkstrndup(cp, l);
      clearComment();
      break;
    case DescriptionComment:
      data->Description = vtkstrdup(getComment());
      clearComment();
      break;
    case SeeAlsoComment:
      data->SeeAlso = vtkstrdup(getComment());
      clearComment();
      break;
    case CaveatsComment:
      data->Caveats = vtkstrdup(getComment());
      clearComment();
      break;
    case DoxygenComment:
      if (commentType == DOX_COMMAND_OTHER)
      {
        /* Apply only to next item unless within a member group */
        commentState = (commentMemberGroup ? StickyComment : ClosedComment);
      }
      else
      {
        /* Comment might not apply to next item, so store it */
        storeComment();
        clearComment();
      }
      break;
    case TrailingComment:
      if (currentClass)
      {
        applyComment(currentClass);
      }
      else
      {
        applyComment(currentNamespace);
      }
      clearComment();
      break;
  }
}

/* This is called when a blank line occurs in the header file */
void commentBreak(void)
{
  if (!commentMemberGroup && commentState == StickyComment)
  {
    clearComment();
  }
  else if (commentState == DoxygenComment)
  {
    /* blank lines only end targeted doxygen comments */
    if (commentType != DOX_COMMAND_OTHER)
    {
      closeComment();
    }
  }
  else
  {
    /* blank lines end VTK comments */
    closeComment();
  }
}

/* This is called when doxygen @{ or @} are encountered */
void setCommentMemberGroup(int g)
{
  commentMemberGroup = g;
  clearComment();
}

/* Assign comments to the items that they apply to */
void assignComments(ClassInfo* cls)
{
  int i, j;
  int t;
  const char* name;
  const char* comment;

  for (i = 0; i < cls->NumberOfComments; i++)
  {
    t = cls->Comments[i]->Type;
    name = cls->Comments[i]->Name;
    comment = cls->Comments[i]->Comment;
    /* find the item the comment applies to */
    if (t == DOX_COMMAND_CLASS || t == DOX_COMMAND_STRUCT || t == DOX_COMMAND_UNION)
    {
      for (j = 0; j < cls->NumberOfClasses; j++)
      {
        if (cls->Classes[j]->Name && name && strcmp(cls->Classes[j]->Name, name) == 0)
        {
          cls->Classes[j]->Comment = comment;
          break;
        }
      }
    }
    else if (t == DOX_COMMAND_ENUM)
    {
      for (j = 0; j < cls->NumberOfEnums; j++)
      {
        if (cls->Enums[j]->Name && name && strcmp(cls->Enums[j]->Name, name) == 0)
        {
          cls->Enums[j]->Comment = comment;
          break;
        }
      }
    }
    else if (t == DOX_COMMAND_TYPEDEF)
    {
      for (j = 0; j < cls->NumberOfTypedefs; j++)
      {
        if (cls->Typedefs[j]->Name && name && strcmp(cls->Typedefs[j]->Name, name) == 0)
        {
          cls->Typedefs[j]->Comment = comment;
          break;
        }
      }
    }
    else if (t == DOX_COMMAND_FN)
    {
      for (j = 0; j < cls->NumberOfFunctions; j++)
      {
        if (cls->Functions[j]->Name && name && strcmp(cls->Functions[j]->Name, name) == 0)
        {
          cls->Functions[j]->Comment = comment;
          break;
        }
      }
    }
    else if (t == DOX_COMMAND_VAR)
    {
      for (j = 0; j < cls->NumberOfVariables; j++)
      {
        if (cls->Variables[j]->Name && name && strcmp(cls->Variables[j]->Name, name) == 0)
        {
          cls->Variables[j]->Comment = comment;
          break;
        }
      }
      for (j = 0; j < cls->NumberOfConstants; j++)
      {
        if (cls->Constants[j]->Name && name && strcmp(cls->Constants[j]->Name, name) == 0)
        {
          cls->Constants[j]->Comment = comment;
          break;
        }
      }
    }
    else if (t == DOX_COMMAND_NAMESPACE)
    {
      for (j = 0; j < cls->NumberOfNamespaces; j++)
      {
        if (cls->Namespaces[j]->Name && name && strcmp(cls->Namespaces[j]->Name, name) == 0)
        {
          cls->Namespaces[j]->Comment = comment;
          break;
        }
      }
    }
  }

  /* recurse into child classes */
  for (i = 0; i < cls->NumberOfClasses; i++)
  {
    if (cls->Classes[i])
    {
      assignComments(cls->Classes[i]);
    }
  }

  /* recurse into child namespaces */
  for (i = 0; i < cls->NumberOfNamespaces; i++)
  {
    if (cls->Namespaces[i])
    {
      assignComments(cls->Namespaces[i]);
    }
  }
}

/*----------------------------------------------------------------
 * Macros
 */

/* "private" variables */
const char* macroName = NULL;
int macroUsed = 0;
int macroEnded = 0;

static const char* getMacro(void)
{
  if (macroUsed == 0)
  {
    macroUsed = macroEnded;
    return macroName;
  }
  return NULL;
}

/*----------------------------------------------------------------
 * Namespaces
 *
 * operates on: currentNamespace
 */

/* "private" variables */
NamespaceInfo* namespaceStack[10];
int namespaceDepth = 0;

/* enter a namespace */
static void pushNamespace(const char* name)
{
  int i;
  NamespaceInfo* oldNamespace = currentNamespace;

  for (i = 0; i < oldNamespace->NumberOfNamespaces; i++)
  {
    /* see if the namespace already exists */
    if (strcmp(name, oldNamespace->Namespaces[i]->Name) == 0)
    {
      currentNamespace = oldNamespace->Namespaces[i];
    }
  }

  /* create a new namespace */
  if (i == oldNamespace->NumberOfNamespaces)
  {
    currentNamespace = (NamespaceInfo*)malloc(sizeof(NamespaceInfo));
    vtkParse_InitNamespace(currentNamespace);
    currentNamespace->Name = name;
    vtkParse_AddNamespaceToNamespace(oldNamespace, currentNamespace);
  }

  namespaceStack[namespaceDepth++] = oldNamespace;
}

/* leave the namespace */
static void popNamespace(void)
{
  currentNamespace = namespaceStack[--namespaceDepth];
}

/*----------------------------------------------------------------
 * Classes
 *
 * operates on: currentClass, access_level
 */

/* "private" variables */
ClassInfo* classStack[10];
parse_access_t classAccessStack[10];
int classDepth = 0;

/* start an internal class definition */
static void pushClass(void)
{
  classAccessStack[classDepth] = access_level;
  classStack[classDepth++] = currentClass;
}

/* leave the internal class */
static void popClass(void)
{
  currentClass = classStack[--classDepth];
  access_level = classAccessStack[classDepth];
}

/*----------------------------------------------------------------
 * Templates
 *
 * operates on: currentTemplate
 */

/* "private" variables */
TemplateInfo* templateStack[10];
int templateDepth = 0;

/* begin a template */
static void startTemplate(void)
{
  currentTemplate = (TemplateInfo*)malloc(sizeof(TemplateInfo));
  vtkParse_InitTemplate(currentTemplate);
}

/* clear a template, if set */
static void clearTemplate(void)
{
  if (currentTemplate)
  {
    free(currentTemplate);
  }
  currentTemplate = NULL;
}

/* push the template onto the stack, and start a new one */
static void pushTemplate(void)
{
  templateStack[templateDepth++] = currentTemplate;
  startTemplate();
}

/* pop a template off the stack */
static void popTemplate(void)
{
  currentTemplate = templateStack[--templateDepth];
}

/*----------------------------------------------------------------
 * Function signatures
 *
 * operates on: currentFunction
 */

/* "private" variables */
int sigClosed = 0;
size_t sigMark[10];
size_t sigLength = 0;
size_t sigAllocatedLength = 0;
int sigMarkDepth = 0;
char* signature = NULL;

/* start a new signature */
static void startSig(void)
{
  signature = NULL;
  sigLength = 0;
  sigAllocatedLength = 0;
  sigClosed = 0;
  sigMarkDepth = 0;
  sigMark[0] = 0;
}

/* get the signature */
static const char* getSig(void)
{
  return signature;
}

/* get the signature length */
static size_t getSigLength(void)
{
  return sigLength;
}

/* reallocate Signature if n chars cannot be appended */
static void checkSigSize(size_t n)
{
  const char* ccp;

  if (sigAllocatedLength == 0)
  {
    sigLength = 0;
    sigAllocatedLength = 80 + n;
    signature = vtkParse_NewString(data->Strings, sigAllocatedLength);
    signature[0] = '\0';
  }
  else if (sigLength + n > sigAllocatedLength)
  {
    sigAllocatedLength += sigLength + n;
    ccp = signature;
    signature = vtkParse_NewString(data->Strings, sigAllocatedLength);
    strncpy(signature, ccp, sigLength);
    signature[sigLength] = '\0';
  }
}

/* close the signature, i.e. allow no more additions to it */
static void closeSig(void)
{
  sigClosed = 1;
}

/* re-open the signature */
static void openSig(void)
{
  sigClosed = 0;
}

/* append text to the end of the signature */
static void postSig(const char* arg)
{
  if (!sigClosed)
  {
    size_t n = strlen(arg);
    checkSigSize(n);
    if (n > 0)
    {
      strncpy(&signature[sigLength], arg, n + 1);
      sigLength += n;
    }
  }
}

/* set a mark in the signature for later operations */
static void markSig(void)
{
  sigMark[sigMarkDepth] = 0;
  if (signature)
  {
    sigMark[sigMarkDepth] = sigLength;
  }
  sigMarkDepth++;
}

/* get the contents of the sig from the mark, and clear the mark */
static const char* copySig(void)
{
  const char* cp = NULL;
  if (sigMarkDepth > 0)
  {
    sigMarkDepth--;
  }
  if (signature)
  {
    cp = &signature[sigMark[sigMarkDepth]];
  }
  return vtkstrdup(cp);
}

/* cut the sig from the mark to the current location, and clear the mark */
static const char* cutSig(void)
{
  const char* cp = NULL;
  if (sigMarkDepth > 0)
  {
    sigMarkDepth--;
  }
  if (signature)
  {
    sigLength = sigMark[sigMarkDepth];
    cp = vtkstrdup(&signature[sigLength]);
    signature[sigLength] = 0;
  }
  return cp;
}

/* chop the last space from the signature */
static void chopSig(void)
{
  if (signature)
  {
    size_t n = sigLength;
    if (n > 0 && signature[n - 1] == ' ')
    {
      signature[n - 1] = '\0';
      sigLength--;
    }
  }
}

/* chop the last space from the signature unless the preceding token
   is an operator (used to remove spaces before argument lists) */
static void postSigLeftBracket(const char* s)
{
  if (signature)
  {
    size_t n = sigLength;
    if (n > 1 && signature[n - 1] == ' ')
    {
      const char* ops = "%*/-+!~&|^<>=.,:;{}";
      char c = signature[n - 2];
      const char* cp;
      for (cp = ops; *cp != '\0'; cp++)
      {
        if (*cp == c)
        {
          break;
        }
      }
      if (*cp == '\0')
      {
        signature[n - 1] = '\0';
        sigLength--;
      }
    }
  }
  postSig(s);
}

/* chop trailing space and add a right bracket */
static void postSigRightBracket(const char* s)
{
  chopSig();
  postSig(s);
}

/*----------------------------------------------------------------
 * Subroutines for building a type
 */

/* "private" variables */
unsigned int storedType;
unsigned int typeStack[10];
unsigned int declAttributes;
unsigned int attributeStack[10];
int typeDepth = 0;

/* save the type on the stack */
static void pushType(void)
{
  attributeStack[typeDepth] = declAttributes;
  typeStack[typeDepth++] = storedType;
}

/* pop the type stack */
static void popType(void)
{
  storedType = typeStack[--typeDepth];
  declAttributes = attributeStack[typeDepth];
}

/* clear the storage type */
static void clearType(void)
{
  storedType = 0;
  declAttributes = 0;
}

/* save the type */
static void setTypeBase(unsigned int base)
{
  storedType &= ~(unsigned int)(VTK_PARSE_BASE_TYPE);
  storedType |= base;
}

/* set a type modifier bit */
static void setTypeMod(unsigned int mod)
{
  storedType |= mod;
}

/* modify the indirection (pointers, refs) in the storage type */
static void setTypePtr(unsigned int ind)
{
  storedType &= ~(unsigned int)(VTK_PARSE_INDIRECT | VTK_PARSE_RVALUE);
  ind &= (VTK_PARSE_INDIRECT | VTK_PARSE_RVALUE);
  storedType |= ind;
}

/* retrieve the storage type */
static unsigned int getType(void)
{
  return storedType;
}

/* combine two primitive type parts, e.g. "long int" */
static unsigned int buildTypeBase(unsigned int a, unsigned int b)
{
  unsigned int base = (a & VTK_PARSE_BASE_TYPE);
  unsigned int basemod = (b & VTK_PARSE_BASE_TYPE);

  switch (base)
  {
    case 0:
      base = basemod;
      break;
    case VTK_PARSE_UNSIGNED_INT:
      base = (basemod | VTK_PARSE_UNSIGNED);
      break;
    case VTK_PARSE_INT:
      base = basemod;
      if (base == VTK_PARSE_CHAR)
      {
        base = VTK_PARSE_SIGNED_CHAR;
      }
      break;
    case VTK_PARSE_CHAR:
      if (basemod == VTK_PARSE_INT)
      {
        base = VTK_PARSE_SIGNED_CHAR;
      }
      else if (basemod == VTK_PARSE_UNSIGNED_INT)
      {
        base = VTK_PARSE_UNSIGNED_CHAR;
      }
      break;
    case VTK_PARSE_SHORT:
      if (basemod == VTK_PARSE_UNSIGNED_INT)
      {
        base = VTK_PARSE_UNSIGNED_SHORT;
      }
      break;
    case VTK_PARSE_LONG:
      if (basemod == VTK_PARSE_UNSIGNED_INT)
      {
        base = VTK_PARSE_UNSIGNED_LONG;
      }
      else if (basemod == VTK_PARSE_LONG)
      {
        base = VTK_PARSE_LONG_LONG;
      }
      else if (basemod == VTK_PARSE_DOUBLE)
      {
        base = VTK_PARSE_LONG_DOUBLE;
      }
      break;
    case VTK_PARSE_UNSIGNED_LONG:
      if (basemod == VTK_PARSE_LONG)
      {
        base = VTK_PARSE_UNSIGNED_LONG_LONG;
      }
      break;
    case VTK_PARSE_LONG_LONG:
      if (basemod == VTK_PARSE_UNSIGNED_INT)
      {
        base = VTK_PARSE_UNSIGNED_LONG_LONG;
      }
      break;
    case VTK_PARSE___INT64:
      if (basemod == VTK_PARSE_UNSIGNED_INT)
      {
        base = VTK_PARSE_UNSIGNED___INT64;
      }
      break;
    case VTK_PARSE_DOUBLE:
      if (basemod == VTK_PARSE_LONG)
      {
        base = VTK_PARSE_LONG_DOUBLE;
      }
      break;
  }

  return ((a & ~(unsigned int)(VTK_PARSE_BASE_TYPE)) | base);
}

/* add an attribute specifier to the current declaration */
static void addAttribute(unsigned int flags)
{
  declAttributes |= flags;
}

/* check if an attribute is set for the current declaration */
static int getAttributes(void)
{
  return declAttributes;
}

/*----------------------------------------------------------------
 * Array information
 */

/* "private" variables */
int numberOfDimensions = 0;
const char** arrayDimensions = NULL;

/* clear the array counter */
static void clearArray(void)
{
  numberOfDimensions = 0;
  arrayDimensions = NULL;
}

/* add another dimension */
static void pushArraySize(const char* size)
{
  vtkParse_AddStringToArray(&arrayDimensions, &numberOfDimensions, size);
}

/* add another dimension to the front */
static void pushArrayFront(const char* size)
{
  int i;

  vtkParse_AddStringToArray(&arrayDimensions, &numberOfDimensions, 0);

  for (i = numberOfDimensions - 1; i > 0; i--)
  {
    arrayDimensions[i] = arrayDimensions[i - 1];
  }

  arrayDimensions[0] = size;
}

/* get the number of dimensions */
static int getArrayNDims(void)
{
  return numberOfDimensions;
}

/* get the whole array */
static const char** getArray(void)
{
  if (numberOfDimensions > 0)
  {
    return arrayDimensions;
  }
  return NULL;
}

/*----------------------------------------------------------------
 * Variables and Parameters
 */

/* "private" variables */
const char* currentVarName = 0;
const char* currentVarValue = 0;
const char* currentId = 0;

/* clear the var Id */
static void clearVarName(void)
{
  currentVarName = NULL;
}

/* set the var Id */
static void setVarName(const char* text)
{
  currentVarName = text;
}

/* return the var id */
static const char* getVarName(void)
{
  return currentVarName;
}

/* variable value -------------- */

/* clear the var value */
static void clearVarValue(void)
{
  currentVarValue = NULL;
}

/* set the var value */
static void setVarValue(const char* text)
{
  currentVarValue = text;
}

/* return the var value */
static const char* getVarValue(void)
{
  return currentVarValue;
}

/* variable type -------------- */

/* clear the current Id */
static void clearTypeId(void)
{
  currentId = NULL;
}

/* set the current Id, it is sticky until cleared */
static void setTypeId(const char* text)
{
  if (currentId == NULL)
  {
    currentId = text;
  }
}

/* set the signature and type together */
static void typeSig(const char* text)
{
  postSig(text);
  postSig(" ");

  if (currentId == 0)
  {
    setTypeId(text);
  }
}

/* return the current Id */
static const char* getTypeId(void)
{
  return currentId;
}

/*----------------------------------------------------------------
 * Specifically for function pointers, the scope (i.e. class) that
 * the function is a method of.
 */

const char* pointerScopeStack[10];
int pointerScopeDepth = 0;

/* save the scope for scoped method pointers */
static void scopeSig(const char* scope)
{
  if (scope && scope[0] != '\0')
  {
    postSig(scope);
  }
  else
  {
    scope = NULL;
  }
  pointerScopeStack[pointerScopeDepth++] = vtkstrdup(scope);
}

/* get the scope back */
static const char* getScope(void)
{
  return pointerScopeStack[--pointerScopeDepth];
}

/*----------------------------------------------------------------
 * Function stack
 *
 * operates on: currentFunction
 */

/* "private" variables */
FunctionInfo* functionStack[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
const char* functionVarNameStack[10];
const char* functionTypeIdStack[10];
int functionDepth = 0;

static void pushFunction(void)
{
  functionStack[functionDepth] = currentFunction;
  currentFunction = (FunctionInfo*)malloc(sizeof(FunctionInfo));
  vtkParse_InitFunction(currentFunction);
  if (!functionStack[functionDepth])
  {
    startSig();
  }
  functionVarNameStack[functionDepth] = getVarName();
  functionTypeIdStack[functionDepth] = getTypeId();
  pushType();
  clearType();
  clearVarName();
  clearTypeId();
  functionDepth++;
  functionStack[functionDepth] = 0;
}

static void popFunction(void)
{
  FunctionInfo* newFunction = currentFunction;

  --functionDepth;
  currentFunction = functionStack[functionDepth];
  clearVarName();
  if (functionVarNameStack[functionDepth])
  {
    setVarName(functionVarNameStack[functionDepth]);
  }
  clearTypeId();
  if (functionTypeIdStack[functionDepth])
  {
    setTypeId(functionTypeIdStack[functionDepth]);
  }
  popType();

  functionStack[functionDepth + 1] = newFunction;
}

static FunctionInfo* getFunction(void)
{
  return functionStack[functionDepth + 1];
}

/*----------------------------------------------------------------
 * Attributes
 */

int attributeRole = 0;
const char* attributePrefix = NULL;

/* Set kind of attributes to collect in attribute_specifier_seq */
static void setAttributeRole(int x)
{
  attributeRole = x;
}

/* Get the current kind of attribute */
static int getAttributeRole(void)
{
  return attributeRole;
}

/* Ignore attributes until further notice */
static void clearAttributeRole(void)
{
  attributeRole = 0;
}

/* Set the "using" prefix for attributes */
static void setAttributePrefix(const char* x)
{
  attributePrefix = x;
}

/* Get the "using" prefix for attributes */
static const char* getAttributePrefix(void)
{
  return attributePrefix;
}

/*----------------------------------------------------------------
 * Utility methods
 */

/* expand a type by including pointers from another */
static unsigned int add_indirection(unsigned int type1, unsigned int type2)
{
  unsigned int ptr1 = (type1 & VTK_PARSE_POINTER_MASK);
  unsigned int ptr2 = (type2 & VTK_PARSE_POINTER_MASK);
  unsigned int reverse = 0;
  unsigned int result;

  /* one of type1 or type2 will only have VTK_PARSE_INDIRECT, but
   * we don't know which one. */
  result = ((type1 & ~VTK_PARSE_POINTER_MASK) | (type2 & ~VTK_PARSE_POINTER_MASK));

  /* if there are two ampersands, it is an rvalue reference */
  if ((type1 & type2 & VTK_PARSE_REF) != 0)
  {
    result |= VTK_PARSE_RVALUE;
  }

  while (ptr2)
  {
    reverse = ((reverse << 2) | (ptr2 & VTK_PARSE_POINTER_LOWMASK));
    ptr2 = ((ptr2 >> 2) & VTK_PARSE_POINTER_MASK);
  }

  while (reverse)
  {
    ptr1 = ((ptr1 << 2) | (reverse & VTK_PARSE_POINTER_LOWMASK));
    reverse = ((reverse >> 2) & VTK_PARSE_POINTER_MASK);

    /* make sure we don't exceed the VTK_PARSE_POINTER bitfield */
    if ((ptr1 & VTK_PARSE_POINTER_MASK) != ptr1)
    {
      ptr1 = VTK_PARSE_BAD_INDIRECT;
      break;
    }
  }

  return (ptr1 | result);
}

/* There is only one array, so add any parenthetical indirection to it */
static unsigned int add_indirection_to_array(unsigned int type)
{
  unsigned int ptrs = (type & VTK_PARSE_POINTER_MASK);
  unsigned int result = (type & ~VTK_PARSE_POINTER_MASK);
  unsigned int reverse = 0;

  if ((type & VTK_PARSE_INDIRECT) == VTK_PARSE_BAD_INDIRECT)
  {
    return (result | VTK_PARSE_BAD_INDIRECT);
  }

  while (ptrs)
  {
    reverse = ((reverse << 2) | (ptrs & VTK_PARSE_POINTER_LOWMASK));
    ptrs = ((ptrs >> 2) & VTK_PARSE_POINTER_MASK);
  }

  /* I know the reversal makes no difference, but it is still
   * nice to add the pointers in the correct order, just in
   * case const pointers are thrown into the mix. */
  while (reverse)
  {
    pushArrayFront("");
    reverse = ((reverse >> 2) & VTK_PARSE_POINTER_MASK);
  }

  return result;
}

#ifndef YY_NULLPTR
#if defined __cplusplus
#if 201103L <= __cplusplus
#define YY_NULLPTR nullptr
#else
#define YY_NULLPTR 0
#endif
#else
#define YY_NULLPTR ((void*)0)
#endif
#endif

/* Debug traces.  */
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
#define YYTOKENTYPE
enum yytokentype
{
  ID = 258,
  VTK_ID = 259,
  QT_ID = 260,
  StdString = 261,
  UnicodeString = 262,
  OSTREAM = 263,
  ISTREAM = 264,
  LP = 265,
  LA = 266,
  STRING_LITERAL = 267,
  INT_LITERAL = 268,
  HEX_LITERAL = 269,
  BIN_LITERAL = 270,
  OCT_LITERAL = 271,
  FLOAT_LITERAL = 272,
  CHAR_LITERAL = 273,
  ZERO = 274,
  NULLPTR = 275,
  SSIZE_T = 276,
  SIZE_T = 277,
  NULLPTR_T = 278,
  BEGIN_ATTRIB = 279,
  STRUCT = 280,
  CLASS = 281,
  UNION = 282,
  ENUM = 283,
  PUBLIC = 284,
  PRIVATE = 285,
  PROTECTED = 286,
  CONST = 287,
  VOLATILE = 288,
  MUTABLE = 289,
  STATIC = 290,
  THREAD_LOCAL = 291,
  VIRTUAL = 292,
  EXPLICIT = 293,
  INLINE = 294,
  CONSTEXPR = 295,
  FRIEND = 296,
  EXTERN = 297,
  OPERATOR = 298,
  TEMPLATE = 299,
  THROW = 300,
  TRY = 301,
  CATCH = 302,
  NOEXCEPT = 303,
  DECLTYPE = 304,
  TYPENAME = 305,
  TYPEDEF = 306,
  NAMESPACE = 307,
  USING = 308,
  NEW = 309,
  DELETE = 310,
  DEFAULT = 311,
  STATIC_CAST = 312,
  DYNAMIC_CAST = 313,
  CONST_CAST = 314,
  REINTERPRET_CAST = 315,
  OP_LSHIFT_EQ = 316,
  OP_RSHIFT_EQ = 317,
  OP_LSHIFT = 318,
  OP_RSHIFT_A = 319,
  OP_DOT_POINTER = 320,
  OP_ARROW_POINTER = 321,
  OP_ARROW = 322,
  OP_INCR = 323,
  OP_DECR = 324,
  OP_PLUS_EQ = 325,
  OP_MINUS_EQ = 326,
  OP_TIMES_EQ = 327,
  OP_DIVIDE_EQ = 328,
  OP_REMAINDER_EQ = 329,
  OP_AND_EQ = 330,
  OP_OR_EQ = 331,
  OP_XOR_EQ = 332,
  OP_LOGIC_AND = 333,
  OP_LOGIC_OR = 334,
  OP_LOGIC_EQ = 335,
  OP_LOGIC_NEQ = 336,
  OP_LOGIC_LEQ = 337,
  OP_LOGIC_GEQ = 338,
  ELLIPSIS = 339,
  DOUBLE_COLON = 340,
  OTHER = 341,
  AUTO = 342,
  VOID = 343,
  BOOL = 344,
  FLOAT = 345,
  DOUBLE = 346,
  INT = 347,
  SHORT = 348,
  LONG = 349,
  INT64__ = 350,
  CHAR = 351,
  CHAR16_T = 352,
  CHAR32_T = 353,
  WCHAR_T = 354,
  SIGNED = 355,
  UNSIGNED = 356
};
#endif

/* Value type.  */
#if !defined YYSTYPE && !defined YYSTYPE_IS_DECLARED

union YYSTYPE {

  const char* str;
  unsigned int integer;
};

typedef union YYSTYPE YYSTYPE;
#define YYSTYPE_IS_TRIVIAL 1
#define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

int yyparse(void);

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
#undef YYERROR_VERBOSE
#define YYERROR_VERBOSE 1
#else
#define YYERROR_VERBOSE 0
#endif

/* Default (constant) value used for initialization for null
   right-hand sides.  Unlike the standard yacc.c template, here we set
   the default value of $$ to a zeroed-out value.  Since the default
   value is undefined, this behavior is technically correct.  */
static YYSTYPE yyval_default;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef YY_
#if defined YYENABLE_NLS && YYENABLE_NLS
#if ENABLE_NLS
#include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#define YY_(Msgid) dgettext("bison-runtime", Msgid)
#endif
#endif
#ifndef YY_
#define YY_(Msgid) Msgid
#endif
#endif

#ifndef YYFREE
#define YYFREE free
#endif
#ifndef YYMALLOC
#define YYMALLOC malloc
#endif
#ifndef YYREALLOC
#define YYREALLOC realloc
#endif

#define YYSIZEMAX ((size_t)-1)

#ifdef __cplusplus
typedef bool yybool;
#define yytrue true
#define yyfalse false
#else
/* When we move to stdbool, get rid of the various casts to yybool.  */
typedef unsigned char yybool;
#define yytrue 1
#define yyfalse 0
#endif

#ifndef YYSETJMP
#include <setjmp.h>
#define YYJMP_BUF jmp_buf
#define YYSETJMP(Env) setjmp(Env)
/* Pacify Clang and ICC.  */
#define YYLONGJMP(Env, Val)                                                                        \
  do                                                                                               \
  {                                                                                                \
    longjmp(Env, Val);                                                                             \
    YYASSERT(0);                                                                                   \
  } while (yyfalse)
#endif

#ifndef YY_ATTRIBUTE
#if (defined __GNUC__ && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__))) ||             \
  defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#define YY_ATTRIBUTE(Spec) __attribute__(Spec)
#else
#define YY_ATTRIBUTE(Spec) /* empty */
#endif
#endif

#ifndef YY_ATTRIBUTE_PURE
#define YY_ATTRIBUTE_PURE YY_ATTRIBUTE((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
#define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE((__unused__))
#endif

/* The _Noreturn keyword of C11.  */
#if !defined _Noreturn
#if defined __cplusplus && 201103L <= __cplusplus
#define _Noreturn [[noreturn]]
#elif !(defined __STDC_VERSION__ && 201112 <= __STDC_VERSION__)
#if (3 <= __GNUC__ || (__GNUC__ == 2 && 8 <= __GNUC_MINOR__) || 0x5110 <= __SUNPRO_C)
#define _Noreturn __attribute__((__noreturn__))
#elif defined _MSC_VER && 1200 <= _MSC_VER
#define _Noreturn __declspec(noreturn)
#else
#define _Noreturn
#endif
#endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if 1
#define YYUSE(E) ((void)(E))
#else
#define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && !defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                                                        \
  _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wuninitialized\"")             \
    _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
#define YY_IGNORE_MAYBE_UNINITIALIZED_END _Pragma("GCC diagnostic pop")
#else
#define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
#define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
#define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#ifndef YYASSERT
#define YYASSERT(Condition) ((void)((Condition) || (abort(), 0)))
#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL 3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST 6566

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS 125
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS 277
/* YYNRULES -- Number of rules.  */
#define YYNRULES 676
/* YYNRULES -- Number of states.  */
#define YYNSTATES 1057
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 8
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK 2
#define YYMAXUTOK 356

#define YYTRANSLATE(YYX) ((unsigned)(YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] = { 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 121, 2, 2, 2, 117, 111, 2, 108, 109, 115, 120, 107,
  119, 124, 118, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 106, 102, 110, 105, 116, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 112, 2, 113, 123, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 103, 122, 104, 114, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9,
  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
  34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
  58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
  82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101 };

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] = { 0, 1798, 1798, 1800, 1802, 1801, 1812, 1813, 1814, 1815,
  1816, 1817, 1818, 1819, 1820, 1821, 1822, 1823, 1824, 1825, 1826, 1827, 1828, 1831, 1832, 1833,
  1834, 1835, 1836, 1839, 1840, 1847, 1854, 1855, 1855, 1857, 1860, 1867, 1868, 1871, 1872, 1873,
  1876, 1877, 1880, 1880, 1895, 1894, 1900, 1906, 1905, 1910, 1916, 1917, 1918, 1921, 1923, 1925,
  1928, 1929, 1932, 1933, 1935, 1937, 1936, 1945, 1949, 1950, 1951, 1954, 1955, 1956, 1957, 1958,
  1959, 1960, 1961, 1962, 1963, 1964, 1965, 1966, 1967, 1970, 1971, 1972, 1973, 1974, 1975, 1978,
  1979, 1980, 1981, 1985, 1986, 1989, 1991, 1994, 1999, 2000, 2003, 2004, 2007, 2008, 2009, 2020,
  2021, 2022, 2026, 2027, 2031, 2031, 2044, 2051, 2060, 2061, 2062, 2065, 2066, 2066, 2070, 2071,
  2073, 2074, 2075, 2075, 2083, 2087, 2088, 2091, 2093, 2095, 2096, 2099, 2100, 2108, 2109, 2112,
  2113, 2115, 2117, 2119, 2123, 2125, 2126, 2129, 2132, 2133, 2136, 2137, 2136, 2141, 2183, 2186,
  2187, 2188, 2190, 2192, 2194, 2198, 2201, 2201, 2234, 2233, 2237, 2245, 2236, 2255, 2257, 2256,
  2261, 2263, 2261, 2265, 2267, 2265, 2269, 2272, 2269, 2283, 2284, 2287, 2288, 2290, 2291, 2294,
  2294, 2304, 2305, 2313, 2314, 2315, 2316, 2319, 2322, 2323, 2324, 2327, 2328, 2329, 2332, 2333,
  2334, 2338, 2339, 2340, 2341, 2344, 2345, 2346, 2350, 2355, 2349, 2367, 2371, 2382, 2381, 2390,
  2394, 2397, 2407, 2411, 2412, 2415, 2416, 2418, 2419, 2420, 2423, 2424, 2426, 2427, 2428, 2430,
  2431, 2434, 2447, 2448, 2449, 2450, 2457, 2458, 2461, 2461, 2469, 2470, 2471, 2474, 2476, 2477,
  2481, 2480, 2497, 2521, 2493, 2532, 2532, 2535, 2536, 2539, 2540, 2543, 2544, 2550, 2551, 2551,
  2554, 2555, 2555, 2557, 2559, 2563, 2565, 2563, 2589, 2590, 2593, 2593, 2595, 2595, 2597, 2597,
  2602, 2603, 2603, 2611, 2614, 2684, 2685, 2687, 2688, 2688, 2691, 2694, 2695, 2699, 2711, 2710,
  2732, 2734, 2734, 2755, 2755, 2757, 2761, 2762, 2763, 2762, 2768, 2770, 2771, 2772, 2773, 2774,
  2775, 2778, 2779, 2783, 2784, 2788, 2789, 2792, 2793, 2797, 2798, 2799, 2800, 2803, 2804, 2807,
  2807, 2810, 2811, 2814, 2814, 2818, 2819, 2819, 2826, 2827, 2830, 2831, 2832, 2833, 2834, 2837,
  2839, 2841, 2845, 2847, 2849, 2851, 2853, 2855, 2857, 2857, 2862, 2865, 2868, 2871, 2871, 2879,
  2879, 2888, 2889, 2890, 2891, 2892, 2893, 2894, 2895, 2896, 2897, 2904, 2905, 2906, 2907, 2908,
  2909, 2910, 2916, 2917, 2920, 2921, 2923, 2924, 2927, 2928, 2931, 2932, 2933, 2934, 2937, 2938,
  2939, 2940, 2941, 2945, 2946, 2947, 2950, 2951, 2954, 2955, 2963, 2966, 2966, 2968, 2968, 2972,
  2973, 2975, 2979, 2980, 2982, 2982, 2985, 2987, 2991, 2994, 2994, 2996, 2996, 3000, 3003, 3003,
  3005, 3005, 3009, 3010, 3012, 3014, 3016, 3018, 3020, 3024, 3025, 3028, 3029, 3030, 3031, 3032,
  3033, 3034, 3035, 3036, 3037, 3040, 3041, 3042, 3043, 3044, 3045, 3046, 3047, 3048, 3049, 3050,
  3051, 3052, 3053, 3054, 3074, 3075, 3076, 3077, 3080, 3084, 3088, 3088, 3092, 3093, 3108, 3109,
  3134, 3134, 3138, 3138, 3142, 3142, 3146, 3146, 3150, 3150, 3154, 3154, 3157, 3158, 3161, 3165,
  3166, 3169, 3172, 3173, 3174, 3175, 3178, 3178, 3182, 3183, 3186, 3187, 3190, 3191, 3198, 3199,
  3200, 3201, 3202, 3203, 3204, 3205, 3206, 3207, 3208, 3209, 3212, 3213, 3214, 3215, 3216, 3217,
  3218, 3219, 3220, 3221, 3222, 3223, 3224, 3225, 3226, 3227, 3228, 3229, 3230, 3231, 3232, 3233,
  3234, 3235, 3236, 3237, 3238, 3239, 3240, 3241, 3242, 3243, 3244, 3245, 3248, 3249, 3250, 3251,
  3252, 3253, 3254, 3255, 3256, 3257, 3258, 3259, 3260, 3261, 3262, 3263, 3264, 3265, 3266, 3267,
  3268, 3269, 3270, 3271, 3272, 3273, 3274, 3275, 3276, 3277, 3280, 3281, 3282, 3283, 3284, 3285,
  3286, 3287, 3288, 3295, 3296, 3299, 3300, 3301, 3302, 3302, 3303, 3306, 3307, 3310, 3311, 3312,
  3313, 3349, 3349, 3350, 3351, 3352, 3353, 3355, 3356, 3359, 3360, 3361, 3362, 3365, 3366, 3367,
  3370, 3371, 3373, 3374, 3376, 3377, 3380, 3381, 3384, 3385, 3386, 3390, 3389, 3403, 3404, 3407,
  3407, 3409, 3409, 3413, 3413, 3415, 3415, 3417, 3417, 3421, 3421, 3426, 3427, 3429, 3430, 3433,
  3434, 3437, 3438, 3441, 3442, 3443, 3444, 3445, 3446, 3447, 3448, 3448, 3448, 3448, 3448, 3449,
  3450, 3451, 3452, 3453, 3456, 3459, 3460, 3463, 3466, 3466, 3466 };
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char* const yytname[] = { "$end", "error", "$undefined", "ID", "VTK_ID", "QT_ID",
  "StdString", "UnicodeString", "OSTREAM", "ISTREAM", "LP", "LA", "STRING_LITERAL", "INT_LITERAL",
  "HEX_LITERAL", "BIN_LITERAL", "OCT_LITERAL", "FLOAT_LITERAL", "CHAR_LITERAL", "ZERO", "NULLPTR",
  "SSIZE_T", "SIZE_T", "NULLPTR_T", "BEGIN_ATTRIB", "STRUCT", "CLASS", "UNION", "ENUM", "PUBLIC",
  "PRIVATE", "PROTECTED", "CONST", "VOLATILE", "MUTABLE", "STATIC", "THREAD_LOCAL", "VIRTUAL",
  "EXPLICIT", "INLINE", "CONSTEXPR", "FRIEND", "EXTERN", "OPERATOR", "TEMPLATE", "THROW", "TRY",
  "CATCH", "NOEXCEPT", "DECLTYPE", "TYPENAME", "TYPEDEF", "NAMESPACE", "USING", "NEW", "DELETE",
  "DEFAULT", "STATIC_CAST", "DYNAMIC_CAST", "CONST_CAST", "REINTERPRET_CAST", "OP_LSHIFT_EQ",
  "OP_RSHIFT_EQ", "OP_LSHIFT", "OP_RSHIFT_A", "OP_DOT_POINTER", "OP_ARROW_POINTER", "OP_ARROW",
  "OP_INCR", "OP_DECR", "OP_PLUS_EQ", "OP_MINUS_EQ", "OP_TIMES_EQ", "OP_DIVIDE_EQ",
  "OP_REMAINDER_EQ", "OP_AND_EQ", "OP_OR_EQ", "OP_XOR_EQ", "OP_LOGIC_AND", "OP_LOGIC_OR",
  "OP_LOGIC_EQ", "OP_LOGIC_NEQ", "OP_LOGIC_LEQ", "OP_LOGIC_GEQ", "ELLIPSIS", "DOUBLE_COLON",
  "OTHER", "AUTO", "VOID", "BOOL", "FLOAT", "DOUBLE", "INT", "SHORT", "LONG", "INT64__", "CHAR",
  "CHAR16_T", "CHAR32_T", "WCHAR_T", "SIGNED", "UNSIGNED", "';'", "'{'", "'}'", "'='", "':'", "','",
  "'('", "')'", "'<'", "'&'", "'['", "']'", "'~'", "'*'", "'>'", "'%'", "'/'", "'-'", "'+'", "'!'",
  "'|'", "'^'", "'.'", "$accept", "translation_unit", "opt_declaration_seq", "$@1", "declaration",
  "template_declaration", "explicit_instantiation", "linkage_specification", "namespace_definition",
  "$@2", "namespace_alias_definition", "forward_declaration", "simple_forward_declaration",
  "class_definition", "class_specifier", "$@3", "class_head", "$@4", "$@5", "class_key",
  "class_head_name", "class_name", "opt_final", "member_specification", "$@6",
  "member_access_specifier", "member_declaration", "template_member_declaration",
  "friend_declaration", "base_specifier_list", "base_specifier", "opt_virtual",
  "opt_access_specifier", "access_specifier", "opaque_enum_declaration", "enum_definition",
  "enum_specifier", "$@7", "enum_head", "enum_key", "opt_enum_base", "$@8", "enumerator_list",
  "enumerator_definition", "$@9", "nested_variable_initialization", "ignored_initializer",
  "ignored_class", "ignored_class_body", "typedef_declaration", "basic_typedef_declaration",
  "typedef_declarator_list", "typedef_declarator_list_cont", "typedef_declarator",
  "typedef_direct_declarator", "function_direct_declarator", "$@10", "$@11",
  "typedef_declarator_id", "using_declaration", "using_id", "using_directive", "alias_declaration",
  "$@12", "template_head", "$@13", "$@14", "$@15", "template_parameter_list", "$@16",
  "template_parameter", "$@17", "$@18", "$@19", "$@20", "$@21", "$@22", "opt_ellipsis",
  "class_or_typename", "opt_template_parameter_initializer", "template_parameter_initializer",
  "$@23", "template_parameter_value", "function_definition", "function_declaration",
  "nested_method_declaration", "nested_operator_declaration", "method_definition",
  "method_declaration", "operator_declaration", "conversion_function", "$@24", "$@25",
  "conversion_function_id", "operator_function_nr", "operator_function_sig", "$@26",
  "operator_function_id", "operator_sig", "function_nr", "function_trailer_clause",
  "func_cv_qualifier_seq", "func_cv_qualifier", "opt_noexcept_specifier", "noexcept_sig",
  "opt_ref_qualifier", "virt_specifier_seq", "virt_specifier", "opt_body_as_trailer",
  "opt_trailing_return_type", "trailing_return_type", "$@27", "function_body", "function_try_block",
  "handler_seq", "function_sig", "$@28", "structor_declaration", "$@29", "$@30", "structor_sig",
  "$@31", "opt_ctor_initializer", "mem_initializer_list", "mem_initializer",
  "parameter_declaration_clause", "$@32", "parameter_list", "$@33", "parameter_declaration", "$@34",
  "$@35", "opt_initializer", "initializer", "$@36", "$@37", "$@38", "constructor_args", "$@39",
  "variable_declaration", "init_declarator_id", "opt_declarator_list", "declarator_list_cont",
  "$@40", "init_declarator", "opt_ptr_operator_seq", "direct_abstract_declarator", "$@41",
  "direct_declarator", "$@42", "lp_or_la", "$@43", "opt_array_or_parameters", "$@44", "$@45",
  "function_qualifiers", "abstract_declarator", "declarator", "opt_declarator_id", "declarator_id",
  "bitfield_size", "opt_array_decorator_seq", "array_decorator_seq", "$@46",
  "array_decorator_seq_impl", "array_decorator", "$@47", "array_size_specifier", "$@48",
  "id_expression", "unqualified_id", "qualified_id", "nested_name_specifier", "$@49", "tilde_sig",
  "identifier_sig", "scope_operator_sig", "template_id", "$@50", "decltype_specifier", "$@51",
  "simple_id", "identifier", "opt_decl_specifier_seq", "decl_specifier2", "decl_specifier_seq",
  "decl_specifier", "storage_class_specifier", "function_specifier", "cv_qualifier",
  "cv_qualifier_seq", "store_type", "store_type_specifier", "$@52", "$@53", "type_specifier",
  "trailing_type_specifier", "$@54", "trailing_type_specifier_seq", "trailing_type_specifier_seq2",
  "$@55", "$@56", "tparam_type", "tparam_type_specifier2", "$@57", "$@58", "tparam_type_specifier",
  "simple_type_specifier", "type_name", "primitive_type", "ptr_operator_seq", "reference",
  "rvalue_reference", "pointer", "$@59", "ptr_cv_qualifier_seq", "pointer_seq",
  "decl_attribute_specifier_seq", "$@60", "id_attribute_specifier_seq", "$@61",
  "ref_attribute_specifier_seq", "$@62", "func_attribute_specifier_seq", "$@63",
  "array_attribute_specifier_seq", "$@64", "class_attribute_specifier_seq", "$@65",
  "attribute_specifier_seq", "attribute_specifier", "attribute_specifier_contents",
  "attribute_using_prefix", "attribute_list", "attribute", "$@66", "attribute_pack",
  "attribute_sig", "attribute_token", "operator_id", "operator_id_no_delim", "keyword", "literal",
  "constant_expression", "constant_expression_item", "$@67", "common_bracket_item",
  "common_bracket_item_no_scope_operator", "any_bracket_contents", "bracket_pitem",
  "any_bracket_item", "braces_item", "angle_bracket_contents", "braces_contents",
  "angle_bracket_pitem", "angle_bracket_item", "angle_brackets_sig", "$@68", "right_angle_bracket",
  "brackets_sig", "$@69", "$@70", "parentheses_sig", "$@71", "$@72", "$@73", "braces_sig", "$@74",
  "ignored_items", "ignored_expression", "ignored_item", "ignored_item_no_semi",
  "ignored_item_no_angle", "ignored_braces", "ignored_brackets", "ignored_parentheses",
  "ignored_left_parenthesis", YY_NULLPTR };
#endif

#define YYPACT_NINF -854
#define YYTABLE_NINF -630

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const short yypact[] = { -854, 59, 93, -854, -854, 1388, -854, 239, 270, 455, 475, 484, 514,
  540, -19, 2, 182, -854, -854, -854, 372, -854, -854, -854, -854, -854, -854, -854, 69, -854, 72,
  -854, 3534, -854, -854, 6145, 468, 153, -854, -854, -854, -854, -854, -854, -854, -854, -854,
  -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854,
  -854, -854, -854, -854, -854, -854, -854, -854, 34, -854, -854, -854, -854, -854, -854, 5848,
  -854, 196, 196, 196, 196, -854, 40, 6145, -854, 143, -854, 172, 1335, 1223, 122, 5423, 357, 396,
  -854, 145, 5947, -854, -854, -854, -854, 247, 136, -854, -854, -854, -854, -854, 245, -854, -854,
  781, 180, 3900, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854,
  -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854,
  -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854,
  -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854,
  -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854,
  -854, -854, -854, -854, -854, -854, -854, -854, 35, -854, -854, -854, -854, -854, -854, -854,
  -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854,
  162, 5423, 109, 167, 177, 179, 227, 265, 269, -854, 283, -854, -854, -854, -854, -854, 1604, 122,
  122, 6145, 247, -854, -854, -854, -854, -854, -854, -854, -854, 208, 109, 167, 177, 179, 227, 265,
  269, -854, -854, -854, 5423, 5423, 240, 282, -854, 1335, 5423, 122, 122, 6367, 302, 5737, -854,
  6367, -854, 1722, 309, 5423, -854, -854, -854, -854, -854, -854, 5848, -854, -854, 6046, 247, 325,
  -854, -854, -854, -854, -854, -854, -854, -854, -854, 6145, -854, -854, -854, -854, -854, -854,
  176, 331, 122, 122, 122, -854, -854, -854, -854, 145, -854, -854, -854, -854, -854, -854, -854,
  -854, -854, -854, -854, -854, -854, 1335, -854, -854, -854, -854, -854, -854, 5683, -854, 502,
  181, -854, -854, -854, -854, -854, -854, -854, -854, 381, -854, -854, -854, 147, -854, 342, -854,
  -854, 1948, 2070, -854, -854, 343, -854, 2192, 3046, 2314, -854, -854, -854, -854, -854, -854,
  6451, 1162, 6451, 771, -854, -854, -854, -854, -854, -854, 5690, -854, 2436, 710, 411, -854, 417,
  -854, 444, -854, -854, -854, 5242, 1335, -854, -854, 459, -854, 247, -854, -854, -854, -854, -854,
  -854, 61, -854, 5371, 504, 122, 122, 245, 465, 1522, -854, -854, -854, 526, -854, 5423, 6046,
  5683, 5423, 467, 2558, 464, 760, 781, -854, -854, -854, 162, -854, -854, -854, -854, -854, 6367,
  1162, 6367, 771, -854, -854, -854, -854, 500, -854, 546, -854, 5562, -854, 546, 476, -854, 1335,
  16, -854, -854, -854, 473, 479, 5690, -854, 487, 247, -854, -854, -854, -854, -854, -854, 5826,
  1834, 480, 226, 490, -854, 781, -854, 505, 3168, -854, -854, 491, -854, -854, -854, -854, 26,
  -854, 6244, 105, 564, -854, -854, -854, -854, -854, -854, -854, -854, -854, 512, -854, 247, 82,
  513, 200, 6451, 6451, 169, 251, -854, -854, -854, -854, 515, 122, -854, -854, -854, 245, 627, 519,
  522, 88, -854, -854, 528, -854, 525, -854, -854, -854, -854, -854, -854, 534, -854, -854, 322,
  1782, -854, -854, 538, -854, -854, 122, 122, 5371, -854, -854, -854, -854, -854, -854, -854, -854,
  214, -854, -854, 6145, 543, -854, -854, 1335, 542, -854, 133, -854, -854, 548, 563, -854, 122,
  -854, -854, -854, 464, 4632, 549, 63, 550, 526, 5826, -854, 500, -854, -854, -854, -854, -6, -854,
  547, -854, -854, -854, 559, 350, -854, -854, -854, -854, -854, 4876, -854, -854, 1543, -854, -854,
  245, 500, 566, -854, 562, 490, 263, 122, -854, 592, 162, 575, -854, -854, -854, -854, -854, 5423,
  5423, 5423, -854, 122, 122, 6145, 247, 136, -854, -854, -854, -854, 247, 105, -854, 4022, 4144,
  4266, -854, 573, -854, -854, -854, 581, 582, -854, 136, -854, 580, -854, 583, 6145, -854, 588,
  589, -854, -854, -854, -854, -854, -854, -854, -854, -854, 590, -854, -854, -854, 630, 596, -854,
  651, 622, -854, -854, -854, -854, 1522, 602, -854, -854, 398, 5423, 622, 622, 2680, -854, -854,
  601, -854, -854, -854, 706, 245, 603, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854,
  -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, -854, 620,
  -854, -854, -854, 176, -854, -854, 559, -854, 652, -854, 615, 136, -854, 4754, -854, 4876, -854,
  -854, -854, -854, 434, -854, 416, -854, -854, -854, -854, 781, -854, -854, -854, -854, 343, -854,
  -854, -854, -854, -854, 5690, -854, -854, -854, -854, -854, 247, -854, -854, -854, -854, -854,
  -854, -854, -854, -854, -854, -854, -854, -854, -854, 464, -854, 247, -854, -854, 1545, -854,
  5423, -854, -854, -854, 5423, -854, 1782, -854, -854, -854, -854, 623, -854, -854, -854, -854,
  -854, 546, 640, 6145, -854, -854, 325, -854, -854, -854, -854, -854, -854, 464, 613, -854, -854,
  -854, -854, -854, -854, 464, -854, 5120, -854, 3656, -854, -854, -854, -854, -854, -854, -854,
  -854, -854, 426, -854, 624, 181, 5826, 624, -854, 621, 632, -854, 102, 1834, -854, -854, -854,
  -854, -854, -854, -854, -854, -854, -854, -854, -854, 1666, -854, 196, -854, -854, -854, 633, 331,
  1335, 5363, 247, 622, 1782, 622, 596, 4876, 3778, -854, 684, -854, -854, -854, 247, -854, 4388,
  4632, 4510, 665, 631, 635, 4876, 634, -854, -854, -854, -854, -854, 4876, 464, 5826, -854, -854,
  -854, -854, -854, 636, 247, -854, 624, -854, -854, 5462, -854, -854, -854, -854, 1666, -854, -854,
  331, 5561, -854, -854, -854, -854, 1335, 5683, -854, -854, -854, 4876, 186, -854, -854, 637, 638,
  -854, -854, -854, -854, -854, -854, -854, 4876, -854, 4876, 644, 4998, -854, -854, -854, -854,
  -854, -854, -854, 1896, 196, 5561, 622, 5561, 642, -854, -854, 643, 502, 46, -854, -854, 6343, 85,
  -854, -854, -854, 4998, -854, 442, 421, 5713, -854, -854, 1896, -854, -854, 5683, -854, 647, -854,
  -854, -854, -854, -854, 6343, -854, -854, 136, -854, 245, -854, -854, -854, -854, -854, 186, 217,
  -854, -854, 106, -854, 5713, -854, 5744, -854, 2802, -854, -854, -854, 421, -854, -854, 2924,
  3290, 229, 75, 5744, 123, -854, -854, -854, 5826, -854, -854, -854, -854, 77, 229, 5826, 3412,
  -854, -854 };

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] = { 3, 0, 4, 1, 474, 0, 486, 441, 442, 443, 437, 438, 439,
  440, 445, 446, 444, 53, 52, 54, 114, 400, 401, 392, 395, 396, 398, 399, 397, 391, 393, 218, 0,
  361, 414, 0, 0, 0, 358, 447, 448, 449, 450, 451, 456, 457, 458, 459, 452, 453, 454, 455, 460, 461,
  22, 356, 5, 19, 20, 13, 11, 12, 9, 37, 17, 380, 44, 484, 10, 16, 380, 0, 484, 14, 135, 7, 6, 8, 0,
  18, 0, 0, 0, 0, 207, 0, 0, 15, 0, 338, 474, 0, 0, 0, 0, 474, 413, 340, 357, 0, 474, 388, 389, 390,
  179, 293, 405, 409, 412, 474, 474, 475, 116, 115, 0, 394, 0, 441, 442, 443, 437, 438, 439, 440,
  675, 676, 586, 581, 582, 583, 580, 584, 585, 587, 588, 445, 446, 444, 645, 553, 552, 554, 573,
  556, 558, 557, 559, 560, 561, 562, 565, 566, 564, 563, 569, 572, 555, 574, 575, 567, 551, 550,
  571, 570, 526, 527, 568, 578, 577, 576, 579, 528, 529, 530, 659, 531, 532, 533, 539, 540, 534,
  535, 536, 537, 538, 541, 542, 543, 544, 545, 546, 547, 548, 549, 657, 656, 669, 645, 663, 660,
  664, 674, 164, 523, 645, 522, 517, 662, 516, 518, 519, 520, 521, 524, 525, 661, 668, 667, 658,
  665, 666, 647, 653, 655, 654, 645, 0, 0, 441, 442, 443, 437, 438, 439, 440, 397, 393, 380, 484,
  380, 484, 474, 0, 474, 413, 0, 179, 373, 375, 374, 378, 379, 377, 376, 645, 33, 365, 363, 364,
  368, 369, 367, 366, 372, 371, 370, 0, 0, 0, 476, 339, 0, 0, 341, 342, 293, 0, 51, 486, 293, 110,
  117, 0, 0, 26, 38, 23, 484, 25, 27, 0, 24, 28, 0, 179, 257, 246, 645, 189, 245, 191, 192, 190,
  210, 484, 0, 213, 21, 417, 354, 197, 195, 225, 345, 0, 341, 342, 343, 59, 344, 58, 0, 348, 346,
  347, 349, 416, 350, 359, 380, 484, 380, 484, 136, 208, 0, 474, 407, 386, 301, 303, 180, 0, 289,
  274, 179, 478, 478, 478, 404, 294, 462, 463, 472, 464, 380, 436, 435, 496, 487, 0, 3, 647, 0, 0,
  632, 631, 170, 162, 0, 0, 0, 639, 641, 637, 362, 474, 394, 293, 51, 293, 117, 345, 380, 380, 151,
  147, 143, 0, 146, 0, 0, 0, 154, 0, 152, 0, 486, 156, 155, 0, 0, 385, 384, 0, 289, 179, 474, 382,
  383, 62, 40, 49, 410, 474, 0, 0, 59, 0, 485, 0, 122, 106, 118, 113, 474, 476, 0, 0, 0, 0, 0, 0,
  264, 0, 0, 229, 228, 480, 227, 255, 351, 352, 353, 620, 293, 51, 293, 117, 198, 196, 387, 380,
  470, 209, 221, 476, 0, 193, 221, 327, 476, 0, 0, 276, 286, 275, 0, 0, 0, 317, 0, 179, 467, 486,
  466, 468, 465, 473, 406, 0, 0, 496, 490, 493, 0, 3, 4, 0, 650, 652, 0, 646, 649, 651, 670, 0, 167,
  0, 0, 0, 474, 671, 30, 648, 673, 609, 609, 609, 415, 0, 143, 179, 410, 0, 474, 293, 293, 0, 327,
  476, 341, 342, 32, 0, 0, 3, 159, 160, 477, 0, 526, 527, 0, 511, 510, 0, 508, 0, 509, 217, 515,
  158, 157, 42, 288, 292, 381, 63, 0, 61, 39, 48, 57, 474, 59, 0, 0, 108, 365, 363, 364, 368, 369,
  367, 366, 0, 120, 476, 0, 112, 411, 474, 0, 258, 259, 0, 645, 244, 0, 474, 410, 0, 233, 486, 226,
  264, 0, 0, 410, 0, 474, 408, 402, 471, 302, 223, 224, 214, 230, 222, 0, 219, 298, 328, 0, 321,
  199, 194, 476, 285, 290, 0, 643, 279, 0, 299, 318, 479, 470, 0, 153, 0, 489, 496, 502, 357, 498,
  500, 4, 31, 29, 672, 168, 165, 0, 0, 0, 431, 430, 429, 0, 179, 293, 424, 428, 181, 182, 179, 0,
  163, 0, 0, 0, 138, 142, 145, 140, 112, 0, 0, 137, 293, 148, 321, 36, 4, 0, 514, 0, 0, 513, 512,
  504, 505, 66, 67, 68, 45, 474, 0, 102, 103, 104, 100, 50, 93, 98, 179, 46, 55, 474, 111, 122, 123,
  119, 105, 340, 0, 179, 179, 0, 211, 270, 265, 266, 271, 355, 252, 481, 0, 635, 598, 627, 603, 628,
  629, 633, 604, 608, 607, 602, 605, 606, 625, 597, 626, 621, 624, 360, 599, 600, 601, 43, 41, 109,
  112, 403, 232, 231, 225, 215, 333, 330, 331, 0, 250, 0, 293, 596, 593, 594, 277, 589, 591, 592,
  622, 0, 282, 304, 469, 491, 488, 495, 0, 499, 497, 501, 35, 170, 474, 432, 433, 434, 426, 319,
  171, 478, 423, 380, 174, 179, 614, 616, 617, 640, 612, 613, 611, 615, 610, 642, 638, 139, 141,
  144, 264, 34, 179, 506, 507, 0, 65, 0, 101, 474, 99, 0, 95, 0, 56, 121, 124, 647, 0, 128, 260,
  262, 261, 248, 221, 267, 0, 235, 234, 257, 256, 609, 620, 609, 107, 480, 264, 336, 332, 324, 325,
  326, 323, 322, 264, 291, 0, 590, 0, 283, 281, 305, 300, 308, 503, 169, 166, 380, 304, 320, 183,
  179, 425, 183, 177, 0, 0, 474, 393, 0, 82, 80, 71, 77, 64, 79, 73, 72, 76, 74, 69, 70, 0, 78, 0,
  204, 205, 75, 0, 338, 0, 0, 179, 179, 0, 179, 47, 0, 127, 126, 247, 212, 269, 474, 179, 253, 0, 0,
  0, 240, 0, 0, 0, 0, 595, 619, 644, 618, 623, 0, 264, 427, 295, 185, 172, 184, 315, 0, 179, 175,
  183, 149, 161, 0, 83, 85, 88, 86, 0, 84, 87, 0, 0, 200, 81, 476, 206, 0, 0, 96, 94, 97, 125, 0,
  268, 272, 236, 0, 630, 634, 242, 233, 241, 216, 482, 337, 251, 284, 0, 0, 296, 316, 178, 309, 91,
  484, 89, 0, 0, 0, 179, 0, 0, 476, 203, 0, 274, 0, 254, 636, 0, 236, 334, 486, 306, 186, 187, 304,
  150, 0, 484, 90, 0, 92, 484, 0, 201, 0, 645, 273, 239, 237, 238, 0, 419, 243, 293, 220, 483, 309,
  188, 297, 311, 310, 0, 314, 645, 647, 410, 131, 0, 484, 0, 202, 0, 421, 380, 418, 480, 312, 313,
  0, 0, 0, 60, 0, 410, 132, 249, 380, 420, 307, 647, 134, 129, 60, 0, 422, 0, 130, 133 };

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] = { -854, -854, -306, -854, -854, 731, -47, -854, -854, -854, -854,
  -743, -76, 0, 8, -854, -854, -854, -854, 18, -331, -79, -651, -854, -854, -854, -854, -46, -45,
  -52, -130, -854, -854, 78, -30, -29, -28, -854, -854, -1, -375, -854, -854, 113, -854, -854, -854,
  -169, -601, 11, -96, -335, 303, 151, -854, -854, -854, -854, 304, 21, 338, -854, 9, -854, 6, -854,
  -854, -854, -854, -854, 57, -854, -854, -854, -854, -854, -854, 629, 178, -792, -854, -854, -854,
  823, -854, -854, -854, 28, -99, 17, -62, -854, -854, -214, -402, -854, -854, -265, -251, -453,
  -435, -854, -854, 91, -854, -854, -125, -854, -152, -854, -854, -854, -56, -854, -854, -854, -854,
  22, -854, -854, -854, -854, 12, -854, 140, -535, -854, -854, -854, -60, -854, -854, -142, -854,
  -854, -854, -854, -854, -854, 10, 435, -232, 438, -854, 96, 42, -254, -854, -192, -854, -542,
  -854, -798, -854, -854, -168, -854, -854, -854, -372, -854, -854, -391, -854, -854, 103, -854,
  -854, -854, 1085, 936, 1026, 71, -854, -854, 276, 821, -5, -854, 33, -854, 248, -17, -38, -854,
  -4, 855, -854, -854, -401, -854, 44, 279, -854, -854, 111, -707, -854, -854, -854, -854, -854,
  -854, -854, -854, -854, -854, 352, 249, 211, -337, 497, -854, 498, -854, 235, -854, 219, -854,
  -379, -854, -330, -854, -785, -854, -854, -854, 150, -854, -268, -854, -854, -854, 375, 241, -854,
  -854, -854, -854, -854, 166, 152, 38, -516, -713, -854, -419, 27, -467, -854, 24, -854, 43, -854,
  -853, -854, -559, -854, -460, -854, -854, -854, -193, -854, -854, -854, 403, -854, -162, -350,
  -854, -349, 29, -514, -854, -532, -854 };

/* YYDEFGOTO[NTERM-NUM].  */
static const short yydefgoto[] = { -1, 1, 2, 4, 56, 284, 58, 59, 60, 392, 61, 62, 63, 286, 65, 276,
  66, 809, 549, 304, 413, 414, 552, 548, 679, 680, 870, 931, 932, 685, 686, 807, 803, 687, 68, 69,
  70, 421, 71, 287, 424, 569, 566, 567, 893, 288, 814, 972, 1025, 73, 74, 510, 518, 511, 385, 386,
  796, 969, 387, 75, 268, 76, 289, 667, 290, 501, 367, 770, 496, 769, 497, 498, 856, 499, 859, 500,
  926, 775, 648, 920, 921, 965, 991, 291, 80, 81, 82, 935, 880, 881, 84, 433, 820, 85, 454, 455,
  832, 456, 86, 458, 598, 599, 600, 438, 439, 740, 708, 824, 984, 957, 958, 986, 298, 299, 896, 459,
  840, 882, 825, 952, 312, 586, 431, 574, 575, 579, 580, 704, 899, 705, 822, 982, 465, 466, 612,
  467, 468, 757, 915, 292, 343, 404, 463, 748, 405, 406, 776, 993, 344, 759, 345, 453, 848, 916,
  1015, 994, 923, 471, 854, 460, 839, 603, 849, 605, 743, 744, 833, 907, 908, 688, 89, 241, 242,
  435, 92, 93, 94, 273, 444, 274, 226, 97, 98, 275, 407, 305, 100, 101, 102, 103, 594, 888, 105,
  355, 452, 106, 107, 227, 1011, 1012, 1032, 1045, 642, 643, 779, 853, 644, 108, 109, 110, 350, 351,
  352, 353, 619, 595, 354, 571, 6, 396, 397, 473, 474, 583, 584, 988, 989, 277, 278, 111, 359, 481,
  482, 483, 484, 485, 766, 627, 628, 540, 721, 722, 723, 752, 753, 842, 754, 725, 651, 789, 790,
  914, 587, 844, 726, 727, 755, 828, 368, 730, 829, 827, 731, 508, 506, 507, 732, 756, 363, 370,
  492, 493, 494, 222, 223, 224, 225 };

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const short yytable[] = { 95, 99, 285, 333, 72, 64, 399, 239, 472, 608, 419, 78, 488, 317,
  77, 87, 519, 475, 476, 255, 400, 504, 83, 67, 602, 300, 301, 302, 728, 310, 243, 245, 279, 375,
  240, 364, 634, 334, 96, 843, 652, 653, 369, 237, 700, 514, 905, 420, 513, 104, 570, 710, 593, 238,
  389, 487, 918, 398, 868, 3, 607, 221, 699, 371, 550, 1006, 550, 925, 244, 604, 220, 331, 738, 243,
  293, 321, 91, 601, 550, 246, 550, 243, 606, -372, 115, 550, 315, 318, 823, -372, 365, 457, 390,
  -2, 243, 83, 67, 360, 332, 365, 777, 1007, 1008, 581, -371, 739, 777, 329, 271, 550, -371, 244,
  992, 311, 377, 589, 116, 330, 610, 244, 608, 114, 294, 611, 316, 319, 550, 729, 604, 529, 306,
  646, 244, 633, 968, 432, 543, 280, 1016, 504, 658, 664, 366, 124, 125, 362, 116, 349, 303, 91,
  400, 366, 671, 470, 220, 647, 256, 257, 258, 259, 260, 261, 262, 551, -60, 734, -60, -60, 724,
  -60, 335, 607, 372, 373, 263, 264, 265, 551, 457, 734, 629, 659, 660, 815, 219, -60, 970, 542,
  -60, 694, 983, 339, 340, 737, -373, 1017, 124, 125, 218, 378, 480, 380, 33, 266, 672, 267, 618,
  38, 551, -60, 337, 457, -60, 588, 346, 590, 736, 384, 593, -373, 666, 436, 281, 5, 437, 734, -60,
  124, 125, -60, 747, 334, 788, 788, 788, 843, 197, 315, 38, 777, 243, 201, 295, 217, 240, 307, 585,
  347, 843, 1047, 843, 348, -375, 383, -492, 328, 777, 339, 340, 346, -492, 861, -374, 238, -378,
  341, 315, 55, 219, 358, 374, 661, 417, 449, -339, 316, 662, -375, 244, 1010, 617, 216, 218, 361,
  -370, 243, 428, -374, 243, -378, -370, 445, 347, 447, 201, 377, 348, 906, 296, 297, 243, 329,
  1031, -117, 316, 909, 423, 83, 238, 308, 418, 330, -379, 391, 326, 777, 709, 479, 692, 338, 389,
  693, 457, 244, -373, 201, 244, 217, 356, 357, 315, 341, 1022, -492, 294, 1023, 554, -379, 244,
  -492, 320, -365, 395, 401, 728, 516, 517, -365, 415, -373, -377, 675, 676, 677, -376, -375, 337,
  450, 777, 663, 902, 91, 904, -329, 335, 216, 334, 314, 316, -173, -494, 321, -363, 1039, 417,
  -377, -494, 948, -363, -376, -375, 964, 777, 521, -153, 897, 525, -176, 379, 1039, 381, 1052, 961,
  -173, 780, 243, 315, 112, 113, 963, 1052, 220, 220, 855, 337, 410, 728, 220, 220, 220, 555, 525,
  418, 462, 592, 701, 337, 981, 321, 202, 512, 522, 512, 243, 418, 788, 678, 1043, 220, 417, 582,
  430, 728, 244, 316, 767, 329, 427, 1050, 440, 1043, 417, 38, 954, 306, 486, 238, 857, 556, 418,
  415, 1055, 321, 1018, 434, 1019, 745, 315, 746, -341, 308, 244, 326, 894, 521, -341, 1020, 418,
  626, 1021, 220, 247, 248, 249, 250, 251, 252, 253, 691, 418, 446, 38, 448, 788, 724, 788, 409,
  557, 1035, 1036, 409, 980, 347, 639, 641, 316, 348, 637, -342, 335, 573, 197, 522, 813, -342, 415,
  649, 247, 248, 249, 250, 251, 252, 253, 526, 219, 219, 415, 314, 527, 924, 219, 219, 219, 847,
  320, 220, 860, -329, 218, 218, 640, 21, 22, 847, 218, 218, 218, -329, 337, -374, 845, 219, 846,
  862, 504, 724, 314, 980, 528, 847, 271, 555, 33, -329, 451, 218, -364, 512, 512, -378, 544, 978,
  -364, 243, -374, 541, 558, 315, -379, 577, 254, 724, -263, 217, 217, 613, -368, 596, 597, 217,
  217, 217, -368, 219, -378, -369, 615, -329, 409, 556, 409, -369, 622, -379, 509, 616, 624, 218,
  -377, 1003, 217, 244, 922, 632, -278, 316, 464, 282, 630, -280, 314, 216, 216, 654, 657, -367,
  665, 216, 216, 216, 941, -367, 541, -377, -376, 547, 320, -117, -117, 668, 669, 423, 553, 670,
  457, 243, 673, 674, 216, 219, 611, -366, 217, 689, 696, 951, 703, -366, 698, -376, 733, 735, 758,
  218, 741, 409, 702, 409, 681, 682, 683, 243, 320, 1024, 835, 836, 837, 838, 568, 967, 742, 761,
  1038, 244, 762, 765, 314, 979, 768, 662, 216, 320, 793, 794, 778, 745, 797, 806, 504, 409, 314,
  457, 457, 320, 1040, 802, 1042, 1054, 217, 244, 799, 800, 805, 512, 504, 341, 812, 821, 823, 1051,
  798, 826, 247, 248, 249, 250, 251, 252, 253, 650, 1014, 830, 746, 898, 895, -335, 409, 409, 919,
  927, 949, 956, 342, 928, 940, 57, 216, 314, 220, 959, 858, 983, 962, 1002, 966, 1004, 626, 960,
  1029, 457, 985, 774, 990, 867, 871, 872, 892, 321, 33, 946, 625, 804, 256, 257, 258, 259, 260,
  261, 262, 521, 873, 874, 690, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 247, 248, 249,
  250, 251, 252, 253, 333, 263, 264, 265, 38, 95, 887, 720, 706, 72, 869, 285, 409, 997, 320, 811,
  878, 522, 33, 877, 883, 875, 795, 655, 917, 656, 243, 620, 67, 33, 697, 876, 720, 939, 334, 851,
  781, 79, 879, 974, 831, 987, 314, 96, 1013, 719, 901, 816, 950, 1005, 546, 1030, 545, 841, 38,
  834, 1034, 695, 314, 645, 477, 478, 219, 760, 244, 38, 623, 272, 331, 1037, 719, 720, 720, 720,
  763, 900, 218, 913, 910, 614, 903, 886, 95, 938, 388, 334, 423, 930, 0, 0, 315, 95, 0, 934, 55,
  332, 933, 936, 0, 0, 0, 0, 0, 0, 329, 67, 285, 801, 0, 719, 719, 719, 0, 0, 330, 0, 0, 0, 942,
  810, 96, 217, 334, 322, 334, 325, 327, 999, 316, 96, 0, 0, 429, 95, 975, 0, 321, 0, 95, 977, 0,
  220, 95, 930, 973, 271, 0, 315, 521, 934, 568, 0, 933, 936, 0, 329, 971, 0, 886, 216, 0, 238, 0,
  336, 0, 330, 0, 943, 0, 942, 0, 96, 720, 0, 0, 0, 96, 0, 998, 95, 96, 95, 976, 469, 0, 316, 522,
  0, 0, 0, 243, 1009, 0, 321, 329, 0, 0, 0, 852, 417, 996, 0, 1000, 1046, 238, 521, 942, 337, 942,
  0, 719, 0, 0, 243, 886, 0, 1053, 96, 943, 96, 0, 0, 0, 0, 0, 1027, 0, 0, 244, 0, 417, 0, 417, 890,
  0, 0, 313, 418, 0, 323, 90, 0, 0, 522, 469, 417, 0, 0, 0, 850, 0, 244, 0, 0, 0, 219, 0, 943, 337,
  0, 0, 0, 720, 1033, 720, 0, 0, 0, 418, 218, 418, 0, 270, 325, 327, 415, 0, 220, 409, 0, 0, 0, 0,
  418, 220, 220, 0, 0, 0, 0, 0, 929, 0, 0, 0, 337, 0, 337, 0, 88, 719, 220, 719, 325, 327, 0, 415,
  416, 415, 336, 469, 0, 0, 720, 217, 0, 0, 0, 0, 0, 0, 415, 720, 720, 720, 0, 0, 706, 720, 324,
  995, 269, 0, 0, 0, 720, 0, 409, 0, 408, 0, 0, 0, 408, 441, 442, 443, 0, 0, 0, 388, 719, 216, 0, 0,
  1026, 0, 336, 0, 1028, 719, 719, 719, 0, 0, 0, 719, 0, 720, 336, 0, 314, 0, 719, 256, 257, 258,
  259, 260, 261, 262, 720, 0, 720, 0, 720, 1041, 382, 0, 0, 0, 219, 263, 264, 265, 0, 0, 0, 219,
  219, 0, 0, 0, 0, 0, 218, 719, 0, 0, 416, 0, 720, 218, 218, 0, 219, 382, 323, 0, 719, 33, 719, 0,
  719, 0, 0, 0, 0, 314, 218, 0, 0, 0, 0, 0, 256, 257, 258, 259, 260, 261, 262, 408, 0, 408, 0, 0,
  325, 327, 719, 217, 0, 320, 263, 264, 265, 38, 217, 217, 0, 0, 0, 270, 0, 416, 0, 409, 0, 0, 0, 0,
  0, 0, 409, 217, 0, 416, 412, 0, 0, 313, 33, 0, 320, 0, 320, 0, 461, 216, 0, 0, 0, 336, 0, 0, 216,
  216, 0, 320, 0, 0, 270, 270, 0, 0, 0, 0, 324, 0, 408, 272, 408, 216, 0, 0, 0, 270, 0, 270, 0, 0,
  376, 0, 0, 0, 0, 808, 0, 0, 0, 0, 0, 0, 520, 0, 0, 0, 817, 818, 0, 0, 0, 0, 408, 0, 0, 382, 256,
  257, 258, 259, 260, 261, 262, 0, 325, 0, 0, 0, 0, 393, 394, 0, 0, 0, 263, 264, 265, 0, 0, 0, 0, 0,
  0, 461, 425, 0, 426, 0, 0, 408, 408, 0, 0, 0, 442, 443, 31, 309, 0, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0,
  7, 8, 9, 10, 11, 12, 13, 609, 0, 0, 0, 0, 707, 0, 520, 0, 270, 0, 14, 15, 16, 0, 17, 18, 19, 20,
  524, 0, 0, 21, 22, 23, 24, 25, 26, 27, 28, 29, 0, 30, 31, 32, 0, 0, 0, 0, 33, 34, 35, 36, 37, 0,
  0, 0, 0, 764, 408, 0, 55, 0, 0, 0, 270, 0, 0, 270, 0, 0, 0, 325, 327, 0, 0, 0, 0, 515, 0, 0, 0, 0,
  0, 0, 38, 270, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 0, 0, 0, 0, 0, 336,
  0, 0, 0, 0, 0, 55, 0, 0, 0, 270, 0, 0, 609, 0, 0, 572, 0, 0, 576, 0, 944, 945, 0, 947, 0, 0, 0,
  638, 559, 560, 561, 562, 563, 564, 565, 0, 591, 0, 0, 0, 0, 0, 0, 0, 0, 0, 263, 264, 265, 0, 0, 7,
  8, 9, 10, 11, 12, 13, 126, 127, 128, 129, 130, 131, 132, 133, 134, 0, 621, 14, 15, 16, 0, 17, 18,
  19, 20, 0, 270, 0, 21, 22, 23, 24, 25, 26, 27, 235, 29, 863, 864, 31, 32, 0, 0, 0, 0, 33, 34, 35,
  0, 865, 0, 0, 0, 0, 0, 0, 1001, 0, 256, 257, 258, 259, 260, 261, 262, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 263, 264, 265, 0, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 866,
  309, 0, 0, 0, 0, 33, 0, 0, 0, 0, 0, 55, 0, 270, 270, 270, 0, 0, 0, 0, 0, 7, 8, 9, 10, 11, 12, 13,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 272, 14, 15, 16, 0, 17, 18, 19, 20, 0, 0, 0, 21, 22, 23, 24, 25, 26,
  27, 235, 29, 863, 236, 31, 282, 520, 0, 408, 0, 33, 34, 0, 55, 283, 771, 772, 773, 0, 270, 256,
  257, 258, 259, 260, 261, 262, 0, 0, 0, 0, 0, 885, 0, 0, 0, 0, 336, 263, 264, 265, 0, 0, 0, 0, 0,
  38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 0, 0, 33, 408, 0, 0, 0, 0,
  0, 0, 0, 55, 0, 0, 576, 0, 256, 257, 258, 259, 260, 261, 262, 0, 336, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  263, 264, 265, 0, 38, 0, 0, 0, 681, 682, 683, 937, 0, 416, 0, 0, 684, 0, 0, 382, 937, 422, 0, 0,
  90, 423, 270, 336, 33, 336, 270, 0, 270, 55, 256, 257, 258, 259, 260, 261, 262, 0, 0, 0, 416, 0,
  416, 0, 0, 0, 336, 0, 263, 264, 265, 0, 0, 0, 0, 416, 0, 336, 937, 0, 38, 0, 0, 937, 0, 0, 0, 937,
  0, 0, 0, 0, 382, 461, 0, 0, 33, 266, 0, 884, 0, 889, 0, 0, 270, 891, 0, 0, 0, 55, 0, 0, 0, 0, 408,
  0, 0, 0, 0, 0, 0, 408, 0, 0, 937, 0, 937, 0, 0, 270, 0, 0, 38, 0, 17, 18, 19, 0, 0, 0, 0, 21, 22,
  23, 24, 25, 26, 27, 235, 29, 609, 236, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 269, 117, 118, 119, 120,
  121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152, 153, 0, 154, 155, 156, 157, 0,
  0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
  177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195,
  196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 489, 197, 0, 198, 199, 200, 201,
  0, 490, 203, 204, 491, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 117, 118, 119, 120,
  121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152, 153, 0, 154, 155, 156, 157, 0,
  0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
  177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195,
  196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 489, 197, 495, 198, 199, 200,
  201, 0, 490, 203, 204, 0, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 117, 118, 119,
  120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138,
  139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152, 153, 0, 154, 155, 156,
  157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174,
  175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193,
  194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 489, 197, 0, 198, 199,
  200, 201, 0, 490, 203, 204, 502, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 117, 118,
  119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137,
  138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152, 153, 0, 154, 155,
  156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173,
  174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
  193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 489, 197, 0, 198,
  199, 200, 201, 505, 490, 203, 204, 0, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 117,
  118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136,
  137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152, 153, 0, 154,
  155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
  173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
  192, 193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 489, 197,
  523, 198, 199, 200, 201, 0, 490, 203, 204, 0, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214,
  215, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
  135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152, 153,
  0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
  172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
  191, 192, 193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 489,
  197, 578, 198, 199, 200, 201, 0, 490, 203, 204, 0, 205, 206, 207, 208, 209, 210, 211, 212, 213,
  214, 215, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133,
  134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152,
  153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
  171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
  190, 191, 192, 193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
  489, 197, 819, 198, 199, 200, 201, 0, 490, 203, 204, 0, 205, 206, 207, 208, 209, 210, 211, 212,
  213, 214, 215, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132,
  133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151,
  152, 153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
  170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188,
  189, 190, 191, 192, 193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
  53, 489, 197, 1044, 198, 199, 200, 201, 0, 490, 203, 204, 0, 205, 206, 207, 208, 209, 210, 211,
  212, 213, 214, 215, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131,
  132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150,
  151, 152, 153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168,
  169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187,
  188, 189, 190, 191, 192, 193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
  52, 53, 489, 197, 1048, 198, 199, 200, 201, 0, 490, 203, 204, 0, 205, 206, 207, 208, 209, 210,
  211, 212, 213, 214, 215, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
  131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149,
  150, 151, 152, 153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167,
  168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186,
  187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
  51, 52, 53, 503, 197, 0, 198, 199, 200, 201, 0, 490, 203, 204, 0, 205, 206, 207, 208, 209, 210,
  211, 212, 213, 214, 215, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
  131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149,
  150, 151, 152, 153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167,
  168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186,
  187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
  51, 52, 53, 631, 197, 0, 198, 199, 200, 201, 0, 490, 203, 204, 0, 205, 206, 207, 208, 209, 210,
  211, 212, 213, 214, 215, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
  131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149,
  150, 151, 152, 153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167,
  168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186,
  187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
  51, 52, 53, 1049, 197, 0, 198, 199, 200, 201, 0, 490, 203, 204, 0, 205, 206, 207, 208, 209, 210,
  211, 212, 213, 214, 215, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
  131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149,
  150, 151, 152, 153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167,
  168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186,
  187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
  51, 52, 53, 1056, 197, 0, 198, 199, 200, 201, 0, 490, 203, 204, 0, 205, 206, 207, 208, 209, 210,
  211, 212, 213, 214, 215, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
  131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149,
  150, 151, 152, 153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167,
  168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186,
  187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
  51, 52, 53, 0, 197, 0, 198, 199, 200, 201, 0, 202, 203, 204, 0, 205, 206, 207, 208, 209, 210, 211,
  212, 213, 214, 215, 117, 118, 119, 120, 121, 122, 123, 372, 373, 126, 127, 128, 129, 130, 131,
  132, 133, 134, 135, 136, 137, 711, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150,
  151, 152, 153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168,
  169, 170, 171, 172, 173, 782, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187,
  188, 189, 190, 191, 192, 193, 0, 712, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
  53, 911, 613, 912, 783, 714, 784, 374, 0, 786, 203, 717, 0, 205, 206, 787, 208, 209, 210, 211,
  212, 213, 214, 718, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131,
  132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150,
  151, 152, 153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168,
  169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187,
  188, 189, 190, 191, 192, 193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
  52, 53, 0, 197, 0, 198, 199, 200, 201, 0, 490, 203, 204, 0, 205, 206, 207, 208, 209, 210, 211,
  212, 213, 214, 215, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131,
  132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150,
  151, 152, 153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168,
  169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187,
  188, 189, 190, 191, 192, 193, 194, 195, 196, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
  52, 53, 0, 197, 0, 198, 199, 200, 201, 0, 0, 203, 204, 0, 205, 206, 207, 208, 209, 210, 211, 212,
  213, 214, 215, 117, 118, 119, 120, 121, 122, 123, 372, 373, 126, 127, 128, 129, 130, 131, 132,
  133, 134, 135, 136, 137, 711, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151,
  152, 153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
  170, 171, 172, 173, 782, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188,
  189, 190, 191, 192, 193, 0, 712, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0,
  613, 0, 783, 714, 784, 374, 785, 786, 203, 717, 0, 205, 206, 787, 208, 209, 210, 211, 212, 213,
  214, 718, 117, 118, 119, 120, 121, 122, 123, 372, 373, 126, 127, 128, 129, 130, 131, 132, 133,
  134, 135, 136, 137, 711, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152,
  153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
  171, 172, 173, 782, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
  190, 191, 192, 193, 0, 712, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 613,
  0, 783, 714, 784, 374, 791, 786, 203, 717, 0, 205, 206, 787, 208, 209, 210, 211, 212, 213, 214,
  718, 117, 118, 119, 120, 121, 122, 123, 372, 373, 126, 127, 128, 129, 130, 131, 132, 133, 134,
  135, 136, 137, 711, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152, 153,
  0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
  172, 173, 782, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
  191, 192, 193, 0, 712, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 613, 0,
  783, 714, 784, 374, 792, 786, 203, 717, 0, 205, 206, 787, 208, 209, 210, 211, 212, 213, 214, 718,
  117, 118, 119, 120, 121, 122, 123, 372, 373, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
  136, 137, 711, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152, 153, 0,
  154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
  172, 173, 782, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
  191, 192, 193, 0, 712, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 613, 0,
  783, 714, 784, 374, 0, 786, 203, 717, 953, 205, 206, 787, 208, 209, 210, 211, 212, 213, 214, 718,
  117, 118, 119, 120, 121, 122, 123, 372, 373, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
  136, 137, 711, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152, 153, 0,
  154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
  172, 173, 782, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
  191, 192, 193, 0, 712, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 613, 0,
  783, 714, 784, 374, 0, 786, 203, 717, 955, 205, 206, 787, 208, 209, 210, 211, 212, 213, 214, 718,
  117, 118, 119, 120, 121, 122, 123, 372, 373, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
  136, 137, 711, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152, 153, 0,
  154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
  172, 173, 365, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
  191, 192, 193, 0, 712, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 613, 0,
  713, 714, 715, 374, 0, 716, 203, 717, 0, 205, 206, 366, 208, 209, 210, 211, 212, 213, 214, 718,
  -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593,
  -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, 0, -593,
  -593, -593, -593, -593, -593, 0, -593, -593, -593, -593, 0, 0, -593, -593, -593, -593, -593, -593,
  -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593,
  -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, 0, -593, 0,
  -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, -593, 0, -593,
  0, -629, -593, -593, -593, 0, -593, -593, -593, 0, -593, -593, -593, -593, -593, -593, -593, -593,
  -593, -593, -593, 117, 118, 119, 120, 121, 122, 123, 372, 373, 126, 127, 128, 129, 130, 131, 132,
  133, 134, 135, 136, 137, 711, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151,
  152, 153, 0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
  170, 171, 172, 173, 749, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188,
  189, 190, 191, 192, 193, 0, 712, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0,
  613, 0, 0, 714, 0, 374, 0, 750, 203, 717, 0, 205, 206, 751, 208, 209, 210, 211, 212, 213, 214,
  718, 117, 118, 119, 120, 121, 122, 123, 372, 373, 126, 127, 128, 129, 130, 131, 132, 133, 134,
  135, 136, 137, 711, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152, 153,
  0, 154, 155, 156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
  172, 173, 0, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
  192, 193, 0, 712, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 613, 0, 0,
  714, 0, 374, 0, 716, 203, 717, 0, 205, 206, 0, 208, 209, 210, 211, 212, 213, 214, 718, 117, 118,
  119, 120, 121, 122, 123, 372, 373, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137,
  711, 139, 140, 141, 142, 143, 144, 145, 146, 147, 0, 148, 149, 150, 151, 152, 153, 0, 154, 155,
  156, 157, 0, 0, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 0,
  175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 0,
  0, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 613, 0, 0, 714, 0, 374, 0, 0,
  203, 717, 0, 205, 206, 0, 208, 209, 210, 211, 212, 213, 214, 718, 228, 229, 230, 231, 232, 233,
  234, 0, 0, 530, 0, 0, 0, 0, 0, 0, 0, 0, 135, 136, 137, 0, 17, 18, 19, 20, 0, 0, 0, 21, 22, 23, 24,
  25, 26, 27, 235, 29, 0, 236, 0, 0, 0, 0, 0, 0, 33, 34, 0, 0, 0, 531, 532, 0, 0, 0, 0, 0, 171, 172,
  173, 533, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
  192, 193, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 0, 0, 534, 0,
  535, 536, 0, 537, 203, 538, 0, 205, 206, 539, 208, 209, 210, 211, 212, 213, 214, 7, 8, 9, 10, 11,
  12, 13, 0, 256, 257, 258, 259, 260, 261, 262, 0, 0, 0, 14, 15, 16, 0, 17, 18, 19, 20, 263, 264,
  265, 21, 22, 23, 24, 25, 26, 27, 235, 29, 0, 236, 31, 0, 0, 0, 0, 0, 33, 34, 35, 309, 0, 0, 0, 0,
  33, 0, 0, 0, 0, 0, 256, 257, 258, 259, 260, 261, 262, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 263, 264,
  265, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 7, 8, 9, 10, 11, 12,
  13, 33, 0, 0, 0, 0, 55, 0, 0, 0, 0, 0, 14, 15, 16, 0, 17, 18, 19, 20, 0, 0, 0, 21, 22, 23, 24, 25,
  26, 27, 235, 29, 0, 236, 31, 282, 0, 38, 0, 0, 33, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45,
  46, 47, 48, 49, 50, 51, 52, 53, 7, 8, 9, 10, 11, 12, 13, 0, 0, 0, 530, 0, 55, 0, 0, 0, 0, 0, 14,
  15, 16, 0, 17, 18, 19, 20, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 235, 29, 0, 236, 31, 0, 0, 0, 0,
  0, 33, 34, 0, 0, 0, 0, 531, 532, 0, 0, 0, 0, 0, 171, 172, 173, 533, 175, 176, 177, 178, 179, 180,
  181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 38, 0, 39, 40, 41, 42, 43, 44,
  45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 0, 0, 0, 534, 0, 535, 536, 0, 537, 203, 538, 55, 205, 206,
  539, 208, 209, 210, 211, 212, 213, 214, 256, 257, 258, 259, 260, 261, 262, 256, 257, 258, 259,
  260, 261, 262, 0, 0, 0, 0, 263, 264, 265, 0, 0, 0, 0, 263, 264, 265, 0, 0, 256, 257, 258, 259,
  260, 261, 262, 0, 0, 0, 31, 0, 0, 0, 0, 0, 33, 0, 263, 264, 265, 0, 0, 33, 256, 257, 258, 259,
  260, 261, 262, 256, 257, 258, 259, 260, 261, 262, 0, 0, 0, 0, 263, 264, 265, 0, 33, 0, 0, 263,
  264, 265, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0, 33, 0, 0, 0,
  55, 38, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 411, 1022, 0, 0, 1023, 0, 0, 38, 0, 0, 0,
  0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 411, 0, 0, 0, 412, 0, 0, 0, 1022, 0, 0, 1023, 228, 229,
  230, 231, 232, 233, 234, 21, 22, 23, 24, 25, 26, 27, 235, 29, 402, 236, 135, 136, 137, 0, 17, 18,
  19, 20, 403, 0, 0, 21, 22, 23, 24, 25, 26, 27, 235, 29, 0, 236, 31, 282, 0, 0, 0, 0, 33, 34, 0, 0,
  283, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
  0, 0, 0, 0, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 228, 229, 230,
  231, 232, 233, 234, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 135, 136, 137, 0, 17, 18, 19, 20, 0, 0, 0,
  21, 22, 23, 24, 25, 26, 27, 235, 29, 0, 236, 31, 0, 0, 0, 0, 0, 33, 34, 35, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 39, 40, 41,
  42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 228, 229, 230, 231, 232, 233, 234, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 135, 136, 137, 0, 17, 18, 19, 20, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 235, 29,
  0, 236, 31, 0, 0, 0, 0, 0, 33, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
  52, 53, 228, 229, 230, 231, 232, 233, 234, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 135, 136, 137, 0, 17,
  18, 19, 20, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 235, 29, 0, 236, 0, 0, 0, 0, 0, 0, 33, 34, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 228, 229, 230, 231, 232, 233,
  234, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 135, 136, 137, 0, 635, 0, 636, 20, 0, 0, 0, 21, 22, 23, 24,
  25, 26, 27, 235, 29, 0, 236, 0, 0, 0, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46,
  47, 48, 49, 50, 51, 52, 53, 228, 229, 230, 231, 232, 233, 234, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  135, 136, 137, 0, 0, 0, 0, 0, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 235, 29, 0, 236, 0, 0, 0, 0, 0,
  0, 33, 34, 0, 0, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 235, 29, 402, 236, 0, 0, 0, 0, 0, 0, 0, 0,
  403, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
  346, 0, 0, 0, 0, 0, 0, 0, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, -287, 0,
  0, 0, 0, 0, 0, 0, 0, 347, 0, 0, 0, 348, 21, 22, 23, 24, 25, 26, 27, 235, 29, 402, 236, 0, 0, 0, 0,
  0, 0, 0, 0, 403, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  346, 0, 0, 0, 0, 0, 0, 0, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 347, 0, 0, 0, 348 };

static const short yycheck[] = { 5, 5, 78, 99, 5, 5, 271, 35, 345, 462, 278, 5, 362, 92, 5, 5, 388,
  347, 348, 36, 271, 370, 5, 5, 459, 81, 82, 83, 587, 91, 35, 35, 70, 226, 35, 197, 496, 99, 5, 752,
  507, 508, 204, 35, 576, 380, 831, 279, 379, 5, 425, 586, 453, 35, 246, 361, 854, 271, 801, 0, 462,
  32, 576, 225, 3, 19, 3, 859, 35, 460, 32, 99, 78, 78, 78, 92, 5, 456, 3, 35, 3, 86, 461, 102, 12,
  3, 91, 92, 3, 108, 64, 342, 254, 0, 99, 78, 78, 114, 99, 64, 642, 55, 56, 434, 102, 111, 648, 99,
  37, 3, 108, 78, 965, 91, 12, 446, 44, 99, 102, 86, 573, 52, 78, 107, 91, 92, 3, 587, 519, 397, 86,
  26, 99, 107, 926, 297, 401, 103, 991, 488, 515, 520, 116, 10, 11, 116, 44, 105, 108, 78, 401, 116,
  64, 345, 116, 50, 3, 4, 5, 6, 7, 8, 9, 102, 103, 102, 103, 106, 587, 106, 99, 573, 10, 11, 21, 22,
  23, 102, 429, 102, 486, 516, 517, 697, 32, 103, 929, 401, 106, 568, 105, 10, 11, 594, 85, 993, 10,
  11, 32, 237, 53, 239, 49, 50, 116, 52, 474, 85, 102, 103, 99, 462, 106, 445, 78, 447, 591, 245,
  619, 110, 526, 45, 72, 4, 48, 102, 103, 10, 11, 106, 609, 293, 651, 652, 653, 948, 103, 242, 85,
  781, 245, 108, 46, 32, 245, 102, 439, 111, 961, 1034, 963, 115, 85, 245, 107, 110, 798, 10, 11,
  78, 113, 796, 85, 245, 85, 84, 271, 114, 116, 24, 108, 102, 277, 335, 102, 242, 107, 110, 245,
  986, 472, 32, 116, 103, 102, 290, 290, 110, 293, 110, 108, 329, 111, 331, 108, 12, 115, 832, 102,
  103, 305, 293, 1009, 103, 271, 840, 106, 290, 290, 90, 277, 293, 85, 105, 95, 857, 584, 355, 104,
  100, 512, 107, 573, 290, 85, 108, 293, 116, 109, 110, 335, 84, 103, 107, 290, 106, 415, 110, 305,
  113, 92, 102, 102, 272, 903, 383, 384, 108, 277, 110, 85, 29, 30, 31, 85, 85, 245, 335, 900, 108,
  827, 290, 829, 112, 293, 116, 428, 91, 335, 26, 107, 388, 102, 1024, 379, 110, 113, 893, 108, 110,
  110, 916, 924, 388, 102, 820, 391, 44, 238, 1040, 240, 1042, 908, 50, 648, 400, 401, 25, 26, 915,
  1051, 363, 364, 775, 293, 103, 965, 369, 370, 371, 415, 416, 379, 342, 452, 577, 305, 949, 435,
  110, 378, 388, 380, 428, 391, 844, 104, 1028, 390, 434, 435, 106, 991, 400, 401, 628, 428, 287,
  1039, 108, 1041, 446, 85, 903, 400, 103, 428, 777, 415, 416, 379, 1052, 469, 32, 304, 34, 106,
  462, 108, 102, 241, 428, 243, 813, 469, 108, 45, 434, 485, 48, 432, 3, 4, 5, 6, 7, 8, 9, 557, 446,
  330, 85, 332, 902, 903, 904, 275, 416, 1020, 1021, 279, 944, 111, 498, 498, 462, 115, 498, 102,
  428, 429, 103, 469, 105, 108, 434, 500, 3, 4, 5, 6, 7, 8, 9, 103, 363, 364, 446, 242, 102, 857,
  369, 370, 371, 108, 277, 488, 781, 112, 363, 364, 498, 32, 33, 108, 369, 370, 371, 112, 428, 85,
  107, 390, 109, 798, 894, 965, 271, 1001, 105, 108, 480, 557, 49, 112, 336, 390, 102, 516, 517, 85,
  102, 941, 108, 569, 110, 400, 102, 573, 85, 103, 103, 991, 109, 363, 364, 103, 102, 32, 33, 369,
  370, 371, 108, 432, 110, 102, 108, 112, 378, 557, 380, 108, 113, 110, 376, 109, 107, 432, 85, 979,
  390, 569, 857, 113, 103, 573, 105, 44, 104, 108, 335, 363, 364, 102, 102, 102, 102, 369, 370, 371,
  886, 108, 457, 110, 85, 407, 379, 102, 103, 3, 112, 106, 414, 112, 886, 641, 109, 113, 390, 488,
  107, 102, 432, 106, 102, 900, 84, 108, 107, 110, 102, 102, 615, 488, 108, 445, 109, 447, 29, 30,
  31, 667, 415, 995, 13, 14, 15, 16, 421, 924, 112, 106, 1023, 641, 113, 84, 401, 943, 104, 107,
  432, 434, 102, 102, 643, 106, 104, 37, 1038, 479, 415, 943, 944, 446, 1026, 106, 1028, 1048, 488,
  667, 113, 113, 107, 662, 1054, 84, 105, 107, 3, 1041, 667, 109, 3, 4, 5, 6, 7, 8, 9, 501, 989,
  102, 108, 84, 102, 113, 516, 517, 105, 109, 47, 67, 104, 102, 102, 5, 488, 462, 701, 109, 779,
  105, 109, 102, 109, 103, 764, 113, 102, 1001, 113, 641, 109, 801, 801, 801, 809, 775, 49, 890,
  485, 684, 3, 4, 5, 6, 7, 8, 9, 775, 801, 801, 554, 3, 4, 5, 6, 7, 8, 9, 21, 22, 23, 3, 4, 5, 6, 7,
  8, 9, 887, 21, 22, 23, 85, 801, 801, 587, 580, 801, 801, 878, 592, 973, 557, 693, 801, 775, 49,
  801, 801, 801, 662, 511, 853, 512, 822, 480, 801, 49, 573, 801, 612, 880, 887, 769, 649, 5, 801,
  929, 740, 957, 557, 801, 987, 587, 825, 698, 899, 982, 406, 1004, 405, 748, 85, 743, 1015, 569,
  573, 498, 354, 354, 701, 619, 822, 85, 482, 37, 887, 1022, 612, 651, 652, 653, 624, 822, 701, 844,
  842, 467, 828, 801, 878, 878, 246, 938, 106, 878, -1, -1, 886, 887, -1, 878, 114, 887, 878, 878,
  -1, -1, -1, -1, -1, -1, 887, 878, 973, 679, -1, 651, 652, 653, -1, -1, 887, -1, -1, -1, 887, 691,
  878, 701, 975, 93, 977, 95, 96, 974, 886, 887, -1, -1, 294, 929, 929, -1, 944, -1, 934, 934, -1,
  894, 938, 934, 929, 865, -1, 943, 944, 934, 693, -1, 934, 934, -1, 938, 929, -1, 878, 701, -1,
  934, -1, 99, -1, 938, -1, 887, -1, 938, -1, 929, 752, -1, -1, -1, 934, -1, 973, 975, 938, 977,
  929, 345, -1, 943, 944, -1, -1, -1, 986, 986, -1, 1001, 977, -1, -1, -1, 770, 995, 973, -1, 975,
  1032, 977, 1001, 975, 887, 977, -1, 752, -1, -1, 1009, 934, -1, 1045, 975, 938, 977, -1, -1, -1,
  -1, -1, 998, -1, -1, 986, -1, 1026, -1, 1028, 805, -1, -1, 91, 995, -1, 94, 5, -1, -1, 1001, 406,
  1041, -1, -1, -1, 764, -1, 1009, -1, -1, -1, 894, -1, 977, 938, -1, -1, -1, 842, 1012, 844, -1,
  -1, -1, 1026, 894, 1028, -1, 37, 243, 244, 995, -1, 1030, 858, -1, -1, -1, -1, 1041, 1037, 1038,
  -1, -1, -1, -1, -1, 863, -1, -1, -1, 975, -1, 977, -1, 5, 842, 1054, 844, 273, 274, -1, 1026, 277,
  1028, 245, 472, -1, -1, 893, 894, -1, -1, -1, -1, -1, -1, 1041, 902, 903, 904, -1, -1, 899, 908,
  94, 971, 37, -1, -1, -1, 915, -1, 917, -1, 275, -1, -1, -1, 279, 314, 315, 316, -1, -1, -1, 512,
  893, 894, -1, -1, 996, -1, 293, -1, 1000, 902, 903, 904, -1, -1, -1, 908, -1, 948, 305, -1, 886,
  -1, 915, 3, 4, 5, 6, 7, 8, 9, 961, -1, 963, -1, 965, 1027, 242, -1, -1, -1, 1030, 21, 22, 23, -1,
  -1, -1, 1037, 1038, -1, -1, -1, -1, -1, 1030, 948, -1, -1, 379, -1, 991, 1037, 1038, -1, 1054,
  271, 272, -1, 961, 49, 963, -1, 965, -1, -1, -1, -1, 943, 1054, -1, -1, -1, -1, -1, 3, 4, 5, 6, 7,
  8, 9, 378, -1, 380, -1, -1, 417, 418, 991, 1030, -1, 995, 21, 22, 23, 85, 1037, 1038, -1, -1, -1,
  227, -1, 434, -1, 1046, -1, -1, -1, -1, -1, -1, 1053, 1054, -1, 446, 106, -1, -1, 335, 49, -1,
  1026, -1, 1028, -1, 342, 1030, -1, -1, -1, 428, -1, -1, 1037, 1038, -1, 1041, -1, -1, 266, 267,
  -1, -1, -1, -1, 272, -1, 445, 480, 447, 1054, -1, -1, -1, 281, -1, 283, -1, -1, 227, -1, -1, -1,
  -1, 688, -1, -1, -1, -1, -1, -1, 388, -1, -1, -1, 699, 700, -1, -1, -1, -1, 479, -1, -1, 401, 3,
  4, 5, 6, 7, 8, 9, -1, 525, -1, -1, -1, -1, 266, 267, -1, -1, -1, 21, 22, 23, -1, -1, -1, -1, -1,
  -1, 429, 281, -1, 283, -1, -1, 516, 517, -1, -1, -1, 555, 556, 43, 44, -1, -1, -1, -1, 49, -1, -1,
  -1, -1, -1, -1, 3, 4, 5, 6, 7, 8, 9, 462, -1, -1, -1, -1, 582, -1, 469, -1, 381, -1, 21, 22, 23,
  -1, 25, 26, 27, 28, 391, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, -1, -1, -1,
  -1, 49, 50, 51, 52, 53, -1, -1, -1, -1, 625, 592, -1, 114, -1, -1, -1, 427, -1, -1, 430, -1, -1,
  -1, 639, 640, -1, -1, -1, -1, 381, -1, -1, -1, -1, -1, -1, 85, 448, 87, 88, 89, 90, 91, 92, 93,
  94, 95, 96, 97, 98, 99, 100, 101, 102, -1, -1, -1, -1, -1, 641, -1, -1, -1, -1, -1, 114, -1, -1,
  -1, 480, -1, -1, 573, -1, -1, 427, -1, -1, 430, -1, 888, 889, -1, 891, -1, -1, -1, 498, 3, 4, 5,
  6, 7, 8, 9, -1, 448, -1, -1, -1, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, -1, 3, 4, 5, 6, 7, 8, 9,
  12, 13, 14, 15, 16, 17, 18, 19, 20, -1, 480, 21, 22, 23, -1, 25, 26, 27, 28, -1, 549, -1, 32, 33,
  34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, -1, -1, -1, -1, 49, 50, 51, -1, 53, -1, -1, -1, -1,
  -1, -1, 976, -1, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1,
  -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 44, -1, -1, -1, -1,
  49, -1, -1, -1, -1, -1, 114, -1, 635, 636, 637, -1, -1, -1, -1, -1, 3, 4, 5, 6, 7, 8, 9, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, 865, 21, 22, 23, -1, 25, 26, 27, 28, -1, -1, -1, 32, 33, 34, 35,
  36, 37, 38, 39, 40, 41, 42, 43, 44, 775, -1, 858, -1, 49, 50, -1, 114, 53, 635, 636, 637, -1, 698,
  3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, 801, -1, -1, -1, -1, 887, 21, 22, 23, -1, -1, -1, -1, -1,
  85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, -1, -1, 49, 917, -1, -1,
  -1, -1, -1, -1, -1, 114, -1, -1, 698, -1, 3, 4, 5, 6, 7, 8, 9, -1, 938, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, 21, 22, 23, -1, 85, -1, -1, -1, 29, 30, 31, 878, -1, 995, -1, -1, 37, -1, -1, 886,
  887, 102, -1, -1, 801, 106, 803, 975, 49, 977, 807, -1, 809, 114, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1,
  1026, -1, 1028, -1, -1, -1, 998, -1, 21, 22, 23, -1, -1, -1, -1, 1041, -1, 1009, 929, -1, 85, -1,
  -1, 934, -1, -1, -1, 938, -1, -1, -1, -1, 943, 944, -1, -1, 49, 50, -1, 801, -1, 803, -1, -1, 865,
  807, -1, -1, -1, 114, -1, -1, -1, -1, 1046, -1, -1, -1, -1, -1, -1, 1053, -1, -1, 975, -1, 977,
  -1, -1, 890, -1, -1, 85, -1, 25, 26, 27, -1, -1, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, 1001,
  42, -1, -1, -1, -1, -1, -1, -1, -1, -1, 114, -1, 865, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39,
  40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
  64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
  88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, -1, 105, 106, 107, 108, -1,
  110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9,
  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
  -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
  58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
  82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104,
  105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
  3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
  29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52,
  53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
  77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
  101, 102, 103, -1, 105, 106, 107, 108, -1, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
  121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1,
  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
  72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
  96, 97, 98, 99, 100, 101, 102, 103, -1, 105, 106, 107, 108, 109, 110, 111, 112, -1, 114, 115, 116,
  117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
  19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42,
  43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
  67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
  91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, -1, 110, 111,
  112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36,
  37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
  61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
  85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
  107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5,
  6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54,
  55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
  79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102,
  103, 104, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122,
  123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
  27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50,
  51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
  75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98,
  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118,
  119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45,
  -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
  70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93,
  94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114,
  115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
  17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40,
  -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
  65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88,
  89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, -1, 105, 106, 107, 108, -1, 110,
  111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11,
  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35,
  36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
  60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
  84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, -1, 105, 106,
  107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5,
  6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54,
  55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
  79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102,
  103, -1, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122,
  123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
  27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50,
  51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
  75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98,
  99, 100, 101, 102, 103, -1, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118,
  119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45,
  -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
  70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93,
  94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114,
  115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
  17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40,
  -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
  65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88,
  89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, -1, 110,
  111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11,
  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35,
  36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
  60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
  84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, 105, 106,
  107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5,
  6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54,
  55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
  79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1,
  103, -1, 105, 106, 107, 108, -1, -1, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122,
  123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
  27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50,
  51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
  75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98,
  99, 100, 101, -1, 103, -1, 105, 106, 107, 108, 109, 110, 111, 112, -1, 114, 115, 116, 117, 118,
  119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45,
  -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
  70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93,
  94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, 105, 106, 107, 108, 109, 110, 111, 112, -1, 114,
  115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
  17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40,
  -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
  65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88,
  89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, 105, 106, 107, 108, 109, 110,
  111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11,
  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35,
  36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
  60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
  -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, 105, 106,
  107, 108, -1, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5,
  6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54,
  55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
  79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1,
  103, -1, 105, 106, 107, 108, -1, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
  123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
  27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50,
  51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
  75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98,
  99, 100, 101, -1, 103, -1, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118,
  119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45,
  -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
  70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93,
  94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114,
  115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
  17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40,
  -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
  65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88,
  89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, -1, 106, -1, 108, -1, 110, 111,
  112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36,
  37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
  61, 62, 63, -1, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1,
  85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, -1, 106, -1,
  108, -1, 110, 111, 112, -1, 114, 115, -1, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
  33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56,
  57, 58, 59, 60, 61, 62, 63, -1, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
  81, 82, 83, -1, -1, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1,
  -1, 106, -1, 108, -1, -1, 111, 112, -1, 114, 115, -1, 117, 118, 119, 120, 121, 122, 123, 124, 3,
  4, 5, 6, 7, 8, 9, -1, -1, 12, -1, -1, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25, 26, 27, 28, -1,
  -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1, 42, -1, -1, -1, -1, -1, -1, 49, 50, -1, -1, -1,
  54, 55, -1, -1, -1, -1, -1, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
  78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101,
  -1, -1, -1, 105, -1, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122,
  123, 3, 4, 5, 6, 7, 8, 9, -1, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, 21, 22, 23, -1, 25, 26, 27, 28, 21,
  22, 23, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1, 42, 43, -1, -1, -1, -1, -1, 49, 50, 51, 44, -1,
  -1, -1, -1, 49, -1, -1, -1, -1, -1, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, 21, 22, 23, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 3, 4, 5,
  6, 7, 8, 9, 49, -1, -1, -1, -1, 114, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25, 26, 27, 28, -1, -1,
  -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, -1, 85, -1, -1, 49, 50, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 114, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 3,
  4, 5, 6, 7, 8, 9, -1, -1, -1, 12, -1, 114, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25, 26, 27, 28, -1,
  -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1, 42, 43, -1, -1, -1, -1, -1, 49, 50, -1, -1, -1,
  -1, 54, 55, -1, -1, -1, -1, -1, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
  77, 78, 79, 80, 81, 82, 83, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101,
  -1, -1, -1, -1, 105, -1, 107, 108, -1, 110, 111, 112, 114, 114, 115, 116, 117, 118, 119, 120, 121,
  122, 123, 3, 4, 5, 6, 7, 8, 9, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, 21, 22, 23, -1, -1, -1, -1,
  21, 22, 23, -1, -1, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, 43, -1, -1, -1, -1, -1, 49, -1, 21, 22, 23,
  -1, -1, 49, 3, 4, 5, 6, 7, 8, 9, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, 21, 22, 23, -1, 49, -1, -1,
  21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 49, -1, -1,
  -1, -1, -1, -1, 49, -1, -1, -1, 114, 85, -1, -1, -1, -1, -1, 114, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, 102, 103, -1, -1, 106, -1, -1, 85, -1, -1, -1, -1, -1, -1, 85, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, 102, -1, -1, -1, 106, -1, -1, -1, 103, -1, -1, 106, 3, 4, 5, 6, 7, 8, 9, 32, 33, 34, 35,
  36, 37, 38, 39, 40, 41, 42, 21, 22, 23, -1, 25, 26, 27, 28, 51, -1, -1, 32, 33, 34, 35, 36, 37,
  38, 39, 40, -1, 42, 43, 44, -1, -1, -1, -1, 49, 50, -1, -1, 53, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, -1, -1, -1, -1, 85,
  -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25, 26, 27, 28, -1, -1, -1, 32, 33, 34, 35, 36,
  37, 38, 39, 40, -1, 42, 43, -1, -1, -1, -1, -1, 49, 50, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 3, 4, 5, 6, 7, 8, 9, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25, 26, 27, 28, -1, -1, -1, 32, 33, 34, 35,
  36, 37, 38, 39, 40, -1, 42, 43, -1, -1, -1, -1, -1, 49, 50, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 3, 4, 5, 6, 7, 8, 9, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25, 26, 27, 28, -1, -1, -1, 32, 33, 34,
  35, 36, 37, 38, 39, 40, -1, 42, -1, -1, -1, -1, -1, -1, 49, 50, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 3, 4, 5, 6, 7, 8, 9,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25, -1, 27, 28, -1, -1, -1, 32, 33,
  34, 35, 36, 37, 38, 39, 40, -1, 42, -1, -1, -1, -1, -1, -1, 49, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 3, 4, 5, 6, 7,
  8, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, 32,
  33, 34, 35, 36, 37, 38, 39, 40, -1, 42, -1, -1, -1, -1, -1, -1, 49, 50, -1, -1, -1, -1, -1, 32,
  33, 34, 35, 36, 37, 38, 39, 40, 41, 42, -1, -1, -1, -1, -1, -1, -1, -1, 51, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 78, -1, -1,
  -1, -1, -1, -1, -1, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, -1, -1,
  -1, -1, -1, -1, -1, -1, 111, -1, -1, -1, 115, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, -1, -1,
  -1, -1, -1, -1, -1, -1, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, 78, -1, -1, -1, -1, -1, -1, -1, -1, 87, 88, 89, 90, 91, 92,
  93, 94, 95, 96, 97, 98, 99, 100, 101, -1, -1, -1, -1, -1, -1, -1, -1, -1, 111, -1, -1, -1, 115 };

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] = { 0, 126, 127, 0, 128, 342, 343, 3, 4, 5, 6, 7, 8, 9, 21, 22,
  23, 25, 26, 27, 28, 32, 33, 34, 35, 36, 37, 38, 39, 40, 42, 43, 44, 49, 50, 51, 52, 53, 85, 87,
  88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 114, 129, 130, 131, 132, 133, 135,
  136, 137, 138, 139, 141, 144, 159, 160, 161, 163, 164, 174, 175, 184, 186, 187, 189, 208, 209,
  210, 211, 214, 215, 218, 223, 264, 294, 295, 296, 297, 299, 300, 301, 302, 304, 306, 307, 310,
  311, 312, 313, 314, 316, 317, 320, 321, 332, 333, 334, 354, 25, 26, 52, 12, 44, 3, 4, 5, 6, 7, 8,
  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
  35, 36, 37, 38, 39, 40, 42, 43, 44, 45, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
  62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
  86, 103, 105, 106, 107, 108, 110, 111, 112, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
  333, 334, 365, 366, 367, 397, 398, 399, 400, 401, 305, 322, 3, 4, 5, 6, 7, 8, 9, 39, 42, 139, 144,
  161, 164, 296, 297, 302, 304, 310, 316, 3, 4, 5, 6, 7, 8, 9, 103, 307, 3, 4, 5, 6, 7, 8, 9, 21,
  22, 23, 50, 52, 185, 294, 296, 297, 301, 302, 304, 308, 140, 352, 353, 308, 103, 352, 44, 53, 130,
  137, 138, 164, 170, 187, 189, 208, 264, 310, 316, 46, 102, 103, 237, 238, 237, 237, 237, 108, 144,
  310, 316, 102, 342, 44, 215, 242, 245, 295, 300, 302, 304, 146, 302, 304, 306, 307, 301, 295, 296,
  301, 342, 301, 110, 139, 144, 161, 164, 175, 215, 297, 311, 320, 342, 10, 11, 84, 202, 265, 273,
  275, 78, 111, 115, 270, 335, 336, 337, 338, 341, 318, 342, 342, 24, 355, 307, 103, 397, 393, 393,
  64, 116, 191, 383, 393, 394, 393, 10, 11, 108, 387, 294, 12, 308, 352, 308, 352, 295, 139, 161,
  179, 180, 183, 202, 273, 393, 105, 134, 294, 294, 102, 344, 345, 218, 222, 223, 297, 41, 51, 266,
  269, 270, 309, 311, 334, 103, 102, 106, 145, 146, 297, 301, 302, 304, 354, 266, 162, 102, 106,
  165, 294, 294, 352, 310, 202, 106, 247, 393, 216, 352, 298, 45, 48, 228, 229, 108, 301, 301, 301,
  303, 308, 352, 308, 352, 215, 242, 342, 319, 276, 219, 220, 222, 223, 224, 240, 284, 295, 297,
  267, 105, 257, 258, 260, 261, 202, 273, 282, 335, 346, 347, 346, 346, 336, 338, 308, 53, 356, 357,
  358, 359, 360, 103, 127, 394, 102, 110, 113, 395, 396, 397, 104, 193, 195, 196, 198, 200, 190,
  113, 102, 396, 109, 389, 390, 388, 342, 176, 178, 270, 145, 176, 294, 308, 308, 177, 284, 295,
  302, 304, 104, 296, 302, 103, 102, 105, 354, 12, 54, 55, 64, 105, 107, 108, 110, 112, 116, 364,
  365, 218, 222, 102, 267, 265, 342, 148, 143, 3, 102, 147, 342, 146, 302, 304, 297, 102, 3, 4, 5,
  6, 7, 8, 9, 167, 168, 306, 166, 165, 342, 294, 297, 248, 249, 294, 103, 104, 250, 251, 145, 302,
  348, 349, 387, 246, 377, 266, 145, 266, 294, 308, 314, 315, 340, 32, 33, 225, 226, 227, 344, 225,
  286, 287, 288, 344, 219, 224, 295, 102, 107, 259, 103, 391, 108, 109, 273, 354, 339, 185, 294,
  113, 358, 107, 300, 307, 362, 363, 127, 104, 102, 113, 107, 383, 25, 27, 164, 296, 302, 304, 310,
  327, 328, 331, 332, 26, 50, 203, 189, 342, 373, 373, 373, 102, 177, 183, 102, 165, 176, 176, 102,
  107, 108, 344, 102, 127, 188, 3, 112, 112, 64, 116, 109, 113, 29, 30, 31, 104, 149, 150, 29, 30,
  31, 37, 154, 155, 158, 294, 106, 342, 146, 104, 107, 344, 317, 102, 306, 107, 398, 400, 393, 109,
  84, 252, 254, 342, 301, 231, 354, 250, 24, 85, 105, 106, 107, 110, 112, 124, 333, 334, 365, 366,
  367, 371, 372, 379, 380, 381, 383, 384, 387, 391, 102, 102, 102, 165, 314, 78, 111, 230, 108, 112,
  289, 290, 106, 108, 344, 268, 64, 110, 116, 368, 369, 371, 381, 392, 262, 367, 274, 340, 106, 113,
  359, 301, 84, 361, 387, 104, 194, 192, 294, 294, 294, 320, 202, 271, 275, 270, 329, 271, 203, 64,
  105, 107, 109, 110, 116, 371, 374, 375, 109, 109, 102, 102, 178, 181, 104, 316, 113, 113, 342,
  106, 157, 158, 107, 37, 156, 202, 142, 342, 168, 105, 105, 171, 398, 249, 202, 202, 104, 217, 107,
  255, 3, 232, 243, 109, 386, 382, 385, 102, 228, 221, 291, 290, 13, 14, 15, 16, 285, 241, 269, 370,
  369, 378, 107, 109, 108, 277, 287, 300, 195, 342, 330, 283, 284, 197, 346, 308, 199, 271, 250,
  271, 41, 42, 53, 102, 131, 136, 138, 151, 152, 153, 159, 160, 174, 184, 187, 189, 212, 213, 214,
  242, 264, 294, 295, 297, 310, 316, 294, 342, 294, 154, 169, 394, 102, 239, 225, 84, 253, 316, 247,
  373, 377, 373, 348, 250, 292, 293, 250, 372, 102, 104, 375, 376, 263, 278, 308, 277, 105, 204,
  205, 271, 281, 335, 204, 201, 109, 102, 342, 138, 152, 153, 187, 189, 212, 264, 295, 310, 237,
  102, 222, 242, 297, 202, 202, 155, 202, 368, 47, 254, 271, 244, 113, 383, 113, 67, 234, 235, 109,
  113, 368, 109, 368, 250, 206, 109, 271, 204, 182, 136, 144, 172, 189, 213, 310, 316, 310, 344,
  222, 224, 400, 256, 105, 233, 113, 236, 231, 350, 351, 109, 207, 379, 272, 280, 352, 144, 172,
  310, 237, 144, 202, 102, 344, 103, 257, 19, 55, 56, 310, 321, 323, 324, 233, 354, 279, 379, 277,
  32, 34, 45, 48, 103, 106, 145, 173, 352, 144, 352, 102, 393, 321, 325, 270, 280, 400, 400, 393,
  394, 147, 145, 352, 145, 173, 104, 326, 308, 348, 104, 102, 173, 145, 147, 308, 394, 173, 102 };

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] = { 0, 125, 126, 127, 128, 127, 129, 129, 129, 129, 129, 129,
  129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 130, 130, 130, 130, 130, 130, 131, 131,
  132, 133, 134, 133, 133, 135, 136, 136, 137, 137, 137, 138, 138, 140, 139, 142, 141, 141, 143,
  141, 141, 144, 144, 144, 145, 145, 145, 146, 146, 147, 147, 148, 149, 148, 148, 150, 150, 150,
  151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 152, 152, 152, 152, 152,
  152, 153, 153, 153, 153, 154, 154, 155, 155, 155, 156, 156, 157, 157, 158, 158, 158, 159, 159,
  159, 160, 160, 162, 161, 163, 163, 164, 164, 164, 165, 166, 165, 167, 167, 168, 168, 169, 168,
  170, 171, 171, 172, 172, 172, 172, 173, 173, 174, 174, 175, 175, 175, 175, 175, 176, 177, 177,
  178, 179, 179, 181, 182, 180, 183, 184, 185, 185, 185, 185, 185, 185, 186, 188, 187, 190, 189,
  191, 192, 189, 193, 194, 193, 196, 197, 195, 198, 199, 195, 200, 201, 195, 202, 202, 203, 203,
  204, 204, 206, 205, 207, 207, 208, 208, 208, 208, 209, 210, 210, 210, 211, 211, 211, 212, 212,
  212, 213, 213, 213, 213, 214, 214, 214, 216, 217, 215, 218, 219, 221, 220, 222, 223, 224, 225,
  226, 226, 227, 227, 228, 228, 228, 229, 229, 230, 230, 230, 231, 231, 232, 233, 233, 233, 233,
  234, 234, 236, 235, 237, 237, 237, 238, 239, 239, 241, 240, 243, 244, 242, 246, 245, 247, 247,
  248, 248, 249, 249, 250, 251, 250, 252, 253, 252, 252, 252, 255, 256, 254, 257, 257, 259, 258,
  260, 258, 261, 258, 262, 263, 262, 264, 265, 266, 266, 267, 268, 267, 269, 270, 270, 271, 272,
  271, 273, 274, 273, 276, 275, 275, 277, 278, 279, 277, 277, 280, 280, 280, 280, 280, 280, 281,
  281, 282, 282, 283, 283, 284, 284, 285, 285, 285, 285, 286, 286, 288, 287, 289, 289, 291, 290,
  292, 293, 292, 294, 294, 295, 295, 295, 295, 295, 296, 296, 296, 297, 297, 297, 297, 297, 297,
  298, 297, 299, 300, 301, 303, 302, 305, 304, 306, 306, 306, 306, 306, 306, 306, 306, 306, 306,
  307, 307, 307, 307, 307, 307, 307, 308, 308, 309, 309, 309, 309, 310, 310, 311, 311, 311, 311,
  312, 312, 312, 312, 312, 313, 313, 313, 314, 314, 315, 315, 316, 318, 317, 319, 317, 320, 320,
  320, 321, 321, 322, 321, 321, 321, 323, 325, 324, 326, 324, 327, 329, 328, 330, 328, 331, 331,
  331, 331, 331, 331, 331, 332, 332, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 334, 334,
  334, 334, 334, 334, 334, 334, 334, 334, 334, 334, 334, 334, 334, 335, 335, 335, 335, 336, 337,
  339, 338, 340, 340, 341, 341, 343, 342, 345, 344, 347, 346, 349, 348, 351, 350, 353, 352, 354,
  354, 355, 356, 356, 357, 358, 358, 358, 358, 360, 359, 361, 361, 362, 362, 363, 363, 364, 364,
  364, 364, 364, 364, 364, 364, 364, 364, 364, 364, 365, 365, 365, 365, 365, 365, 365, 365, 365,
  365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365,
  365, 365, 365, 365, 365, 365, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366,
  366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 367, 367,
  367, 367, 367, 367, 367, 367, 367, 368, 368, 369, 369, 369, 370, 369, 369, 371, 371, 372, 372,
  372, 372, 372, 372, 372, 372, 372, 372, 373, 373, 374, 374, 374, 374, 375, 375, 375, 376, 376,
  377, 377, 378, 378, 379, 379, 380, 380, 380, 382, 381, 383, 383, 385, 384, 386, 384, 388, 387,
  389, 387, 390, 387, 392, 391, 393, 393, 394, 394, 395, 395, 396, 396, 397, 397, 397, 397, 397,
  397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 398, 399, 399, 400, 401, 401, 401 };

/* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const unsigned char yyr2[] = { 0, 2, 1, 0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 1, 2, 2, 2, 2, 2, 2, 5, 4, 5, 4, 0, 6, 6, 5, 1, 2, 4, 3, 5, 4, 5, 0, 5, 0, 7, 4, 0, 5, 2, 1, 1,
  1, 3, 4, 2, 1, 1, 0, 1, 0, 0, 4, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 2,
  2, 2, 3, 4, 3, 4, 1, 4, 2, 4, 4, 0, 1, 0, 1, 1, 1, 1, 5, 3, 6, 4, 5, 0, 5, 4, 3, 1, 2, 2, 0, 0, 3,
  1, 3, 0, 2, 0, 5, 6, 2, 1, 5, 6, 3, 4, 5, 3, 1, 2, 5, 5, 6, 5, 6, 2, 0, 3, 2, 1, 1, 0, 0, 8, 1, 3,
  1, 2, 2, 2, 3, 3, 4, 0, 8, 0, 5, 0, 0, 7, 1, 0, 4, 0, 0, 5, 0, 0, 5, 0, 0, 6, 0, 1, 1, 1, 0, 1, 0,
  3, 1, 2, 2, 2, 2, 2, 3, 4, 2, 3, 2, 3, 4, 2, 4, 5, 3, 1, 1, 2, 1, 2, 3, 0, 0, 7, 2, 2, 0, 6, 2, 1,
  2, 7, 0, 1, 1, 1, 0, 2, 1, 1, 1, 0, 1, 1, 0, 2, 1, 0, 2, 2, 2, 0, 1, 0, 3, 3, 1, 1, 6, 0, 6, 0, 6,
  0, 0, 8, 0, 5, 0, 2, 1, 3, 3, 3, 0, 0, 2, 1, 0, 4, 3, 1, 0, 0, 6, 0, 1, 0, 3, 0, 2, 0, 4, 1, 0, 4,
  4, 2, 0, 2, 0, 0, 4, 2, 0, 1, 3, 0, 6, 3, 0, 5, 0, 3, 1, 0, 0, 0, 7, 1, 0, 2, 2, 3, 3, 2, 1, 2, 1,
  2, 0, 1, 2, 4, 1, 1, 1, 1, 0, 1, 0, 2, 1, 2, 0, 5, 0, 0, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 0, 5, 1, 1, 1, 0, 5, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 3, 1, 1,
  1, 1, 2, 3, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 0, 3, 0, 4, 1, 3, 4, 1, 1, 0, 4, 2,
  2, 2, 0, 3, 0, 4, 2, 0, 3, 0, 4, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0, 4, 0, 1, 1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0,
  2, 0, 2, 0, 2, 4, 2, 1, 3, 0, 1, 2, 3, 0, 3, 0, 1, 1, 2, 1, 3, 2, 2, 3, 3, 1, 1, 1, 1, 2, 2, 2, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 0, 2, 0, 2, 1, 1, 1, 1, 1, 0, 4, 1, 1, 0, 4, 0, 5, 0, 4, 0, 4, 0, 4, 0, 4, 0, 2, 0,
  2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 4, 3, 1, 1, 1 };

/* YYDPREC[RULE-NUM] -- Dynamic precedence of rule #RULE-NUM (0 if none).  */
static const unsigned char yydprec[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
static const unsigned char yymerger[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* YYIMMEDIATE[RULE-NUM] -- True iff rule #RULE-NUM is not to be deferred, as
   in the case of predicates.  */
static const yybool yyimmediate[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const unsigned char yyconflp[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 239, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 241, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 237, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 231, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 235, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 11, 13, 15, 17, 19,
  21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67,
  69, 0, 71, 73, 75, 77, 79, 81, 0, 83, 85, 87, 89, 0, 0, 91, 93, 95, 97, 99, 101, 103, 105, 107,
  109, 111, 113, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137, 139, 141, 143, 145,
  147, 149, 151, 153, 155, 157, 159, 161, 0, 163, 0, 165, 167, 169, 171, 173, 175, 177, 179, 181,
  183, 185, 187, 189, 191, 193, 0, 195, 0, 0, 197, 199, 201, 0, 203, 205, 207, 0, 209, 211, 213,
  215, 217, 219, 221, 223, 225, 227, 229, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short yyconfl[] = { 0, 410, 0, 410, 0, 410, 0, 321, 0, 629, 0, 629, 0, 629, 0, 629, 0,
  629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0,
  629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0,
  629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0,
  629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0,
  629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0,
  629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0,
  629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0,
  629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0,
  629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 629, 0, 304, 0,
  304, 0, 304, 0, 314, 0, 410, 0, 410, 0 };

/* Error token number */
#define YYTERROR 1

YYSTYPE yylval;

int yynerrs;
int yychar;

static const int YYEOF = 0;
static const int YYEMPTY = -2;

typedef enum
{
  yyok,
  yyaccept,
  yyabort,
  yyerr
} YYRESULTTAG;

#define YYCHK(YYE)                                                                                 \
  do                                                                                               \
  {                                                                                                \
    YYRESULTTAG yychk_flag = YYE;                                                                  \
    if (yychk_flag != yyok)                                                                        \
      return yychk_flag;                                                                           \
  } while (0)

#if YYDEBUG

#ifndef YYFPRINTF
#define YYFPRINTF fprintf
#endif

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
#define YY_LOCATION_PRINT(File, Loc) ((void)0)
#endif

#define YYDPRINTF(Args)                                                                            \
  do                                                                                               \
  {                                                                                                \
    if (yydebug)                                                                                   \
      YYFPRINTF Args;                                                                              \
  } while (0)

/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void yy_symbol_value_print(FILE* yyo, int yytype, YYSTYPE const* const yyvaluep)
{
  FILE* yyoutput = yyo;
  YYUSE(yyoutput);
  if (!yyvaluep)
    return;
  YYUSE(yytype);
}

/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void yy_symbol_print(FILE* yyo, int yytype, YYSTYPE const* const yyvaluep)
{
  YYFPRINTF(yyo, "%s %s (", yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print(yyo, yytype, yyvaluep);
  YYFPRINTF(yyo, ")");
}

#define YY_SYMBOL_PRINT(Title, Type, Value, Location)                                              \
  do                                                                                               \
  {                                                                                                \
    if (yydebug)                                                                                   \
    {                                                                                              \
      YYFPRINTF(stderr, "%s ", Title);                                                             \
      yy_symbol_print(stderr, Type, Value);                                                        \
      YYFPRINTF(stderr, "\n");                                                                     \
    }                                                                                              \
  } while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;

struct yyGLRStack;
static void yypstack(struct yyGLRStack* yystackp, size_t yyk) YY_ATTRIBUTE_UNUSED;
static void yypdumpstack(struct yyGLRStack* yystackp) YY_ATTRIBUTE_UNUSED;

#else /* !YYDEBUG */

#define YYDPRINTF(Args)
#define YY_SYMBOL_PRINT(Title, Type, Value, Location)

#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
#define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYMAXDEPTH * sizeof (GLRStackItem)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Minimum number of free items on the stack allowed after an
   allocation.  This is to allow allocation and initialization
   to be completed by functions that call yyexpandGLRStack before the
   stack is expanded, thus insuring that all necessary pointers get
   properly redirected to new data.  */
#define YYHEADROOM 2

#ifndef YYSTACKEXPANDABLE
#define YYSTACKEXPANDABLE 1
#endif

#if YYSTACKEXPANDABLE
#define YY_RESERVE_GLRSTACK(Yystack)                                                               \
  do                                                                                               \
  {                                                                                                \
    if (Yystack->yyspaceLeft < YYHEADROOM)                                                         \
      yyexpandGLRStack(Yystack);                                                                   \
  } while (0)
#else
#define YY_RESERVE_GLRSTACK(Yystack)                                                               \
  do                                                                                               \
  {                                                                                                \
    if (Yystack->yyspaceLeft < YYHEADROOM)                                                         \
      yyMemoryExhausted(Yystack);                                                                  \
  } while (0)
#endif

#if YYERROR_VERBOSE

#ifndef yystpcpy
#if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#define yystpcpy stpcpy
#else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char* yystpcpy(char* yydest, const char* yysrc)
{
  char* yyd = yydest;
  const char* yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static size_t yytnamerr(char* yyres, const char* yystr)
{
  if (*yystr == '"')
  {
    size_t yyn = 0;
    char const* yyp = yystr;

    for (;;)
      switch (*++yyp)
      {
        case '\'':
        case ',':
          goto do_not_strip_quotes;

        case '\\':
          if (*++yyp != '\\')
            goto do_not_strip_quotes;
          /* Fall through.  */
        default:
          if (yyres)
            yyres[yyn] = *yyp;
          yyn++;
          break;

        case '"':
          if (yyres)
            yyres[yyn] = '\0';
          return yyn;
      }
  do_not_strip_quotes:;
  }

  if (!yyres)
    return strlen(yystr);

  return (size_t)(yystpcpy(yyres, yystr) - yyres);
}
#endif

#endif /* !YYERROR_VERBOSE */

/** State numbers, as in LALR(1) machine */
typedef int yyStateNum;

/** Rule numbers, as in LALR(1) machine */
typedef int yyRuleNum;

/** Grammar symbol */
typedef int yySymbol;

/** Item references, as in LALR(1) machine */
typedef short yyItemNum;

typedef struct yyGLRState yyGLRState;
typedef struct yyGLRStateSet yyGLRStateSet;
typedef struct yySemanticOption yySemanticOption;
typedef union yyGLRStackItem yyGLRStackItem;
typedef struct yyGLRStack yyGLRStack;

struct yyGLRState
{
  /** Type tag: always true.  */
  yybool yyisState;
  /** Type tag for yysemantics.  If true, yysval applies, otherwise
   *  yyfirstVal applies.  */
  yybool yyresolved;
  /** Number of corresponding LALR(1) machine state.  */
  yyStateNum yylrState;
  /** Preceding state in this stack */
  yyGLRState* yypred;
  /** Source position of the last token produced by my symbol */
  size_t yyposn;
  union {
    /** First in a chain of alternative reductions producing the
     *  non-terminal corresponding to this state, threaded through
     *  yynext.  */
    yySemanticOption* yyfirstVal;
    /** Semantic value for this state.  */
    YYSTYPE yysval;
  } yysemantics;
};

struct yyGLRStateSet
{
  yyGLRState** yystates;
  /** During nondeterministic operation, yylookaheadNeeds tracks which
   *  stacks have actually needed the current lookahead.  During deterministic
   *  operation, yylookaheadNeeds[0] is not maintained since it would merely
   *  duplicate yychar != YYEMPTY.  */
  yybool* yylookaheadNeeds;
  size_t yysize, yycapacity;
};

struct yySemanticOption
{
  /** Type tag: always false.  */
  yybool yyisState;
  /** Rule number for this reduction */
  yyRuleNum yyrule;
  /** The last RHS state in the list of states to be reduced.  */
  yyGLRState* yystate;
  /** The lookahead for this reduction.  */
  int yyrawchar;
  YYSTYPE yyval;
  /** Next sibling in chain of options.  To facilitate merging,
   *  options are chained in decreasing order by address.  */
  yySemanticOption* yynext;
};

/** Type of the items in the GLR stack.  The yyisState field
 *  indicates which item of the union is valid.  */
union yyGLRStackItem {
  yyGLRState yystate;
  yySemanticOption yyoption;
};

struct yyGLRStack
{
  int yyerrState;

  YYJMP_BUF yyexception_buffer;
  yyGLRStackItem* yyitems;
  yyGLRStackItem* yynextFree;
  size_t yyspaceLeft;
  yyGLRState* yysplitPoint;
  yyGLRState* yylastDeleted;
  yyGLRStateSet yytops;
};

#if YYSTACKEXPANDABLE
static void yyexpandGLRStack(yyGLRStack* yystackp);
#endif

_Noreturn static void yyFail(yyGLRStack* yystackp, const char* yymsg)
{
  if (yymsg != YY_NULLPTR)
    yyerror(yymsg);
  YYLONGJMP(yystackp->yyexception_buffer, 1);
}

_Noreturn static void yyMemoryExhausted(yyGLRStack* yystackp)
{
  YYLONGJMP(yystackp->yyexception_buffer, 2);
}

#if YYDEBUG || YYERROR_VERBOSE
/** A printable representation of TOKEN.  */
static const char* yytokenName(yySymbol yytoken)
{
  if (yytoken == YYEMPTY)
    return "";

  return yytname[yytoken];
}
#endif

/** Fill in YYVSP[YYLOW1 .. YYLOW0-1] from the chain of states starting
 *  at YYVSP[YYLOW0].yystate.yypred.  Leaves YYVSP[YYLOW1].yystate.yypred
 *  containing the pointer to the next state in the chain.  */
static void yyfillin(yyGLRStackItem*, int, int);
static void yyfillin(yyGLRStackItem* yyvsp, int yylow0, int yylow1)
{
  int i;
  yyGLRState* s = yyvsp[yylow0].yystate.yypred;
  for (i = yylow0 - 1; i >= yylow1; i -= 1)
  {
#if YYDEBUG
    yyvsp[i].yystate.yylrState = s->yylrState;
#endif
    yyvsp[i].yystate.yyresolved = s->yyresolved;
    if (s->yyresolved)
      yyvsp[i].yystate.yysemantics.yysval = s->yysemantics.yysval;
    else
      /* The effect of using yysval or yyloc (in an immediate rule) is
       * undefined.  */
      yyvsp[i].yystate.yysemantics.yyfirstVal = YY_NULLPTR;
    s = yyvsp[i].yystate.yypred = s->yypred;
  }
}

/* Do nothing if YYNORMAL or if *YYLOW <= YYLOW1.  Otherwise, fill in
 * YYVSP[YYLOW1 .. *YYLOW-1] as in yyfillin and set *YYLOW = YYLOW1.
 * For convenience, always return YYLOW1.  */
static int yyfill(yyGLRStackItem*, int*, int, yybool);
static int yyfill(yyGLRStackItem* yyvsp, int* yylow, int yylow1, yybool yynormal)
{
  if (!yynormal && yylow1 < *yylow)
  {
    yyfillin(yyvsp, *yylow, yylow1);
    *yylow = yylow1;
  }
  return yylow1;
}

/** Perform user action for rule number YYN, with RHS length YYRHSLEN,
 *  and top stack item YYVSP.  YYLVALP points to place to put semantic
 *  value ($$), and yylocp points to place for location information
 *  (@$).  Returns yyok for normal return, yyaccept for YYACCEPT,
 *  yyerr for YYERROR, yyabort for YYABORT.  */
static YYRESULTTAG yyuserAction(
  yyRuleNum yyn, int yyrhslen, yyGLRStackItem* yyvsp, yyGLRStack* yystackp, YYSTYPE* yyvalp)
{
  yybool yynormal = (yybool)(yystackp->yysplitPoint == YY_NULLPTR);
  int yylow;
  YYUSE(yyvalp);
  YYUSE(yyrhslen);
#undef yyerrok
#define yyerrok (yystackp->yyerrState = 0)
#undef YYACCEPT
#define YYACCEPT return yyaccept
#undef YYABORT
#define YYABORT return yyabort
#undef YYERROR
#define YYERROR return yyerrok, yyerr
#undef YYRECOVERING
#define YYRECOVERING() (yystackp->yyerrState != 0)
#undef yyclearin
#define yyclearin (yychar = YYEMPTY)
#undef YYFILL
#define YYFILL(N) yyfill(yyvsp, &yylow, (N), yynormal)
#undef YYBACKUP
#define YYBACKUP(Token, Value) return yyerror(YY_("syntax error: cannot back up")), yyerrok, yyerr

  yylow = 1;
  if (yyrhslen == 0)
    *yyvalp = yyval_default;
  else
    *yyvalp = yyvsp[YYFILL(1 - (int)yyrhslen)].yystate.yysemantics.yysval;
  switch (yyn)
  {
    case 4:

    {
      startSig();
      clearType();
      clearTypeId();
      clearTemplate();
      closeComment();
    }

    break;

    case 33:

    {
      pushNamespace((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 34:

    {
      popNamespace();
    }

    break;

    case 44:

    {
      pushType();
    }

    break;

    case 45:

    {
      const char* name = (currentClass ? currentClass->Name : NULL);
      popType();
      clearTypeId();
      if (name)
      {
        setTypeId(name);
        setTypeBase(guess_id_type(name));
      }
      end_class();
    }

    break;

    case 46:

    {
      start_class((((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.str),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-4)].yystate.yysemantics.yysval.integer));
      currentClass->IsFinal =
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.integer);
    }

    break;

    case 48:

    {
      start_class((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-3)].yystate.yysemantics.yysval.integer));
      currentClass->IsFinal =
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer);
    }

    break;

    case 49:

    {
      start_class(
        NULL, (((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 51:

    {
      start_class(
        NULL, (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 52:

    {
      ((*yyvalp).integer) = 0;
    }

    break;

    case 53:

    {
      ((*yyvalp).integer) = 1;
    }

    break;

    case 54:

    {
      ((*yyvalp).integer) = 2;
    }

    break;

    case 55:

    {
      ((*yyvalp).str) =
        vtkstrcat((((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str));
    }

    break;

    case 56:

    {
      ((*yyvalp).str) = vtkstrcat3(
        "::", (((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.str),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str));
    }

    break;

    case 60:

    {
      ((*yyvalp).integer) = 0;
    }

    break;

    case 61:

    {
      ((*yyvalp).integer) =
        (strcmp((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str),
           "final") == 0);
    }

    break;

    case 63:

    {
      startSig();
      clearType();
      clearTypeId();
      clearTemplate();
      closeComment();
    }

    break;

    case 66:

    {
      access_level = VTK_ACCESS_PUBLIC;
    }

    break;

    case 67:

    {
      access_level = VTK_ACCESS_PRIVATE;
    }

    break;

    case 68:

    {
      access_level = VTK_ACCESS_PROTECTED;
    }

    break;

    case 92:

    {
      output_friend_function();
    }

    break;

    case 95:

    {
      add_base_class(currentClass,
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str), access_level,
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 96:

    {
      add_base_class(currentClass,
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.integer),
        (VTK_PARSE_VIRTUAL |
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer)));
    }

    break;

    case 97:

    {
      add_base_class(currentClass,
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-3)].yystate.yysemantics.yysval.integer),
        ((((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.integer) |
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer)));
    }

    break;

    case 98:

    {
      ((*yyvalp).integer) = 0;
    }

    break;

    case 99:

    {
      ((*yyvalp).integer) = VTK_PARSE_VIRTUAL;
    }

    break;

    case 100:

    {
      ((*yyvalp).integer) = access_level;
    }

    break;

    case 102:

    {
      ((*yyvalp).integer) = VTK_ACCESS_PUBLIC;
    }

    break;

    case 103:

    {
      ((*yyvalp).integer) = VTK_ACCESS_PRIVATE;
    }

    break;

    case 104:

    {
      ((*yyvalp).integer) = VTK_ACCESS_PROTECTED;
    }

    break;

    case 110:

    {
      pushType();
    }

    break;

    case 111:

    {
      popType();
      clearTypeId();
      if ((((yyGLRStackItem const*)yyvsp)[YYFILL(-4)].yystate.yysemantics.yysval.str) != NULL)
      {
        setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(-4)].yystate.yysemantics.yysval.str));
        setTypeBase(guess_id_type(
          (((yyGLRStackItem const*)yyvsp)[YYFILL(-4)].yystate.yysemantics.yysval.str)));
      }
      end_enum();
    }

    break;

    case 112:

    {
      start_enum((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-3)].yystate.yysemantics.yysval.integer),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer),
        getTypeId());
      clearType();
      clearTypeId();
      ((*yyvalp).str) = (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str);
    }

    break;

    case 113:

    {
      start_enum(NULL,
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.integer),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer),
        getTypeId());
      clearType();
      clearTypeId();
      ((*yyvalp).str) = NULL;
    }

    break;

    case 114:

    {
      ((*yyvalp).integer) = 0;
    }

    break;

    case 115:

    {
      ((*yyvalp).integer) = 1;
    }

    break;

    case 116:

    {
      ((*yyvalp).integer) = 1;
    }

    break;

    case 117:

    {
      ((*yyvalp).integer) = 0;
    }

    break;

    case 118:

    {
      pushType();
    }

    break;

    case 119:

    {
      ((*yyvalp).integer) = getType();
      popType();
    }

    break;

    case 123:

    {
      closeComment();
      add_enum((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str), NULL);
      clearType();
    }

    break;

    case 124:

    {
      postSig("=");
      markSig();
      closeComment();
    }

    break;

    case 125:

    {
      chopSig();
      add_enum(
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-4)].yystate.yysemantics.yysval.str), copySig());
      clearType();
    }

    break;

    case 148:

    {
      pushFunction();
      postSig("(");
    }

    break;

    case 149:

    {
      postSig(")");
    }

    break;

    case 150:

    {
      ((*yyvalp).integer) = (VTK_PARSE_FUNCTION |
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-7)].yystate.yysemantics.yysval.integer));
      popFunction();
    }

    break;

    case 151:

    {
      ValueInfo* item = (ValueInfo*)malloc(sizeof(ValueInfo));
      vtkParse_InitValue(item);
      item->ItemType = VTK_TYPEDEF_INFO;
      item->Access = access_level;

      handle_complex_type(item, getAttributes(), getType(),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer), getSig());

      if (currentTemplate)
      {
        item->Template = currentTemplate;
        currentTemplate = NULL;
      }

      if (getVarName())
      {
        item->Name = getVarName();
        item->Comment = vtkstrdup(getComment());
      }

      if (item->Class == NULL)
      {
        vtkParse_FreeValue(item);
      }
      else if (currentClass)
      {
        vtkParse_AddTypedefToClass(currentClass, item);
      }
      else
      {
        vtkParse_AddTypedefToNamespace(currentNamespace, item);
      }
    }

    break;

    case 152:

    {
      add_using((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str), 0);
    }

    break;

    case 154:

    {
      ((*yyvalp).str) = (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str);
    }

    break;

    case 155:

    {
      ((*yyvalp).str) =
        vtkstrcat((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 156:

    {
      ((*yyvalp).str) =
        vtkstrcat((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 157:

    {
      ((*yyvalp).str) =
        vtkstrcat3((((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 158:

    {
      ((*yyvalp).str) =
        vtkstrcat3((((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 159:

    {
      add_using((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str), 1);
    }

    break;

    case 160:

    {
      markSig();
    }

    break;

    case 161:

    {
      ValueInfo* item = (ValueInfo*)malloc(sizeof(ValueInfo));
      vtkParse_InitValue(item);
      item->ItemType = VTK_TYPEDEF_INFO;
      item->Access = access_level;

      handle_complex_type(item, getAttributes(), getType(),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.integer), copySig());

      item->Name = (((yyGLRStackItem const*)yyvsp)[YYFILL(-6)].yystate.yysemantics.yysval.str);
      item->Comment = vtkstrdup(getComment());

      if (currentTemplate)
      {
        vtkParse_FreeValue(item);
      }
      else if (currentClass)
      {
        vtkParse_AddTypedefToClass(currentClass, item);
      }
      else
      {
        vtkParse_AddTypedefToNamespace(currentNamespace, item);
      }
    }

    break;

    case 162:

    {
      postSig("template<> ");
      clearTypeId();
    }

    break;

    case 164:

    {
      postSig("template<");
      pushType();
      clearType();
      clearTypeId();
      startTemplate();
    }

    break;

    case 165:

    {
      chopSig();
      if (getSig()[getSigLength() - 1] == '>')
      {
        postSig(" ");
      }
      postSig("> ");
      clearTypeId();
      popType();
    }

    break;

    case 168:

    {
      chopSig();
      postSig(", ");
      clearType();
      clearTypeId();
    }

    break;

    case 170:

    {
      markSig();
    }

    break;

    case 171:

    {
      add_template_parameter(getType(),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer), copySig());
    }

    break;

    case 173:

    {
      markSig();
    }

    break;

    case 174:

    {
      add_template_parameter(0,
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer), copySig());
    }

    break;

    case 176:

    {
      pushTemplate();
      markSig();
    }

    break;

    case 177:

    {
      int i;
      TemplateInfo* newTemplate = currentTemplate;
      popTemplate();
      add_template_parameter(0,
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer), copySig());
      i = currentTemplate->NumberOfParameters - 1;
      currentTemplate->Parameters[i]->Template = newTemplate;
    }

    break;

    case 179:

    {
      ((*yyvalp).integer) = 0;
    }

    break;

    case 180:

    {
      postSig("...");
      ((*yyvalp).integer) = VTK_PARSE_PACK;
    }

    break;

    case 181:

    {
      postSig("class ");
    }

    break;

    case 182:

    {
      postSig("typename ");
    }

    break;

    case 185:

    {
      postSig("=");
      markSig();
    }

    break;

    case 186:

    {
      int i = currentTemplate->NumberOfParameters - 1;
      ValueInfo* param = currentTemplate->Parameters[i];
      chopSig();
      param->Value = copySig();
    }

    break;

    case 189:

    {
      output_function();
    }

    break;

    case 190:

    {
      output_function();
    }

    break;

    case 191:

    {
      reject_function();
    }

    break;

    case 192:

    {
      reject_function();
    }

    break;

    case 200:

    {
      output_function();
    }

    break;

    case 210:

    {
      postSig("(");
      currentFunction->IsExplicit = ((getType() & VTK_PARSE_EXPLICIT) != 0);
      set_return(currentFunction, getAttributes(), getType(), getTypeId(), 0);
    }

    break;

    case 211:

    {
      postSig(")");
    }

    break;

    case 212:

    {
      postSig(";");
      closeSig();
      currentFunction->IsOperator = 1;
      currentFunction->Name = "operator typecast";
      currentFunction->Comment = vtkstrdup(getComment());
      vtkParseDebug("Parsed operator", "operator typecast");
    }

    break;

    case 213:

    {
      ((*yyvalp).str) = copySig();
    }

    break;

    case 214:

    {
      postSig(";");
      closeSig();
      currentFunction->Name =
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str);
      currentFunction->Comment = vtkstrdup(getComment());
      vtkParseDebug("Parsed operator", currentFunction->Name);
    }

    break;

    case 215:

    {
      postSig("(");
      currentFunction->IsOperator = 1;
      set_return(currentFunction, getAttributes(), getType(), getTypeId(), 0);
    }

    break;

    case 216:

    {
      postSig(")");
    }

    break;

    case 217:

    {
      chopSig();
      ((*yyvalp).str) = vtkstrcat(
        copySig(), (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 218:

    {
      markSig();
      postSig("operator ");
    }

    break;

    case 219:

    {
      postSig(";");
      closeSig();
      currentFunction->Name =
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str);
      currentFunction->Comment = vtkstrdup(getComment());
      vtkParseDebug("Parsed func", currentFunction->Name);
    }

    break;

    case 223:

    {
      postSig(" const");
      currentFunction->IsConst = 1;
    }

    break;

    case 224:

    {
      postSig(" volatile");
    }

    break;

    case 226:

    {
      chopSig();
    }

    break;

    case 228:

    {
      postSig(" noexcept");
    }

    break;

    case 229:

    {
      postSig(" throw");
    }

    break;

    case 231:

    {
      postSig("&");
    }

    break;

    case 232:

    {
      postSig("&&");
    }

    break;

    case 235:

    {
      postSig(" ");
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      if (strcmp((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str),
            "final") == 0)
      {
        currentFunction->IsFinal = 1;
      }
      else if (strcmp((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str),
                 "override") == 0)
      {
        currentFunction->IsOverride = 1;
      }
    }

    break;

    case 237:

    {
      currentFunction->IsDeleted = 1;
    }

    break;

    case 239:

    {
      postSig(" = 0");
      currentFunction->IsPureVirtual = 1;
      if (currentClass)
      {
        currentClass->IsAbstract = 1;
      }
    }

    break;

    case 242:

    {
      postSig(" -> ");
      clearType();
      clearTypeId();
    }

    break;

    case 243:

    {
      chopSig();
      set_return(currentFunction, getAttributes(), getType(), getTypeId(), 0);
    }

    break;

    case 250:

    {
      postSig("(");
      set_return(currentFunction, getAttributes(), getType(), getTypeId(), 0);
    }

    break;

    case 251:

    {
      postSig(")");
    }

    break;

    case 252:

    {
      closeSig();
      if (getType() & VTK_PARSE_VIRTUAL)
      {
        currentFunction->IsVirtual = 1;
      }
      if (getType() & VTK_PARSE_EXPLICIT)
      {
        currentFunction->IsExplicit = 1;
      }
      if (getAttributes() & VTK_PARSE_WRAPEXCLUDE)
      {
        currentFunction->IsExcluded = 1;
      }
      if (getAttributes() & VTK_PARSE_DEPRECATED)
      {
        currentFunction->IsDeprecated = 1;
        currentFunction->DeprecatedReason = deprecationReason;
        currentFunction->DeprecatedVersion = deprecationVersion;
      }
      currentFunction->Name =
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-3)].yystate.yysemantics.yysval.str);
      currentFunction->Comment = vtkstrdup(getComment());
    }

    break;

    case 253:

    {
      openSig();
    }

    break;

    case 254:

    {
      postSig(";");
      closeSig();
      vtkParseDebug("Parsed func", currentFunction->Name);
    }

    break;

    case 255:

    {
      pushType();
      postSig("(");
    }

    break;

    case 256:

    {
      postSig(")");
      popType();
    }

    break;

    case 264:

    {
      clearType();
      clearTypeId();
    }

    break;

    case 266:

    {
      clearType();
      clearTypeId();
    }

    break;

    case 267:

    {
      clearType();
      clearTypeId();
      postSig(", ");
    }

    break;

    case 269:

    {
      currentFunction->IsVariadic = 1;
      postSig(", ...");
    }

    break;

    case 270:

    {
      currentFunction->IsVariadic = 1;
      postSig("...");
    }

    break;

    case 271:

    {
      markSig();
    }

    break;

    case 272:

    {
      ValueInfo* param = (ValueInfo*)malloc(sizeof(ValueInfo));
      vtkParse_InitValue(param);

      handle_complex_type(param, getAttributes(), getType(),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer), copySig());
      add_legacy_parameter(currentFunction, param);

      if (getVarName())
      {
        param->Name = getVarName();
      }

      vtkParse_AddParameterToFunction(currentFunction, param);
    }

    break;

    case 273:

    {
      int i = currentFunction->NumberOfParameters - 1;
      if (getVarValue())
      {
        currentFunction->Parameters[i]->Value = getVarValue();
      }
    }

    break;

    case 274:

    {
      clearVarValue();
    }

    break;

    case 276:

    {
      postSig("=");
      clearVarValue();
      markSig();
    }

    break;

    case 277:

    {
      chopSig();
      setVarValue(copySig());
    }

    break;

    case 278:

    {
      clearVarValue();
      markSig();
    }

    break;

    case 279:

    {
      chopSig();
      setVarValue(copySig());
    }

    break;

    case 280:

    {
      clearVarValue();
      markSig();
      postSig("(");
    }

    break;

    case 281:

    {
      chopSig();
      postSig(")");
      setVarValue(copySig());
    }

    break;

    case 282:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 283:

    {
      postSig(", ");
    }

    break;

    case 286:

    {
      unsigned int attributes = getAttributes();
      unsigned int type = getType();
      ValueInfo* var = (ValueInfo*)malloc(sizeof(ValueInfo));
      vtkParse_InitValue(var);
      var->ItemType = VTK_VARIABLE_INFO;
      var->Access = access_level;

      handle_complex_type(var, attributes, type,
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.integer), getSig());

      if (currentTemplate)
      {
        var->Template = currentTemplate;
        currentTemplate = NULL;
      }

      var->Name = getVarName();
      var->Comment = vtkstrdup(getComment());

      if (getVarValue())
      {
        var->Value = getVarValue();
      }

      /* Is this a typedef? */
      if ((type & VTK_PARSE_TYPEDEF) != 0)
      {
        var->ItemType = VTK_TYPEDEF_INFO;
        if (var->Class == NULL)
        {
          vtkParse_FreeValue(var);
        }
        else if (currentClass)
        {
          vtkParse_AddTypedefToClass(currentClass, var);
        }
        else
        {
          vtkParse_AddTypedefToNamespace(currentNamespace, var);
        }
      }
      /* Is this a constant? */
      else if (((type & VTK_PARSE_CONST) != 0) && var->Value != NULL &&
        (((type & VTK_PARSE_INDIRECT) == 0) || ((type & VTK_PARSE_INDIRECT) == VTK_PARSE_ARRAY)))
      {
        var->ItemType = VTK_CONSTANT_INFO;
        if (currentClass)
        {
          vtkParse_AddConstantToClass(currentClass, var);
        }
        else
        {
          vtkParse_AddConstantToNamespace(currentNamespace, var);
        }
      }
      /* This is a true variable i.e. not constant */
      else
      {
        if (currentClass)
        {
          vtkParse_AddVariableToClass(currentClass, var);
        }
        else
        {
          vtkParse_AddVariableToNamespace(currentNamespace, var);
        }
      }
    }

    break;

    case 290:

    {
      postSig(", ");
    }

    break;

    case 293:

    {
      setTypePtr(0);
    }

    break;

    case 294:

    {
      setTypePtr((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 295:

    {
      if ((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer) ==
        VTK_PARSE_FUNCTION)
      {
        ((*yyvalp).integer) = (VTK_PARSE_FUNCTION_PTR |
          (((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.integer));
      }
      else
      {
        ((*yyvalp).integer) =
          (((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.integer);
      }
    }

    break;

    case 296:

    {
      postSig(")");
    }

    break;

    case 297:

    {
      const char* scope = getScope();
      unsigned int parens = add_indirection(
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-5)].yystate.yysemantics.yysval.integer),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-3)].yystate.yysemantics.yysval.integer));
      if ((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer) ==
        VTK_PARSE_FUNCTION)
      {
        if (scope)
        {
          scope = vtkstrndup(scope, strlen(scope) - 2);
        }
        getFunction()->Class = scope;
        ((*yyvalp).integer) = (parens | VTK_PARSE_FUNCTION);
      }
      else if ((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer) ==
        VTK_PARSE_ARRAY)
      {
        ((*yyvalp).integer) = add_indirection_to_array(parens);
      }
    }

    break;

    case 298:

    {
      ((*yyvalp).integer) =
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.integer);
    }

    break;

    case 299:

    {
      postSig(")");
    }

    break;

    case 300:

    {
      const char* scope = getScope();
      unsigned int parens = add_indirection(
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-4)].yystate.yysemantics.yysval.integer),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-3)].yystate.yysemantics.yysval.integer));
      if ((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer) ==
        VTK_PARSE_FUNCTION)
      {
        if (scope)
        {
          scope = vtkstrndup(scope, strlen(scope) - 2);
        }
        getFunction()->Class = scope;
        ((*yyvalp).integer) = (parens | VTK_PARSE_FUNCTION);
      }
      else if ((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer) ==
        VTK_PARSE_ARRAY)
      {
        ((*yyvalp).integer) = add_indirection_to_array(parens);
      }
    }

    break;

    case 301:

    {
      postSig("(");
      scopeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      postSig("*");
    }

    break;

    case 302:

    {
      ((*yyvalp).integer) =
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer);
    }

    break;

    case 303:

    {
      postSig("(");
      scopeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      postSig("&");
      ((*yyvalp).integer) = VTK_PARSE_REF;
    }

    break;

    case 304:

    {
      ((*yyvalp).integer) = 0;
    }

    break;

    case 305:

    {
      pushFunction();
      postSig("(");
    }

    break;

    case 306:

    {
      postSig(")");
    }

    break;

    case 307:

    {
      ((*yyvalp).integer) = VTK_PARSE_FUNCTION;
      popFunction();
    }

    break;

    case 308:

    {
      ((*yyvalp).integer) = VTK_PARSE_ARRAY;
    }

    break;

    case 311:

    {
      currentFunction->IsConst = 1;
    }

    break;

    case 316:

    {
      ((*yyvalp).integer) = add_indirection(
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.integer),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 318:

    {
      ((*yyvalp).integer) = add_indirection(
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.integer),
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 319:

    {
      clearVarName();
      chopSig();
    }

    break;

    case 321:

    {
      setVarName((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str));
    }

    break;

    case 322:

    {
      setVarName((((yyGLRStackItem const*)yyvsp)[YYFILL(-3)].yystate.yysemantics.yysval.str));
    }

    break;

    case 327:

    {
      clearArray();
    }

    break;

    case 329:

    {
      clearArray();
    }

    break;

    case 333:

    {
      postSig("[");
    }

    break;

    case 334:

    {
      postSig("]");
    }

    break;

    case 335:

    {
      pushArraySize("");
    }

    break;

    case 336:

    {
      markSig();
    }

    break;

    case 337:

    {
      chopSig();
      pushArraySize(copySig());
    }

    break;

    case 343:

    {
      ((*yyvalp).str) =
        vtkstrcat("~", (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 344:

    {
      ((*yyvalp).str) =
        vtkstrcat("~", (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 345:

    {
      ((*yyvalp).str) =
        vtkstrcat((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 346:

    {
      ((*yyvalp).str) =
        vtkstrcat((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 347:

    {
      ((*yyvalp).str) =
        vtkstrcat((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 348:

    {
      ((*yyvalp).str) =
        vtkstrcat((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 349:

    {
      ((*yyvalp).str) =
        vtkstrcat((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 350:

    {
      ((*yyvalp).str) =
        vtkstrcat((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 351:

    {
      ((*yyvalp).str) =
        vtkstrcat3((((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 352:

    {
      ((*yyvalp).str) =
        vtkstrcat3((((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 353:

    {
      ((*yyvalp).str) =
        vtkstrcat3((((yyGLRStackItem const*)yyvsp)[YYFILL(-2)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 354:

    {
      postSig("template ");
    }

    break;

    case 355:

    {
      ((*yyvalp).str) =
        vtkstrcat4((((yyGLRStackItem const*)yyvsp)[YYFILL(-4)].yystate.yysemantics.yysval.str),
          "template ", (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str),
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 356:

    {
      postSig("~");
    }

    break;

    case 357:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 358:

    {
      ((*yyvalp).str) = "::";
      postSig(((*yyvalp).str));
    }

    break;

    case 359:

    {
      markSig();
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str));
      postSig("<");
    }

    break;

    case 360:

    {
      chopSig();
      if (getSig()[getSigLength() - 1] == '>')
      {
        postSig(" ");
      }
      postSig(">");
      ((*yyvalp).str) = copySig();
      clearTypeId();
    }

    break;

    case 361:

    {
      markSig();
      postSig("decltype");
    }

    break;

    case 362:

    {
      chopSig();
      ((*yyvalp).str) = copySig();
      clearTypeId();
    }

    break;

    case 363:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 364:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 365:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 366:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 367:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 368:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 369:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 370:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 371:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 372:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 383:

    {
      setTypeBase(buildTypeBase(
        getType(), (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer)));
    }

    break;

    case 384:

    {
      setTypeMod(VTK_PARSE_TYPEDEF);
    }

    break;

    case 385:

    {
      setTypeMod(VTK_PARSE_FRIEND);
    }

    break;

    case 388:

    {
      setTypeMod((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 389:

    {
      setTypeMod((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 390:

    {
      setTypeMod((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 391:

    {
      postSig("constexpr ");
      ((*yyvalp).integer) = 0;
    }

    break;

    case 392:

    {
      postSig("mutable ");
      ((*yyvalp).integer) = VTK_PARSE_MUTABLE;
    }

    break;

    case 393:

    {
      ((*yyvalp).integer) = 0;
    }

    break;

    case 394:

    {
      ((*yyvalp).integer) = 0;
    }

    break;

    case 395:

    {
      postSig("static ");
      ((*yyvalp).integer) = VTK_PARSE_STATIC;
    }

    break;

    case 396:

    {
      postSig("thread_local ");
      ((*yyvalp).integer) = VTK_PARSE_THREAD_LOCAL;
    }

    break;

    case 397:

    {
      ((*yyvalp).integer) = 0;
    }

    break;

    case 398:

    {
      postSig("virtual ");
      ((*yyvalp).integer) = VTK_PARSE_VIRTUAL;
    }

    break;

    case 399:

    {
      postSig("explicit ");
      ((*yyvalp).integer) = VTK_PARSE_EXPLICIT;
    }

    break;

    case 400:

    {
      postSig("const ");
      ((*yyvalp).integer) = VTK_PARSE_CONST;
    }

    break;

    case 401:

    {
      postSig("volatile ");
      ((*yyvalp).integer) = VTK_PARSE_VOLATILE;
    }

    break;

    case 403:

    {
      ((*yyvalp).integer) =
        ((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.integer) |
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 405:

    {
      setTypeBase((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 407:

    {
      setTypeBase((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 410:

    {
      postSig(" ");
      setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) =
        guess_id_type((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 411:

    {
      postSig(" ");
      setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) =
        guess_id_type((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str));
    }

    break;

    case 413:

    {
      postSig(" ");
      setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = 0;
    }

    break;

    case 414:

    {
      postSig("typename ");
    }

    break;

    case 415:

    {
      postSig(" ");
      setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) =
        guess_id_type((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str));
    }

    break;

    case 416:

    {
      postSig(" ");
      setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) =
        guess_id_type((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str));
    }

    break;

    case 417:

    {
      postSig(" ");
      setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) =
        guess_id_type((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str));
    }

    break;

    case 419:

    {
      setTypeBase((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 421:

    {
      setTypeBase((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 424:

    {
      setTypeBase((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 426:

    {
      setTypeBase((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 429:

    {
      postSig(" ");
      setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = 0;
    }

    break;

    case 430:

    {
      postSig(" ");
      setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) =
        guess_id_type((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 431:

    {
      postSig(" ");
      setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) =
        guess_id_type((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 432:

    {
      postSig(" ");
      setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) =
        guess_id_type((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 433:

    {
      postSig(" ");
      setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) =
        guess_id_type((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 434:

    {
      postSig(" ");
      setTypeId((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) =
        guess_id_type((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 435:

    {
      setTypeId("");
    }

    break;

    case 437:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_STRING;
    }

    break;

    case 438:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_UNICODE_STRING;
    }

    break;

    case 439:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_OSTREAM;
    }

    break;

    case 440:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_ISTREAM;
    }

    break;

    case 441:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_UNKNOWN;
    }

    break;

    case 442:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_OBJECT;
    }

    break;

    case 443:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_QOBJECT;
    }

    break;

    case 444:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_NULLPTR_T;
    }

    break;

    case 445:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_SSIZE_T;
    }

    break;

    case 446:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_SIZE_T;
    }

    break;

    case 447:

    {
      postSig("auto ");
      ((*yyvalp).integer) = 0;
    }

    break;

    case 448:

    {
      postSig("void ");
      ((*yyvalp).integer) = VTK_PARSE_VOID;
    }

    break;

    case 449:

    {
      postSig("bool ");
      ((*yyvalp).integer) = VTK_PARSE_BOOL;
    }

    break;

    case 450:

    {
      postSig("float ");
      ((*yyvalp).integer) = VTK_PARSE_FLOAT;
    }

    break;

    case 451:

    {
      postSig("double ");
      ((*yyvalp).integer) = VTK_PARSE_DOUBLE;
    }

    break;

    case 452:

    {
      postSig("char ");
      ((*yyvalp).integer) = VTK_PARSE_CHAR;
    }

    break;

    case 453:

    {
      postSig("char16_t ");
      ((*yyvalp).integer) = VTK_PARSE_CHAR16_T;
    }

    break;

    case 454:

    {
      postSig("char32_t ");
      ((*yyvalp).integer) = VTK_PARSE_CHAR32_T;
    }

    break;

    case 455:

    {
      postSig("wchar_t ");
      ((*yyvalp).integer) = VTK_PARSE_WCHAR_T;
    }

    break;

    case 456:

    {
      postSig("int ");
      ((*yyvalp).integer) = VTK_PARSE_INT;
    }

    break;

    case 457:

    {
      postSig("short ");
      ((*yyvalp).integer) = VTK_PARSE_SHORT;
    }

    break;

    case 458:

    {
      postSig("long ");
      ((*yyvalp).integer) = VTK_PARSE_LONG;
    }

    break;

    case 459:

    {
      postSig("__int64 ");
      ((*yyvalp).integer) = VTK_PARSE___INT64;
    }

    break;

    case 460:

    {
      postSig("signed ");
      ((*yyvalp).integer) = VTK_PARSE_INT;
    }

    break;

    case 461:

    {
      postSig("unsigned ");
      ((*yyvalp).integer) = VTK_PARSE_UNSIGNED_INT;
    }

    break;

    case 465:

    {
      ((*yyvalp).integer) =
        ((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.integer) |
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 466:

    {
      postSig("&");
      ((*yyvalp).integer) = VTK_PARSE_REF;
    }

    break;

    case 467:

    {
      postSig("&&");
      ((*yyvalp).integer) = (VTK_PARSE_RVALUE | VTK_PARSE_REF);
    }

    break;

    case 468:

    {
      postSig("*");
    }

    break;

    case 469:

    {
      ((*yyvalp).integer) =
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer);
    }

    break;

    case 470:

    {
      ((*yyvalp).integer) = VTK_PARSE_POINTER;
    }

    break;

    case 471:

    {
      if (((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer) &
            VTK_PARSE_CONST) != 0)
      {
        ((*yyvalp).integer) = VTK_PARSE_CONST_POINTER;
      }
      if (((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer) &
            VTK_PARSE_VOLATILE) != 0)
      {
        ((*yyvalp).integer) = VTK_PARSE_BAD_INDIRECT;
      }
    }

    break;

    case 473:

    {
      unsigned int n;
      n = (((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.integer) << 2) |
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
      if ((n & VTK_PARSE_INDIRECT) != n)
      {
        n = VTK_PARSE_BAD_INDIRECT;
      }
      ((*yyvalp).integer) = n;
    }

    break;

    case 474:

    {
      setAttributeRole(VTK_PARSE_ATTRIB_DECL);
    }

    break;

    case 475:

    {
      clearAttributeRole();
    }

    break;

    case 476:

    {
      setAttributeRole(VTK_PARSE_ATTRIB_ID);
    }

    break;

    case 477:

    {
      clearAttributeRole();
    }

    break;

    case 478:

    {
      setAttributeRole(VTK_PARSE_ATTRIB_REF);
    }

    break;

    case 479:

    {
      clearAttributeRole();
    }

    break;

    case 480:

    {
      setAttributeRole(VTK_PARSE_ATTRIB_FUNC);
    }

    break;

    case 481:

    {
      clearAttributeRole();
    }

    break;

    case 482:

    {
      setAttributeRole(VTK_PARSE_ATTRIB_ARRAY);
    }

    break;

    case 483:

    {
      clearAttributeRole();
    }

    break;

    case 484:

    {
      setAttributeRole(VTK_PARSE_ATTRIB_CLASS);
    }

    break;

    case 485:

    {
      clearAttributeRole();
    }

    break;

    case 488:

    {
      setAttributePrefix(NULL);
    }

    break;

    case 491:

    {
      setAttributePrefix(vtkstrcat(
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str), "::"));
    }

    break;

    case 496:

    {
      markSig();
    }

    break;

    case 497:

    {
      handle_attribute(
        cutSig(), (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 498:

    {
      ((*yyvalp).integer) = 0;
    }

    break;

    case 499:

    {
      ((*yyvalp).integer) = VTK_PARSE_PACK;
    }

    break;

    case 504:

    {
      ((*yyvalp).str) = "()";
    }

    break;

    case 505:

    {
      ((*yyvalp).str) = "[]";
    }

    break;

    case 506:

    {
      ((*yyvalp).str) = " new[]";
    }

    break;

    case 507:

    {
      ((*yyvalp).str) = " delete[]";
    }

    break;

    case 508:

    {
      ((*yyvalp).str) = "<";
    }

    break;

    case 509:

    {
      ((*yyvalp).str) = ">";
    }

    break;

    case 510:

    {
      ((*yyvalp).str) = ",";
    }

    break;

    case 511:

    {
      ((*yyvalp).str) = "=";
    }

    break;

    case 512:

    {
      ((*yyvalp).str) = ">>";
    }

    break;

    case 513:

    {
      ((*yyvalp).str) = ">>";
    }

    break;

    case 514:

    {
      ((*yyvalp).str) = vtkstrcat(
        "\"\" ", (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 516:

    {
      ((*yyvalp).str) = "%";
    }

    break;

    case 517:

    {
      ((*yyvalp).str) = "*";
    }

    break;

    case 518:

    {
      ((*yyvalp).str) = "/";
    }

    break;

    case 519:

    {
      ((*yyvalp).str) = "-";
    }

    break;

    case 520:

    {
      ((*yyvalp).str) = "+";
    }

    break;

    case 521:

    {
      ((*yyvalp).str) = "!";
    }

    break;

    case 522:

    {
      ((*yyvalp).str) = "~";
    }

    break;

    case 523:

    {
      ((*yyvalp).str) = "&";
    }

    break;

    case 524:

    {
      ((*yyvalp).str) = "|";
    }

    break;

    case 525:

    {
      ((*yyvalp).str) = "^";
    }

    break;

    case 526:

    {
      ((*yyvalp).str) = " new";
    }

    break;

    case 527:

    {
      ((*yyvalp).str) = " delete";
    }

    break;

    case 528:

    {
      ((*yyvalp).str) = "<<=";
    }

    break;

    case 529:

    {
      ((*yyvalp).str) = ">>=";
    }

    break;

    case 530:

    {
      ((*yyvalp).str) = "<<";
    }

    break;

    case 531:

    {
      ((*yyvalp).str) = ".*";
    }

    break;

    case 532:

    {
      ((*yyvalp).str) = "->*";
    }

    break;

    case 533:

    {
      ((*yyvalp).str) = "->";
    }

    break;

    case 534:

    {
      ((*yyvalp).str) = "+=";
    }

    break;

    case 535:

    {
      ((*yyvalp).str) = "-=";
    }

    break;

    case 536:

    {
      ((*yyvalp).str) = "*=";
    }

    break;

    case 537:

    {
      ((*yyvalp).str) = "/=";
    }

    break;

    case 538:

    {
      ((*yyvalp).str) = "%=";
    }

    break;

    case 539:

    {
      ((*yyvalp).str) = "++";
    }

    break;

    case 540:

    {
      ((*yyvalp).str) = "--";
    }

    break;

    case 541:

    {
      ((*yyvalp).str) = "&=";
    }

    break;

    case 542:

    {
      ((*yyvalp).str) = "|=";
    }

    break;

    case 543:

    {
      ((*yyvalp).str) = "^=";
    }

    break;

    case 544:

    {
      ((*yyvalp).str) = "&&";
    }

    break;

    case 545:

    {
      ((*yyvalp).str) = "||";
    }

    break;

    case 546:

    {
      ((*yyvalp).str) = "==";
    }

    break;

    case 547:

    {
      ((*yyvalp).str) = "!=";
    }

    break;

    case 548:

    {
      ((*yyvalp).str) = "<=";
    }

    break;

    case 549:

    {
      ((*yyvalp).str) = ">=";
    }

    break;

    case 550:

    {
      ((*yyvalp).str) = "typedef";
    }

    break;

    case 551:

    {
      ((*yyvalp).str) = "typename";
    }

    break;

    case 552:

    {
      ((*yyvalp).str) = "class";
    }

    break;

    case 553:

    {
      ((*yyvalp).str) = "struct";
    }

    break;

    case 554:

    {
      ((*yyvalp).str) = "union";
    }

    break;

    case 555:

    {
      ((*yyvalp).str) = "template";
    }

    break;

    case 556:

    {
      ((*yyvalp).str) = "public";
    }

    break;

    case 557:

    {
      ((*yyvalp).str) = "protected";
    }

    break;

    case 558:

    {
      ((*yyvalp).str) = "private";
    }

    break;

    case 559:

    {
      ((*yyvalp).str) = "const";
    }

    break;

    case 560:

    {
      ((*yyvalp).str) = "volatile";
    }

    break;

    case 561:

    {
      ((*yyvalp).str) = "static";
    }

    break;

    case 562:

    {
      ((*yyvalp).str) = "thread_local";
    }

    break;

    case 563:

    {
      ((*yyvalp).str) = "constexpr";
    }

    break;

    case 564:

    {
      ((*yyvalp).str) = "inline";
    }

    break;

    case 565:

    {
      ((*yyvalp).str) = "virtual";
    }

    break;

    case 566:

    {
      ((*yyvalp).str) = "explicit";
    }

    break;

    case 567:

    {
      ((*yyvalp).str) = "decltype";
    }

    break;

    case 568:

    {
      ((*yyvalp).str) = "default";
    }

    break;

    case 569:

    {
      ((*yyvalp).str) = "extern";
    }

    break;

    case 570:

    {
      ((*yyvalp).str) = "using";
    }

    break;

    case 571:

    {
      ((*yyvalp).str) = "namespace";
    }

    break;

    case 572:

    {
      ((*yyvalp).str) = "operator";
    }

    break;

    case 573:

    {
      ((*yyvalp).str) = "enum";
    }

    break;

    case 574:

    {
      ((*yyvalp).str) = "throw";
    }

    break;

    case 575:

    {
      ((*yyvalp).str) = "noexcept";
    }

    break;

    case 576:

    {
      ((*yyvalp).str) = "const_cast";
    }

    break;

    case 577:

    {
      ((*yyvalp).str) = "dynamic_cast";
    }

    break;

    case 578:

    {
      ((*yyvalp).str) = "static_cast";
    }

    break;

    case 579:

    {
      ((*yyvalp).str) = "reinterpret_cast";
    }

    break;

    case 593:

    {
      postSig("< ");
    }

    break;

    case 594:

    {
      postSig("> ");
    }

    break;

    case 596:

    {
      postSig(">");
    }

    break;

    case 598:

    {
      chopSig();
      postSig("::");
    }

    break;

    case 602:

    {
      const char* op = (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str);
      if ((op[0] == '+' || op[0] == '-' || op[0] == '*' || op[0] == '&') && op[1] == '\0')
      {
        int c1 = 0;
        size_t l;
        const char* cp;
        chopSig();
        cp = getSig();
        l = getSigLength();
        if (l > 0)
        {
          c1 = cp[l - 1];
        }
        if (c1 != 0 && c1 != '(' && c1 != '[' && c1 != '=')
        {
          postSig(" ");
        }
        postSig(op);
        if (vtkParse_CharType(c1, (CPRE_XID | CPRE_QUOTE)) || c1 == ')' || c1 == ']')
        {
          postSig(" ");
        }
      }
      else if ((op[0] == '-' && op[1] == '>') || op[0] == '.')
      {
        chopSig();
        postSig(op);
      }
      else
      {
        postSig(op);
        postSig(" ");
      }
    }

    break;

    case 603:

    {
      postSig(":");
      postSig(" ");
    }

    break;

    case 604:

    {
      postSig(".");
    }

    break;

    case 605:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      postSig(" ");
    }

    break;

    case 606:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      postSig(" ");
    }

    break;

    case 608:

    {
      chopSig();
      postSig(" ");
    }

    break;

    case 612:

    {
      postSig("< ");
    }

    break;

    case 613:

    {
      postSig("> ");
    }

    break;

    case 614:

    {
      postSig(">");
    }

    break;

    case 616:

    {
      postSig("= ");
    }

    break;

    case 617:

    {
      chopSig();
      postSig(", ");
    }

    break;

    case 619:

    {
      chopSig();
      postSig(";");
    }

    break;

    case 627:

    {
      postSig("= ");
    }

    break;

    case 628:

    {
      chopSig();
      postSig(", ");
    }

    break;

    case 629:

    {
      chopSig();
      if (getSig()[getSigLength() - 1] == '<')
      {
        postSig(" ");
      }
      postSig("<");
    }

    break;

    case 630:

    {
      chopSig();
      if (getSig()[getSigLength() - 1] == '>')
      {
        postSig(" ");
      }
      postSig("> ");
    }

    break;

    case 633:

    {
      postSigLeftBracket("[");
    }

    break;

    case 634:

    {
      postSigRightBracket("] ");
    }

    break;

    case 635:

    {
      postSig("[[");
    }

    break;

    case 636:

    {
      chopSig();
      postSig("]] ");
    }

    break;

    case 637:

    {
      postSigLeftBracket("(");
    }

    break;

    case 638:

    {
      postSigRightBracket(") ");
    }

    break;

    case 639:

    {
      postSigLeftBracket("(");
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      postSig("*");
    }

    break;

    case 640:

    {
      postSigRightBracket(") ");
    }

    break;

    case 641:

    {
      postSigLeftBracket("(");
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      postSig("&");
    }

    break;

    case 642:

    {
      postSigRightBracket(") ");
    }

    break;

    case 643:

    {
      postSig("{ ");
    }

    break;

    case 644:

    {
      postSig("} ");
    }

    break;

    default:
      break;
  }

  return yyok;
#undef yyerrok
#undef YYABORT
#undef YYACCEPT
#undef YYERROR
#undef YYBACKUP
#undef yyclearin
#undef YYRECOVERING
}

static void yyuserMerge(int yyn, YYSTYPE* yy0, YYSTYPE* yy1)
{
  YYUSE(yy0);
  YYUSE(yy1);

  switch (yyn)
  {

    default:
      break;
  }
}

/* Bison grammar-table manipulation.  */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void yydestruct(const char* yymsg, int yytype, YYSTYPE* yyvaluep)
{
  YYUSE(yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT(yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE(yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}

/** Number of symbols composing the right hand side of rule #RULE.  */
static int yyrhsLength(yyRuleNum yyrule)
{
  return yyr2[yyrule];
}

static void yydestroyGLRState(char const* yymsg, yyGLRState* yys)
{
  if (yys->yyresolved)
    yydestruct(yymsg, yystos[yys->yylrState], &yys->yysemantics.yysval);
  else
  {
#if YYDEBUG
    if (yydebug)
    {
      if (yys->yysemantics.yyfirstVal)
        YYFPRINTF(stderr, "%s unresolved", yymsg);
      else
        YYFPRINTF(stderr, "%s incomplete", yymsg);
      YY_SYMBOL_PRINT("", yystos[yys->yylrState], YY_NULLPTR, &yys->yyloc);
    }
#endif

    if (yys->yysemantics.yyfirstVal)
    {
      yySemanticOption* yyoption = yys->yysemantics.yyfirstVal;
      yyGLRState* yyrh;
      int yyn;
      for (yyrh = yyoption->yystate, yyn = yyrhsLength(yyoption->yyrule); yyn > 0;
           yyrh = yyrh->yypred, yyn -= 1)
        yydestroyGLRState(yymsg, yyrh);
    }
  }
}

/** Left-hand-side symbol for rule #YYRULE.  */
static yySymbol yylhsNonterm(yyRuleNum yyrule)
{
  return yyr1[yyrule];
}

#define yypact_value_is_default(Yystate) (!!((Yystate) == (-854)))

/** True iff LR state YYSTATE has only a default reduction (regardless
 *  of token).  */
static yybool yyisDefaultedState(yyStateNum yystate)
{
  return (yybool)yypact_value_is_default(yypact[yystate]);
}

/** The default reduction for YYSTATE, assuming it has one.  */
static yyRuleNum yydefaultAction(yyStateNum yystate)
{
  return yydefact[yystate];
}

#define yytable_value_is_error(Yytable_value) 0

/** Set *YYACTION to the action to take in YYSTATE on seeing YYTOKEN.
 *  Result R means
 *    R < 0:  Reduce on rule -R.
 *    R = 0:  Error.
 *    R > 0:  Shift to state R.
 *  Set *YYCONFLICTS to a pointer into yyconfl to a 0-terminated list
 *  of conflicting reductions.
 */
static void yygetLRActions(
  yyStateNum yystate, int yytoken, int* yyaction, const short** yyconflicts)
{
  int yyindex = yypact[yystate] + yytoken;
  if (yyisDefaultedState(yystate) || yyindex < 0 || YYLAST < yyindex || yycheck[yyindex] != yytoken)
  {
    *yyaction = -yydefact[yystate];
    *yyconflicts = yyconfl;
  }
  else if (!yytable_value_is_error(yytable[yyindex]))
  {
    *yyaction = yytable[yyindex];
    *yyconflicts = yyconfl + yyconflp[yyindex];
  }
  else
  {
    *yyaction = 0;
    *yyconflicts = yyconfl + yyconflp[yyindex];
  }
}

/** Compute post-reduction state.
 * \param yystate   the current state
 * \param yysym     the nonterminal to push on the stack
 */
static yyStateNum yyLRgotoState(yyStateNum yystate, yySymbol yysym)
{
  int yyr = yypgoto[yysym - YYNTOKENS] + yystate;
  if (0 <= yyr && yyr <= YYLAST && yycheck[yyr] == yystate)
    return yytable[yyr];
  else
    return yydefgoto[yysym - YYNTOKENS];
}

static yybool yyisShiftAction(int yyaction)
{
  return (yybool)(0 < yyaction);
}

static yybool yyisErrorAction(int yyaction)
{
  return (yybool)(yyaction == 0);
}

/* GLRStates */

/** Return a fresh GLRStackItem in YYSTACKP.  The item is an LR state
 *  if YYISSTATE, and otherwise a semantic option.  Callers should call
 *  YY_RESERVE_GLRSTACK afterwards to make sure there is sufficient
 *  headroom.  */

static yyGLRStackItem* yynewGLRStackItem(yyGLRStack* yystackp, yybool yyisState)
{
  yyGLRStackItem* yynewItem = yystackp->yynextFree;
  yystackp->yyspaceLeft -= 1;
  yystackp->yynextFree += 1;
  yynewItem->yystate.yyisState = yyisState;
  return yynewItem;
}

/** Add a new semantic action that will execute the action for rule
 *  YYRULE on the semantic values in YYRHS to the list of
 *  alternative actions for YYSTATE.  Assumes that YYRHS comes from
 *  stack #YYK of *YYSTACKP. */
static void yyaddDeferredAction(
  yyGLRStack* yystackp, size_t yyk, yyGLRState* yystate, yyGLRState* yyrhs, yyRuleNum yyrule)
{
  yySemanticOption* yynewOption = &yynewGLRStackItem(yystackp, yyfalse)->yyoption;
  YYASSERT(!yynewOption->yyisState);
  yynewOption->yystate = yyrhs;
  yynewOption->yyrule = yyrule;
  if (yystackp->yytops.yylookaheadNeeds[yyk])
  {
    yynewOption->yyrawchar = yychar;
    yynewOption->yyval = yylval;
  }
  else
    yynewOption->yyrawchar = YYEMPTY;
  yynewOption->yynext = yystate->yysemantics.yyfirstVal;
  yystate->yysemantics.yyfirstVal = yynewOption;

  YY_RESERVE_GLRSTACK(yystackp);
}

/* GLRStacks */

/** Initialize YYSET to a singleton set containing an empty stack.  */
static yybool yyinitStateSet(yyGLRStateSet* yyset)
{
  yyset->yysize = 1;
  yyset->yycapacity = 16;
  yyset->yystates = (yyGLRState**)YYMALLOC(16 * sizeof yyset->yystates[0]);
  if (!yyset->yystates)
    return yyfalse;
  yyset->yystates[0] = YY_NULLPTR;
  yyset->yylookaheadNeeds = (yybool*)YYMALLOC(16 * sizeof yyset->yylookaheadNeeds[0]);
  if (!yyset->yylookaheadNeeds)
  {
    YYFREE(yyset->yystates);
    return yyfalse;
  }
  return yytrue;
}

static void yyfreeStateSet(yyGLRStateSet* yyset)
{
  YYFREE(yyset->yystates);
  YYFREE(yyset->yylookaheadNeeds);
}

/** Initialize *YYSTACKP to a single empty stack, with total maximum
 *  capacity for all stacks of YYSIZE.  */
static yybool yyinitGLRStack(yyGLRStack* yystackp, size_t yysize)
{
  yystackp->yyerrState = 0;
  yynerrs = 0;
  yystackp->yyspaceLeft = yysize;
  yystackp->yyitems = (yyGLRStackItem*)YYMALLOC(yysize * sizeof yystackp->yynextFree[0]);
  if (!yystackp->yyitems)
    return yyfalse;
  yystackp->yynextFree = yystackp->yyitems;
  yystackp->yysplitPoint = YY_NULLPTR;
  yystackp->yylastDeleted = YY_NULLPTR;
  return yyinitStateSet(&yystackp->yytops);
}

#if YYSTACKEXPANDABLE
#define YYRELOC(YYFROMITEMS, YYTOITEMS, YYX, YYTYPE)                                               \
  &((YYTOITEMS) - ((YYFROMITEMS) - (yyGLRStackItem*)(YYX)))->YYTYPE

/** If *YYSTACKP is expandable, extend it.  WARNING: Pointers into the
    stack from outside should be considered invalid after this call.
    We always expand when there are 1 or fewer items left AFTER an
    allocation, so that we can avoid having external pointers exist
    across an allocation.  */
static void yyexpandGLRStack(yyGLRStack* yystackp)
{
  yyGLRStackItem* yynewItems;
  yyGLRStackItem *yyp0, *yyp1;
  size_t yynewSize;
  size_t yyn;
  size_t yysize = (size_t)(yystackp->yynextFree - yystackp->yyitems);
  if (YYMAXDEPTH - YYHEADROOM < yysize)
    yyMemoryExhausted(yystackp);
  yynewSize = 2 * yysize;
  if (YYMAXDEPTH < yynewSize)
    yynewSize = YYMAXDEPTH;
  yynewItems = (yyGLRStackItem*)YYMALLOC(yynewSize * sizeof yynewItems[0]);
  if (!yynewItems)
    yyMemoryExhausted(yystackp);
  for (yyp0 = yystackp->yyitems, yyp1 = yynewItems, yyn = yysize; 0 < yyn;
       yyn -= 1, yyp0 += 1, yyp1 += 1)
  {
    *yyp1 = *yyp0;
    if (*(yybool*)yyp0)
    {
      yyGLRState* yys0 = &yyp0->yystate;
      yyGLRState* yys1 = &yyp1->yystate;
      if (yys0->yypred != YY_NULLPTR)
        yys1->yypred = YYRELOC(yyp0, yyp1, yys0->yypred, yystate);
      if (!yys0->yyresolved && yys0->yysemantics.yyfirstVal != YY_NULLPTR)
        yys1->yysemantics.yyfirstVal = YYRELOC(yyp0, yyp1, yys0->yysemantics.yyfirstVal, yyoption);
    }
    else
    {
      yySemanticOption* yyv0 = &yyp0->yyoption;
      yySemanticOption* yyv1 = &yyp1->yyoption;
      if (yyv0->yystate != YY_NULLPTR)
        yyv1->yystate = YYRELOC(yyp0, yyp1, yyv0->yystate, yystate);
      if (yyv0->yynext != YY_NULLPTR)
        yyv1->yynext = YYRELOC(yyp0, yyp1, yyv0->yynext, yyoption);
    }
  }
  if (yystackp->yysplitPoint != YY_NULLPTR)
    yystackp->yysplitPoint =
      YYRELOC(yystackp->yyitems, yynewItems, yystackp->yysplitPoint, yystate);

  for (yyn = 0; yyn < yystackp->yytops.yysize; yyn += 1)
    if (yystackp->yytops.yystates[yyn] != YY_NULLPTR)
      yystackp->yytops.yystates[yyn] =
        YYRELOC(yystackp->yyitems, yynewItems, yystackp->yytops.yystates[yyn], yystate);
  YYFREE(yystackp->yyitems);
  yystackp->yyitems = yynewItems;
  yystackp->yynextFree = yynewItems + yysize;
  yystackp->yyspaceLeft = yynewSize - yysize;
}
#endif

static void yyfreeGLRStack(yyGLRStack* yystackp)
{
  YYFREE(yystackp->yyitems);
  yyfreeStateSet(&yystackp->yytops);
}

/** Assuming that YYS is a GLRState somewhere on *YYSTACKP, update the
 *  splitpoint of *YYSTACKP, if needed, so that it is at least as deep as
 *  YYS.  */
static void yyupdateSplit(yyGLRStack* yystackp, yyGLRState* yys)
{
  if (yystackp->yysplitPoint != YY_NULLPTR && yystackp->yysplitPoint > yys)
    yystackp->yysplitPoint = yys;
}

/** Invalidate stack #YYK in *YYSTACKP.  */
static void yymarkStackDeleted(yyGLRStack* yystackp, size_t yyk)
{
  if (yystackp->yytops.yystates[yyk] != YY_NULLPTR)
    yystackp->yylastDeleted = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yystates[yyk] = YY_NULLPTR;
}

/** Undelete the last stack in *YYSTACKP that was marked as deleted.  Can
    only be done once after a deletion, and only when all other stacks have
    been deleted.  */
static void yyundeleteLastStack(yyGLRStack* yystackp)
{
  if (yystackp->yylastDeleted == YY_NULLPTR || yystackp->yytops.yysize != 0)
    return;
  yystackp->yytops.yystates[0] = yystackp->yylastDeleted;
  yystackp->yytops.yysize = 1;
  YYDPRINTF((stderr, "Restoring last deleted stack as stack #0.\n"));
  yystackp->yylastDeleted = YY_NULLPTR;
}

static void yyremoveDeletes(yyGLRStack* yystackp)
{
  size_t yyi, yyj;
  yyi = yyj = 0;
  while (yyj < yystackp->yytops.yysize)
  {
    if (yystackp->yytops.yystates[yyi] == YY_NULLPTR)
    {
      if (yyi == yyj)
      {
        YYDPRINTF((stderr, "Removing dead stacks.\n"));
      }
      yystackp->yytops.yysize -= 1;
    }
    else
    {
      yystackp->yytops.yystates[yyj] = yystackp->yytops.yystates[yyi];
      /* In the current implementation, it's unnecessary to copy
         yystackp->yytops.yylookaheadNeeds[yyi] since, after
         yyremoveDeletes returns, the parser immediately either enters
         deterministic operation or shifts a token.  However, it doesn't
         hurt, and the code might evolve to need it.  */
      yystackp->yytops.yylookaheadNeeds[yyj] = yystackp->yytops.yylookaheadNeeds[yyi];
      if (yyj != yyi)
      {
        YYDPRINTF((stderr, "Rename stack %lu -> %lu.\n", (unsigned long)yyi, (unsigned long)yyj));
      }
      yyj += 1;
    }
    yyi += 1;
  }
}

/** Shift to a new state on stack #YYK of *YYSTACKP, corresponding to LR
 * state YYLRSTATE, at input position YYPOSN, with (resolved) semantic
 * value *YYVALP and source location *YYLOCP.  */
static void yyglrShift(
  yyGLRStack* yystackp, size_t yyk, yyStateNum yylrState, size_t yyposn, YYSTYPE* yyvalp)
{
  yyGLRState* yynewState = &yynewGLRStackItem(yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yytrue;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yysval = *yyvalp;
  yystackp->yytops.yystates[yyk] = yynewState;

  YY_RESERVE_GLRSTACK(yystackp);
}

/** Shift stack #YYK of *YYSTACKP, to a new state corresponding to LR
 *  state YYLRSTATE, at input position YYPOSN, with the (unresolved)
 *  semantic value of YYRHS under the action for YYRULE.  */
static void yyglrShiftDefer(yyGLRStack* yystackp, size_t yyk, yyStateNum yylrState, size_t yyposn,
  yyGLRState* yyrhs, yyRuleNum yyrule)
{
  yyGLRState* yynewState = &yynewGLRStackItem(yystackp, yytrue)->yystate;
  YYASSERT(yynewState->yyisState);

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yyfalse;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yyfirstVal = YY_NULLPTR;
  yystackp->yytops.yystates[yyk] = yynewState;

  /* Invokes YY_RESERVE_GLRSTACK.  */
  yyaddDeferredAction(yystackp, yyk, yynewState, yyrhs, yyrule);
}

#if !YYDEBUG
#define YY_REDUCE_PRINT(Args)
#else
#define YY_REDUCE_PRINT(Args)                                                                      \
  do                                                                                               \
  {                                                                                                \
    if (yydebug)                                                                                   \
      yy_reduce_print Args;                                                                        \
  } while (0)

/*----------------------------------------------------------------------.
| Report that stack #YYK of *YYSTACKP is going to be reduced by YYRULE. |
`----------------------------------------------------------------------*/

static void yy_reduce_print(yybool yynormal, yyGLRStackItem* yyvsp, size_t yyk, yyRuleNum yyrule)
{
  int yynrhs = yyrhsLength(yyrule);
  int yyi;
  YYFPRINTF(stderr, "Reducing stack %lu by rule %d (line %lu):\n", (unsigned long)yyk, yyrule - 1,
    (unsigned long)yyrline[yyrule]);
  if (!yynormal)
    yyfillin(yyvsp, 1, -yynrhs);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
  {
    YYFPRINTF(stderr, "   $%d = ", yyi + 1);
    yy_symbol_print(stderr, yystos[yyvsp[yyi - yynrhs + 1].yystate.yylrState],
      &yyvsp[yyi - yynrhs + 1].yystate.yysemantics.yysval);
    if (!yyvsp[yyi - yynrhs + 1].yystate.yyresolved)
      YYFPRINTF(stderr, " (unresolved)");
    YYFPRINTF(stderr, "\n");
  }
}
#endif

/** Pop the symbols consumed by reduction #YYRULE from the top of stack
 *  #YYK of *YYSTACKP, and perform the appropriate semantic action on their
 *  semantic values.  Assumes that all ambiguities in semantic values
 *  have been previously resolved.  Set *YYVALP to the resulting value,
 *  and *YYLOCP to the computed location (if any).  Return value is as
 *  for userAction.  */
static YYRESULTTAG yydoAction(yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule, YYSTYPE* yyvalp)
{
  int yynrhs = yyrhsLength(yyrule);

  if (yystackp->yysplitPoint == YY_NULLPTR)
  {
    /* Standard special case: single stack.  */
    yyGLRStackItem* yyrhs = (yyGLRStackItem*)yystackp->yytops.yystates[yyk];
    YYASSERT(yyk == 0);
    yystackp->yynextFree -= yynrhs;
    yystackp->yyspaceLeft += (size_t)yynrhs;
    yystackp->yytops.yystates[0] = &yystackp->yynextFree[-1].yystate;
    YY_REDUCE_PRINT((yytrue, yyrhs, yyk, yyrule));
    return yyuserAction(yyrule, yynrhs, yyrhs, yystackp, yyvalp);
  }
  else
  {
    int yyi;
    yyGLRState* yys;
    yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
    yys = yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred = yystackp->yytops.yystates[yyk];
    for (yyi = 0; yyi < yynrhs; yyi += 1)
    {
      yys = yys->yypred;
      YYASSERT(yys);
    }
    yyupdateSplit(yystackp, yys);
    yystackp->yytops.yystates[yyk] = yys;
    YY_REDUCE_PRINT((yyfalse, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1, yyk, yyrule));
    return yyuserAction(yyrule, yynrhs, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1, yystackp, yyvalp);
  }
}

/** Pop items off stack #YYK of *YYSTACKP according to grammar rule YYRULE,
 *  and push back on the resulting nonterminal symbol.  Perform the
 *  semantic action associated with YYRULE and store its value with the
 *  newly pushed state, if YYFORCEEVAL or if *YYSTACKP is currently
 *  unambiguous.  Otherwise, store the deferred semantic action with
 *  the new state.  If the new state would have an identical input
 *  position, LR state, and predecessor to an existing state on the stack,
 *  it is identified with that existing state, eliminating stack #YYK from
 *  *YYSTACKP.  In this case, the semantic value is
 *  added to the options for the existing state's semantic value.
 */
static YYRESULTTAG yyglrReduce(
  yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule, yybool yyforceEval)
{
  size_t yyposn = yystackp->yytops.yystates[yyk]->yyposn;

  if (yyforceEval || yystackp->yysplitPoint == YY_NULLPTR)
  {
    YYSTYPE yysval;

    YYRESULTTAG yyflag = yydoAction(yystackp, yyk, yyrule, &yysval);
    if (yyflag == yyerr && yystackp->yysplitPoint != YY_NULLPTR)
    {
      YYDPRINTF(
        (stderr, "Parse on stack %lu rejected by rule #%d.\n", (unsigned long)yyk, yyrule - 1));
    }
    if (yyflag != yyok)
      return yyflag;
    YY_SYMBOL_PRINT("-> $$ =", yyr1[yyrule], &yysval, &yyloc);
    yyglrShift(yystackp, yyk,
      yyLRgotoState(yystackp->yytops.yystates[yyk]->yylrState, yylhsNonterm(yyrule)), yyposn,
      &yysval);
  }
  else
  {
    size_t yyi;
    int yyn;
    yyGLRState *yys, *yys0 = yystackp->yytops.yystates[yyk];
    yyStateNum yynewLRState;

    for (yys = yystackp->yytops.yystates[yyk], yyn = yyrhsLength(yyrule); 0 < yyn; yyn -= 1)
    {
      yys = yys->yypred;
      YYASSERT(yys);
    }
    yyupdateSplit(yystackp, yys);
    yynewLRState = yyLRgotoState(yys->yylrState, yylhsNonterm(yyrule));
    YYDPRINTF((stderr,
      "Reduced stack %lu by rule #%d; action deferred.  "
      "Now in state %d.\n",
      (unsigned long)yyk, yyrule - 1, yynewLRState));
    for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
      if (yyi != yyk && yystackp->yytops.yystates[yyi] != YY_NULLPTR)
      {
        yyGLRState* yysplit = yystackp->yysplitPoint;
        yyGLRState* yyp = yystackp->yytops.yystates[yyi];
        while (yyp != yys && yyp != yysplit && yyp->yyposn >= yyposn)
        {
          if (yyp->yylrState == yynewLRState && yyp->yypred == yys)
          {
            yyaddDeferredAction(yystackp, yyk, yyp, yys0, yyrule);
            yymarkStackDeleted(yystackp, yyk);
            YYDPRINTF((stderr, "Merging stack %lu into stack %lu.\n", (unsigned long)yyk,
              (unsigned long)yyi));
            return yyok;
          }
          yyp = yyp->yypred;
        }
      }
    yystackp->yytops.yystates[yyk] = yys;
    yyglrShiftDefer(yystackp, yyk, yynewLRState, yyposn, yys0, yyrule);
  }
  return yyok;
}

static size_t yysplitStack(yyGLRStack* yystackp, size_t yyk)
{
  if (yystackp->yysplitPoint == YY_NULLPTR)
  {
    YYASSERT(yyk == 0);
    yystackp->yysplitPoint = yystackp->yytops.yystates[yyk];
  }
  if (yystackp->yytops.yysize >= yystackp->yytops.yycapacity)
  {
    yyGLRState** yynewStates = YY_NULLPTR;
    yybool* yynewLookaheadNeeds;

    if (yystackp->yytops.yycapacity > (YYSIZEMAX / (2 * sizeof(yyGLRState*))))
      yyMemoryExhausted(yystackp);
    yystackp->yytops.yycapacity *= 2;

    yynewStates = (yyGLRState**)YYREALLOC(
      yystackp->yytops.yystates, (yystackp->yytops.yycapacity * sizeof(yyGLRState*)));
    if (yynewStates == YY_NULLPTR)
      yyMemoryExhausted(yystackp);
    yystackp->yytops.yystates = yynewStates;

    yynewLookaheadNeeds = (yybool*)YYREALLOC(
      yystackp->yytops.yylookaheadNeeds, (yystackp->yytops.yycapacity * sizeof(yybool)));
    if (yynewLookaheadNeeds == YY_NULLPTR)
      yyMemoryExhausted(yystackp);
    yystackp->yytops.yylookaheadNeeds = yynewLookaheadNeeds;
  }
  yystackp->yytops.yystates[yystackp->yytops.yysize] = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yylookaheadNeeds[yystackp->yytops.yysize] =
    yystackp->yytops.yylookaheadNeeds[yyk];
  yystackp->yytops.yysize += 1;
  return yystackp->yytops.yysize - 1;
}

/** True iff YYY0 and YYY1 represent identical options at the top level.
 *  That is, they represent the same rule applied to RHS symbols
 *  that produce the same terminal symbols.  */
static yybool yyidenticalOptions(yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  if (yyy0->yyrule == yyy1->yyrule)
  {
    yyGLRState *yys0, *yys1;
    int yyn;
    for (yys0 = yyy0->yystate, yys1 = yyy1->yystate, yyn = yyrhsLength(yyy0->yyrule); yyn > 0;
         yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
      if (yys0->yyposn != yys1->yyposn)
        return yyfalse;
    return yytrue;
  }
  else
    return yyfalse;
}

/** Assuming identicalOptions (YYY0,YYY1), destructively merge the
 *  alternative semantic values for the RHS-symbols of YYY1 and YYY0.  */
static void yymergeOptionSets(yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  yyGLRState *yys0, *yys1;
  int yyn;
  for (yys0 = yyy0->yystate, yys1 = yyy1->yystate, yyn = yyrhsLength(yyy0->yyrule); yyn > 0;
       yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
  {
    if (yys0 == yys1)
      break;
    else if (yys0->yyresolved)
    {
      yys1->yyresolved = yytrue;
      yys1->yysemantics.yysval = yys0->yysemantics.yysval;
    }
    else if (yys1->yyresolved)
    {
      yys0->yyresolved = yytrue;
      yys0->yysemantics.yysval = yys1->yysemantics.yysval;
    }
    else
    {
      yySemanticOption** yyz0p = &yys0->yysemantics.yyfirstVal;
      yySemanticOption* yyz1 = yys1->yysemantics.yyfirstVal;
      while (yytrue)
      {
        if (yyz1 == *yyz0p || yyz1 == YY_NULLPTR)
          break;
        else if (*yyz0p == YY_NULLPTR)
        {
          *yyz0p = yyz1;
          break;
        }
        else if (*yyz0p < yyz1)
        {
          yySemanticOption* yyz = *yyz0p;
          *yyz0p = yyz1;
          yyz1 = yyz1->yynext;
          (*yyz0p)->yynext = yyz;
        }
        yyz0p = &(*yyz0p)->yynext;
      }
      yys1->yysemantics.yyfirstVal = yys0->yysemantics.yyfirstVal;
    }
  }
}

/** Y0 and Y1 represent two possible actions to take in a given
 *  parsing state; return 0 if no combination is possible,
 *  1 if user-mergeable, 2 if Y0 is preferred, 3 if Y1 is preferred.  */
static int yypreference(yySemanticOption* y0, yySemanticOption* y1)
{
  yyRuleNum r0 = y0->yyrule, r1 = y1->yyrule;
  int p0 = yydprec[r0], p1 = yydprec[r1];

  if (p0 == p1)
  {
    if (yymerger[r0] == 0 || yymerger[r0] != yymerger[r1])
      return 0;
    else
      return 1;
  }
  if (p0 == 0 || p1 == 0)
    return 0;
  if (p0 < p1)
    return 3;
  if (p1 < p0)
    return 2;
  return 0;
}

static YYRESULTTAG yyresolveValue(yyGLRState* yys, yyGLRStack* yystackp);

/** Resolve the previous YYN states starting at and including state YYS
 *  on *YYSTACKP. If result != yyok, some states may have been left
 *  unresolved possibly with empty semantic option chains.  Regardless
 *  of whether result = yyok, each state has been left with consistent
 *  data so that yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG yyresolveStates(yyGLRState* yys, int yyn, yyGLRStack* yystackp)
{
  if (0 < yyn)
  {
    YYASSERT(yys->yypred);
    YYCHK(yyresolveStates(yys->yypred, yyn - 1, yystackp));
    if (!yys->yyresolved)
      YYCHK(yyresolveValue(yys, yystackp));
  }
  return yyok;
}

/** Resolve the states for the RHS of YYOPT on *YYSTACKP, perform its
 *  user action, and return the semantic value and location in *YYVALP
 *  and *YYLOCP.  Regardless of whether result = yyok, all RHS states
 *  have been destroyed (assuming the user action destroys all RHS
 *  semantic values if invoked).  */
static YYRESULTTAG yyresolveAction(yySemanticOption* yyopt, yyGLRStack* yystackp, YYSTYPE* yyvalp)
{
  yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
  int yynrhs = yyrhsLength(yyopt->yyrule);
  YYRESULTTAG yyflag = yyresolveStates(yyopt->yystate, yynrhs, yystackp);
  if (yyflag != yyok)
  {
    yyGLRState* yys;
    for (yys = yyopt->yystate; yynrhs > 0; yys = yys->yypred, yynrhs -= 1)
      yydestroyGLRState("Cleanup: popping", yys);
    return yyflag;
  }

  yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred = yyopt->yystate;
  {
    int yychar_current = yychar;
    YYSTYPE yylval_current = yylval;
    yychar = yyopt->yyrawchar;
    yylval = yyopt->yyval;
    yyflag =
      yyuserAction(yyopt->yyrule, yynrhs, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1, yystackp, yyvalp);
    yychar = yychar_current;
    yylval = yylval_current;
  }
  return yyflag;
}

#if YYDEBUG
static void yyreportTree(yySemanticOption* yyx, int yyindent)
{
  int yynrhs = yyrhsLength(yyx->yyrule);
  int yyi;
  yyGLRState* yys;
  yyGLRState* yystates[1 + YYMAXRHS];
  yyGLRState yyleftmost_state;

  for (yyi = yynrhs, yys = yyx->yystate; 0 < yyi; yyi -= 1, yys = yys->yypred)
    yystates[yyi] = yys;
  if (yys == YY_NULLPTR)
  {
    yyleftmost_state.yyposn = 0;
    yystates[0] = &yyleftmost_state;
  }
  else
    yystates[0] = yys;

  if (yyx->yystate->yyposn < yys->yyposn + 1)
    YYFPRINTF(stderr, "%*s%s -> <Rule %d, empty>\n", yyindent, "",
      yytokenName(yylhsNonterm(yyx->yyrule)), yyx->yyrule - 1);
  else
    YYFPRINTF(stderr, "%*s%s -> <Rule %d, tokens %lu .. %lu>\n", yyindent, "",
      yytokenName(yylhsNonterm(yyx->yyrule)), yyx->yyrule - 1, (unsigned long)(yys->yyposn + 1),
      (unsigned long)yyx->yystate->yyposn);
  for (yyi = 1; yyi <= yynrhs; yyi += 1)
  {
    if (yystates[yyi]->yyresolved)
    {
      if (yystates[yyi - 1]->yyposn + 1 > yystates[yyi]->yyposn)
        YYFPRINTF(stderr, "%*s%s <empty>\n", yyindent + 2, "",
          yytokenName(yystos[yystates[yyi]->yylrState]));
      else
        YYFPRINTF(stderr, "%*s%s <tokens %lu .. %lu>\n", yyindent + 2, "",
          yytokenName(yystos[yystates[yyi]->yylrState]),
          (unsigned long)(yystates[yyi - 1]->yyposn + 1), (unsigned long)yystates[yyi]->yyposn);
    }
    else
      yyreportTree(yystates[yyi]->yysemantics.yyfirstVal, yyindent + 2);
  }
}
#endif

static YYRESULTTAG yyreportAmbiguity(yySemanticOption* yyx0, yySemanticOption* yyx1)
{
  YYUSE(yyx0);
  YYUSE(yyx1);

#if YYDEBUG
  YYFPRINTF(stderr, "Ambiguity detected.\n");
  YYFPRINTF(stderr, "Option 1,\n");
  yyreportTree(yyx0, 2);
  YYFPRINTF(stderr, "\nOption 2,\n");
  yyreportTree(yyx1, 2);
  YYFPRINTF(stderr, "\n");
#endif

  yyerror(YY_("syntax is ambiguous"));
  return yyabort;
}

/** Resolve the ambiguity represented in state YYS in *YYSTACKP,
 *  perform the indicated actions, and set the semantic value of YYS.
 *  If result != yyok, the chain of semantic options in YYS has been
 *  cleared instead or it has been left unmodified except that
 *  redundant options may have been removed.  Regardless of whether
 *  result = yyok, YYS has been left with consistent data so that
 *  yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG yyresolveValue(yyGLRState* yys, yyGLRStack* yystackp)
{
  yySemanticOption* yyoptionList = yys->yysemantics.yyfirstVal;
  yySemanticOption* yybest = yyoptionList;
  yySemanticOption** yypp;
  yybool yymerge = yyfalse;
  YYSTYPE yysval;
  YYRESULTTAG yyflag;

  for (yypp = &yyoptionList->yynext; *yypp != YY_NULLPTR;)
  {
    yySemanticOption* yyp = *yypp;

    if (yyidenticalOptions(yybest, yyp))
    {
      yymergeOptionSets(yybest, yyp);
      *yypp = yyp->yynext;
    }
    else
    {
      switch (yypreference(yybest, yyp))
      {
        case 0:
          return yyreportAmbiguity(yybest, yyp);
          /* break; */
        case 1:
          yymerge = yytrue;
          break;
        case 2:
          break;
        case 3:
          yybest = yyp;
          yymerge = yyfalse;
          break;
        default:
          /* This cannot happen so it is not worth a YYASSERT (yyfalse),
             but some compilers complain if the default case is
             omitted.  */
          break;
      }
      yypp = &yyp->yynext;
    }
  }

  if (yymerge)
  {
    yySemanticOption* yyp;
    int yyprec = yydprec[yybest->yyrule];
    yyflag = yyresolveAction(yybest, yystackp, &yysval);
    if (yyflag == yyok)
      for (yyp = yybest->yynext; yyp != YY_NULLPTR; yyp = yyp->yynext)
      {
        if (yyprec == yydprec[yyp->yyrule])
        {
          YYSTYPE yysval_other;
          yyflag = yyresolveAction(yyp, yystackp, &yysval_other);
          if (yyflag != yyok)
          {
            yydestruct(
              "Cleanup: discarding incompletely merged value for", yystos[yys->yylrState], &yysval);
            break;
          }
          yyuserMerge(yymerger[yyp->yyrule], &yysval, &yysval_other);
        }
      }
  }
  else
    yyflag = yyresolveAction(yybest, yystackp, &yysval);

  if (yyflag == yyok)
  {
    yys->yyresolved = yytrue;
    yys->yysemantics.yysval = yysval;
  }
  else
    yys->yysemantics.yyfirstVal = YY_NULLPTR;
  return yyflag;
}

static YYRESULTTAG yyresolveStack(yyGLRStack* yystackp)
{
  if (yystackp->yysplitPoint != YY_NULLPTR)
  {
    yyGLRState* yys;
    int yyn;

    for (yyn = 0, yys = yystackp->yytops.yystates[0]; yys != yystackp->yysplitPoint;
         yys = yys->yypred, yyn += 1)
      continue;
    YYCHK(yyresolveStates(yystackp->yytops.yystates[0], yyn, yystackp));
  }
  return yyok;
}

static void yycompressStack(yyGLRStack* yystackp)
{
  yyGLRState *yyp, *yyq, *yyr;

  if (yystackp->yytops.yysize != 1 || yystackp->yysplitPoint == YY_NULLPTR)
    return;

  for (yyp = yystackp->yytops.yystates[0], yyq = yyp->yypred, yyr = YY_NULLPTR;
       yyp != yystackp->yysplitPoint; yyr = yyp, yyp = yyq, yyq = yyp->yypred)
    yyp->yypred = yyr;

  yystackp->yyspaceLeft += (size_t)(yystackp->yynextFree - yystackp->yyitems);
  yystackp->yynextFree = ((yyGLRStackItem*)yystackp->yysplitPoint) + 1;
  yystackp->yyspaceLeft -= (size_t)(yystackp->yynextFree - yystackp->yyitems);
  yystackp->yysplitPoint = YY_NULLPTR;
  yystackp->yylastDeleted = YY_NULLPTR;

  while (yyr != YY_NULLPTR)
  {
    yystackp->yynextFree->yystate = *yyr;
    yyr = yyr->yypred;
    yystackp->yynextFree->yystate.yypred = &yystackp->yynextFree[-1].yystate;
    yystackp->yytops.yystates[0] = &yystackp->yynextFree->yystate;
    yystackp->yynextFree += 1;
    yystackp->yyspaceLeft -= 1;
  }
}

static YYRESULTTAG yyprocessOneStack(yyGLRStack* yystackp, size_t yyk, size_t yyposn)
{
  while (yystackp->yytops.yystates[yyk] != YY_NULLPTR)
  {
    yyStateNum yystate = yystackp->yytops.yystates[yyk]->yylrState;
    YYDPRINTF((stderr, "Stack %lu Entering state %d\n", (unsigned long)yyk, yystate));

    YYASSERT(yystate != YYFINAL);

    if (yyisDefaultedState(yystate))
    {
      YYRESULTTAG yyflag;
      yyRuleNum yyrule = yydefaultAction(yystate);
      if (yyrule == 0)
      {
        YYDPRINTF((stderr, "Stack %lu dies.\n", (unsigned long)yyk));
        yymarkStackDeleted(yystackp, yyk);
        return yyok;
      }
      yyflag = yyglrReduce(yystackp, yyk, yyrule, yyimmediate[yyrule]);
      if (yyflag == yyerr)
      {
        YYDPRINTF((stderr,
          "Stack %lu dies "
          "(predicate failure or explicit user error).\n",
          (unsigned long)yyk));
        yymarkStackDeleted(yystackp, yyk);
        return yyok;
      }
      if (yyflag != yyok)
        return yyflag;
    }
    else
    {
      yySymbol yytoken;
      int yyaction;
      const short* yyconflicts;

      yystackp->yytops.yylookaheadNeeds[yyk] = yytrue;
      if (yychar == YYEMPTY)
      {
        YYDPRINTF((stderr, "Reading a token: "));
        yychar = yylex();
      }

      if (yychar <= YYEOF)
      {
        yychar = yytoken = YYEOF;
        YYDPRINTF((stderr, "Now at end of input.\n"));
      }
      else
      {
        yytoken = YYTRANSLATE(yychar);
        YY_SYMBOL_PRINT("Next token is", yytoken, &yylval, &yylloc);
      }

      yygetLRActions(yystate, yytoken, &yyaction, &yyconflicts);

      while (*yyconflicts != 0)
      {
        YYRESULTTAG yyflag;
        size_t yynewStack = yysplitStack(yystackp, yyk);
        YYDPRINTF((stderr, "Splitting off stack %lu from %lu.\n", (unsigned long)yynewStack,
          (unsigned long)yyk));
        yyflag = yyglrReduce(yystackp, yynewStack, *yyconflicts, yyimmediate[*yyconflicts]);
        if (yyflag == yyok)
          YYCHK(yyprocessOneStack(yystackp, yynewStack, yyposn));
        else if (yyflag == yyerr)
        {
          YYDPRINTF((stderr, "Stack %lu dies.\n", (unsigned long)yynewStack));
          yymarkStackDeleted(yystackp, yynewStack);
        }
        else
          return yyflag;
        yyconflicts += 1;
      }

      if (yyisShiftAction(yyaction))
        break;
      else if (yyisErrorAction(yyaction))
      {
        YYDPRINTF((stderr, "Stack %lu dies.\n", (unsigned long)yyk));
        yymarkStackDeleted(yystackp, yyk);
        break;
      }
      else
      {
        YYRESULTTAG yyflag = yyglrReduce(yystackp, yyk, -yyaction, yyimmediate[-yyaction]);
        if (yyflag == yyerr)
        {
          YYDPRINTF((stderr,
            "Stack %lu dies "
            "(predicate failure or explicit user error).\n",
            (unsigned long)yyk));
          yymarkStackDeleted(yystackp, yyk);
          break;
        }
        else if (yyflag != yyok)
          return yyflag;
      }
    }
  }
  return yyok;
}

static void yyreportSyntaxError(yyGLRStack* yystackp)
{
  if (yystackp->yyerrState != 0)
    return;
#if !YYERROR_VERBOSE
  yyerror(YY_("syntax error"));
#else
  {
    yySymbol yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE(yychar);
    size_t yysize0 = yytnamerr(YY_NULLPTR, yytokenName(yytoken));
    size_t yysize = yysize0;
    yybool yysize_overflow = yyfalse;
    char* yymsg = YY_NULLPTR;
    enum
    {
      YYERROR_VERBOSE_ARGS_MAXIMUM = 5
    };
    /* Internationalized format string. */
    const char* yyformat = YY_NULLPTR;
    /* Arguments of yyformat. */
    char const* yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
    /* Number of reported tokens (one for the "unexpected", one per
       "expected").  */
    int yycount = 0;

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yychar) is if
         this state is a consistent state with a default action.  Thus,
         detecting the absence of a lookahead is sufficient to determine
         that there is no unexpected or expected token to report.  In that
         case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is a
         consistent state with a default action.  There might have been a
         previous inconsistent state, consistent state with a non-default
         action, or user semantic action that manipulated yychar.
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */
    if (yytoken != YYEMPTY)
    {
      int yyn = yypact[yystackp->yytops.yystates[0]->yylrState];
      yyarg[yycount++] = yytokenName(yytoken);
      if (!yypact_value_is_default(yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for this
           state because they are default actions.  */
        int yyxbegin = yyn < 0 ? -yyn : 0;
        /* Stay within bounds of both yycheck and yytname.  */
        int yychecklim = YYLAST - yyn + 1;
        int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        int yyx;
        for (yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR &&
            !yytable_value_is_error(yytable[yyx + yyn]))
          {
            if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
            {
              yycount = 1;
              yysize = yysize0;
              break;
            }
            yyarg[yycount++] = yytokenName(yyx);
            {
              size_t yysz = yysize + yytnamerr(YY_NULLPTR, yytokenName(yyx));
              if (yysz < yysize)
                yysize_overflow = yytrue;
              yysize = yysz;
            }
          }
      }
    }

    switch (yycount)
    {
#define YYCASE_(N, S)                                                                              \
  case N:                                                                                          \
    yyformat = S;                                                                                  \
    break
      default: /* Avoid compiler warnings. */
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

    {
      size_t yysz = yysize + strlen(yyformat);
      if (yysz < yysize)
        yysize_overflow = yytrue;
      yysize = yysz;
    }

    if (!yysize_overflow)
      yymsg = (char*)YYMALLOC(yysize);

    if (yymsg)
    {
      char* yyp = yymsg;
      int yyi = 0;
      while ((*yyp = *yyformat))
      {
        if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr(yyp, yyarg[yyi++]);
          yyformat += 2;
        }
        else
        {
          yyp++;
          yyformat++;
        }
      }
      yyerror(yymsg);
      YYFREE(yymsg);
    }
    else
    {
      yyerror(YY_("syntax error"));
      yyMemoryExhausted(yystackp);
    }
  }
#endif /* YYERROR_VERBOSE */
  yynerrs += 1;
}

/* Recover from a syntax error on *YYSTACKP, assuming that *YYSTACKP->YYTOKENP,
   yylval, and yylloc are the syntactic category, semantic value, and location
   of the lookahead.  */
static void yyrecoverSyntaxError(yyGLRStack* yystackp)
{
  size_t yyk;
  int yyj;

  if (yystackp->yyerrState == 3)
    /* We just shifted the error token and (perhaps) took some
       reductions.  Skip tokens until we can proceed.  */
    while (yytrue)
    {
      yySymbol yytoken;
      if (yychar == YYEOF)
        yyFail(yystackp, YY_NULLPTR);
      if (yychar != YYEMPTY)
      {
        yytoken = YYTRANSLATE(yychar);
        yydestruct("Error: discarding", yytoken, &yylval);
      }
      YYDPRINTF((stderr, "Reading a token: "));
      yychar = yylex();
      if (yychar <= YYEOF)
      {
        yychar = yytoken = YYEOF;
        YYDPRINTF((stderr, "Now at end of input.\n"));
      }
      else
      {
        yytoken = YYTRANSLATE(yychar);
        YY_SYMBOL_PRINT("Next token is", yytoken, &yylval, &yylloc);
      }
      yyj = yypact[yystackp->yytops.yystates[0]->yylrState];
      if (yypact_value_is_default(yyj))
        return;
      yyj += yytoken;
      if (yyj < 0 || YYLAST < yyj || yycheck[yyj] != yytoken)
      {
        if (yydefact[yystackp->yytops.yystates[0]->yylrState] != 0)
          return;
      }
      else if (!yytable_value_is_error(yytable[yyj]))
        return;
    }

  /* Reduce to one stack.  */
  for (yyk = 0; yyk < yystackp->yytops.yysize; yyk += 1)
    if (yystackp->yytops.yystates[yyk] != YY_NULLPTR)
      break;
  if (yyk >= yystackp->yytops.yysize)
    yyFail(yystackp, YY_NULLPTR);
  for (yyk += 1; yyk < yystackp->yytops.yysize; yyk += 1)
    yymarkStackDeleted(yystackp, yyk);
  yyremoveDeletes(yystackp);
  yycompressStack(yystackp);

  /* Now pop stack until we find a state that shifts the error token.  */
  yystackp->yyerrState = 3;
  while (yystackp->yytops.yystates[0] != YY_NULLPTR)
  {
    yyGLRState* yys = yystackp->yytops.yystates[0];
    yyj = yypact[yys->yylrState];
    if (!yypact_value_is_default(yyj))
    {
      yyj += YYTERROR;
      if (0 <= yyj && yyj <= YYLAST && yycheck[yyj] == YYTERROR && yyisShiftAction(yytable[yyj]))
      {
        /* Shift the error token.  */
        YY_SYMBOL_PRINT("Shifting", yystos[yytable[yyj]], &yylval, &yyerrloc);
        yyglrShift(yystackp, 0, yytable[yyj], yys->yyposn, &yylval);
        yys = yystackp->yytops.yystates[0];
        break;
      }
    }
    if (yys->yypred != YY_NULLPTR)
      yydestroyGLRState("Error: popping", yys);
    yystackp->yytops.yystates[0] = yys->yypred;
    yystackp->yynextFree -= 1;
    yystackp->yyspaceLeft += 1;
  }
  if (yystackp->yytops.yystates[0] == YY_NULLPTR)
    yyFail(yystackp, YY_NULLPTR);
}

#define YYCHK1(YYE)                                                                                \
  do                                                                                               \
  {                                                                                                \
    switch (YYE)                                                                                   \
    {                                                                                              \
      case yyok:                                                                                   \
        break;                                                                                     \
      case yyabort:                                                                                \
        goto yyabortlab;                                                                           \
      case yyaccept:                                                                               \
        goto yyacceptlab;                                                                          \
      case yyerr:                                                                                  \
        goto yyuser_error;                                                                         \
      default:                                                                                     \
        goto yybuglab;                                                                             \
    }                                                                                              \
  } while (0)

/*----------.
| yyparse.  |
`----------*/

int yyparse(void)
{
  int yyresult;
  yyGLRStack yystack;
  yyGLRStack* const yystackp = &yystack;
  size_t yyposn;

  YYDPRINTF((stderr, "Starting parse\n"));

  yychar = YYEMPTY;
  yylval = yyval_default;

  if (!yyinitGLRStack(yystackp, YYINITDEPTH))
    goto yyexhaustedlab;
  switch (YYSETJMP(yystack.yyexception_buffer))
  {
    case 0:
      break;
    case 1:
      goto yyabortlab;
    case 2:
      goto yyexhaustedlab;
    default:
      goto yybuglab;
  }
  yyglrShift(&yystack, 0, 0, 0, &yylval);
  yyposn = 0;

  while (yytrue)
  {
    /* For efficiency, we have two loops, the first of which is
       specialized to deterministic operation (single stack, no
       potential ambiguity).  */
    /* Standard mode */
    while (yytrue)
    {
      yyRuleNum yyrule;
      int yyaction;
      const short* yyconflicts;

      yyStateNum yystate = yystack.yytops.yystates[0]->yylrState;
      YYDPRINTF((stderr, "Entering state %d\n", yystate));
      if (yystate == YYFINAL)
        goto yyacceptlab;
      if (yyisDefaultedState(yystate))
      {
        yyrule = yydefaultAction(yystate);
        if (yyrule == 0)
        {
          yyreportSyntaxError(&yystack);
          goto yyuser_error;
        }
        YYCHK1(yyglrReduce(&yystack, 0, yyrule, yytrue));
      }
      else
      {
        yySymbol yytoken;
        if (yychar == YYEMPTY)
        {
          YYDPRINTF((stderr, "Reading a token: "));
          yychar = yylex();
        }

        if (yychar <= YYEOF)
        {
          yychar = yytoken = YYEOF;
          YYDPRINTF((stderr, "Now at end of input.\n"));
        }
        else
        {
          yytoken = YYTRANSLATE(yychar);
          YY_SYMBOL_PRINT("Next token is", yytoken, &yylval, &yylloc);
        }

        yygetLRActions(yystate, yytoken, &yyaction, &yyconflicts);
        if (*yyconflicts != 0)
          break;
        if (yyisShiftAction(yyaction))
        {
          YY_SYMBOL_PRINT("Shifting", yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
          yyposn += 1;
          yyglrShift(&yystack, 0, yyaction, yyposn, &yylval);
          if (0 < yystack.yyerrState)
            yystack.yyerrState -= 1;
        }
        else if (yyisErrorAction(yyaction))
        {
          yyreportSyntaxError(&yystack);
          goto yyuser_error;
        }
        else
          YYCHK1(yyglrReduce(&yystack, 0, -yyaction, yytrue));
      }
    }

    while (yytrue)
    {
      yySymbol yytoken_to_shift;
      size_t yys;

      for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
        yystackp->yytops.yylookaheadNeeds[yys] = (yybool)(yychar != YYEMPTY);

      /* yyprocessOneStack returns one of three things:

          - An error flag.  If the caller is yyprocessOneStack, it
            immediately returns as well.  When the caller is finally
            yyparse, it jumps to an error label via YYCHK1.

          - yyok, but yyprocessOneStack has invoked yymarkStackDeleted
            (&yystack, yys), which sets the top state of yys to NULL.  Thus,
            yyparse's following invocation of yyremoveDeletes will remove
            the stack.

          - yyok, when ready to shift a token.

         Except in the first case, yyparse will invoke yyremoveDeletes and
         then shift the next token onto all remaining stacks.  This
         synchronization of the shift (that is, after all preceding
         reductions on all stacks) helps prevent double destructor calls
         on yylval in the event of memory exhaustion.  */

      for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
        YYCHK1(yyprocessOneStack(&yystack, yys, yyposn));
      yyremoveDeletes(&yystack);
      if (yystack.yytops.yysize == 0)
      {
        yyundeleteLastStack(&yystack);
        if (yystack.yytops.yysize == 0)
          yyFail(&yystack, YY_("syntax error"));
        YYCHK1(yyresolveStack(&yystack));
        YYDPRINTF((stderr, "Returning to deterministic operation.\n"));
        yyreportSyntaxError(&yystack);
        goto yyuser_error;
      }

      /* If any yyglrShift call fails, it will fail after shifting.  Thus,
         a copy of yylval will already be on stack 0 in the event of a
         failure in the following loop.  Thus, yychar is set to YYEMPTY
         before the loop to make sure the user destructor for yylval isn't
         called twice.  */
      yytoken_to_shift = YYTRANSLATE(yychar);
      yychar = YYEMPTY;
      yyposn += 1;
      for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
      {
        int yyaction;
        const short* yyconflicts;
        yyStateNum yystate = yystack.yytops.yystates[yys]->yylrState;
        yygetLRActions(yystate, yytoken_to_shift, &yyaction, &yyconflicts);
        /* Note that yyconflicts were handled by yyprocessOneStack.  */
        YYDPRINTF((stderr, "On stack %lu, ", (unsigned long)yys));
        YY_SYMBOL_PRINT("shifting", yytoken_to_shift, &yylval, &yylloc);
        yyglrShift(&yystack, yys, yyaction, yyposn, &yylval);
        YYDPRINTF((stderr, "Stack %lu now in state #%d\n", (unsigned long)yys,
          yystack.yytops.yystates[yys]->yylrState));
      }

      if (yystack.yytops.yysize == 1)
      {
        YYCHK1(yyresolveStack(&yystack));
        YYDPRINTF((stderr, "Returning to deterministic operation.\n"));
        yycompressStack(&yystack);
        break;
      }
    }
    continue;
  yyuser_error:
    yyrecoverSyntaxError(&yystack);
    yyposn = yystack.yytops.yystates[0]->yyposn;
  }

yyacceptlab:
  yyresult = 0;
  goto yyreturn;

yybuglab:
  YYASSERT(yyfalse);
  goto yyabortlab;

yyabortlab:
  yyresult = 1;
  goto yyreturn;

yyexhaustedlab:
  yyerror(YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;

yyreturn:
  if (yychar != YYEMPTY)
    yydestruct("Cleanup: discarding lookahead", YYTRANSLATE(yychar), &yylval);

  /* If the stack is well-formed, pop the stack until it is empty,
     destroying its entries as we go.  But free the stack regardless
     of whether it is well-formed.  */
  if (yystack.yyitems)
  {
    yyGLRState** yystates = yystack.yytops.yystates;
    if (yystates)
    {
      size_t yysize = yystack.yytops.yysize;
      size_t yyk;
      for (yyk = 0; yyk < yysize; yyk += 1)
        if (yystates[yyk])
        {
          while (yystates[yyk])
          {
            yyGLRState* yys = yystates[yyk];
            if (yys->yypred != YY_NULLPTR)
              yydestroyGLRState("Cleanup: popping", yys);
            yystates[yyk] = yys->yypred;
            yystack.yynextFree -= 1;
            yystack.yyspaceLeft += 1;
          }
          break;
        }
    }
    yyfreeGLRStack(&yystack);
  }

  return yyresult;
}

/* DEBUGGING ONLY */
#if YYDEBUG
static void yy_yypstack(yyGLRState* yys)
{
  if (yys->yypred)
  {
    yy_yypstack(yys->yypred);
    YYFPRINTF(stderr, " -> ");
  }
  YYFPRINTF(stderr, "%d@%lu", yys->yylrState, (unsigned long)yys->yyposn);
}

static void yypstates(yyGLRState* yyst)
{
  if (yyst == YY_NULLPTR)
    YYFPRINTF(stderr, "<null>");
  else
    yy_yypstack(yyst);
  YYFPRINTF(stderr, "\n");
}

static void yypstack(yyGLRStack* yystackp, size_t yyk)
{
  yypstates(yystackp->yytops.yystates[yyk]);
}

#define YYINDEX(YYX) ((YYX) == YY_NULLPTR ? -1 : (yyGLRStackItem*)(YYX)-yystackp->yyitems)

static void yypdumpstack(yyGLRStack* yystackp)
{
  yyGLRStackItem* yyp;
  size_t yyi;
  for (yyp = yystackp->yyitems; yyp < yystackp->yynextFree; yyp += 1)
  {
    YYFPRINTF(stderr, "%3lu. ", (unsigned long)(yyp - yystackp->yyitems));
    if (*(yybool*)yyp)
    {
      YYASSERT(yyp->yystate.yyisState);
      YYASSERT(yyp->yyoption.yyisState);
      YYFPRINTF(stderr, "Res: %d, LR State: %d, posn: %lu, pred: %ld", yyp->yystate.yyresolved,
        yyp->yystate.yylrState, (unsigned long)yyp->yystate.yyposn,
        (long)YYINDEX(yyp->yystate.yypred));
      if (!yyp->yystate.yyresolved)
        YYFPRINTF(stderr, ", firstVal: %ld", (long)YYINDEX(yyp->yystate.yysemantics.yyfirstVal));
    }
    else
    {
      YYASSERT(!yyp->yystate.yyisState);
      YYASSERT(!yyp->yyoption.yyisState);
      YYFPRINTF(stderr, "Option. rule: %d, state: %ld, next: %ld", yyp->yyoption.yyrule - 1,
        (long)YYINDEX(yyp->yyoption.yystate), (long)YYINDEX(yyp->yyoption.yynext));
    }
    YYFPRINTF(stderr, "\n");
  }
  YYFPRINTF(stderr, "Tops:");
  for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
    YYFPRINTF(
      stderr, "%lu: %ld; ", (unsigned long)yyi, (long)YYINDEX(yystackp->yytops.yystates[yyi]));
  YYFPRINTF(stderr, "\n");
}
#endif

#undef yylval
#undef yychar
#undef yynerrs

#include "lex.yy.c"
#include <string.h>

/* fill in the type name if none given */
static const char* type_class(unsigned int type, const char* classname)
{
  if (classname)
  {
    if (classname[0] == '\0')
    {
      switch ((type & VTK_PARSE_BASE_TYPE))
      {
        case 0:
          classname = "auto";
          break;
        case VTK_PARSE_VOID:
          classname = "void";
          break;
        case VTK_PARSE_BOOL:
          classname = "bool";
          break;
        case VTK_PARSE_FLOAT:
          classname = "float";
          break;
        case VTK_PARSE_DOUBLE:
          classname = "double";
          break;
        case VTK_PARSE_LONG_DOUBLE:
          classname = "long double";
          break;
        case VTK_PARSE_CHAR:
          classname = "char";
          break;
        case VTK_PARSE_CHAR16_T:
          classname = "char16_t";
          break;
        case VTK_PARSE_CHAR32_T:
          classname = "char32_t";
          break;
        case VTK_PARSE_WCHAR_T:
          classname = "wchar_t";
          break;
        case VTK_PARSE_UNSIGNED_CHAR:
          classname = "unsigned char";
          break;
        case VTK_PARSE_SIGNED_CHAR:
          classname = "signed char";
          break;
        case VTK_PARSE_SHORT:
          classname = "short";
          break;
        case VTK_PARSE_UNSIGNED_SHORT:
          classname = "unsigned short";
          break;
        case VTK_PARSE_INT:
          classname = "int";
          break;
        case VTK_PARSE_UNSIGNED_INT:
          classname = "unsigned int";
          break;
        case VTK_PARSE_LONG:
          classname = "long";
          break;
        case VTK_PARSE_UNSIGNED_LONG:
          classname = "unsigned long";
          break;
        case VTK_PARSE_LONG_LONG:
          classname = "long long";
          break;
        case VTK_PARSE_UNSIGNED_LONG_LONG:
          classname = "unsigned long long";
          break;
        case VTK_PARSE___INT64:
          classname = "__int64";
          break;
        case VTK_PARSE_UNSIGNED___INT64:
          classname = "unsigned __int64";
          break;
      }
    }
  }

  return classname;
}

/* check whether this is the class we are looking for */
static void start_class(const char* classname, int is_struct_or_union)
{
  ClassInfo* outerClass = currentClass;
  pushClass();
  currentClass = (ClassInfo*)malloc(sizeof(ClassInfo));
  vtkParse_InitClass(currentClass);
  currentClass->Name = classname;
  if (is_struct_or_union == 1)
  {
    currentClass->ItemType = VTK_STRUCT_INFO;
  }
  if (is_struct_or_union == 2)
  {
    currentClass->ItemType = VTK_UNION_INFO;
  }

  if (getAttributes() & VTK_PARSE_WRAPEXCLUDE)
  {
    currentClass->IsExcluded = 1;
  }

  if (getAttributes() & VTK_PARSE_DEPRECATED)
  {
    currentClass->IsDeprecated = 1;
    currentClass->DeprecatedReason = deprecationReason;
    currentClass->DeprecatedVersion = deprecationVersion;
  }

  if (classname && classname[0] != '\0')
  {
    /* if name of class being defined contains "::" or "<..>", then skip it */
    const char* cp = classname;
    while (*cp != '\0' && *cp != ':' && *cp != '>')
    {
      cp++;
    }
    if (*cp == '\0')
    {
      if (outerClass)
      {
        vtkParse_AddClassToClass(outerClass, currentClass);
      }
      else
      {
        vtkParse_AddClassToNamespace(currentNamespace, currentClass);
      }
    }
  }

  /* template information */
  if (currentTemplate)
  {
    currentClass->Template = currentTemplate;
    currentTemplate = NULL;
  }

  /* comment, if any */
  currentClass->Comment = vtkstrdup(getComment());

  access_level = VTK_ACCESS_PRIVATE;
  if (is_struct_or_union)
  {
    access_level = VTK_ACCESS_PUBLIC;
  }

  vtkParse_InitFunction(currentFunction);
  startSig();
  clearComment();
  clearType();
  clearTypeId();
}

/* reached the end of a class definition */
static void end_class(void)
{
  /* add default constructors */
  vtkParse_AddDefaultConstructors(currentClass, data->Strings);

  popClass();
}

/* add a base class to the specified class */
static void add_base_class(ClassInfo* cls, const char* name, int access_lev, unsigned int extra)
{
  /* "extra" can contain VTK_PARSE_VIRTUAL and VTK_PARSE_PACK */
  if (cls && access_lev == VTK_ACCESS_PUBLIC && (extra & VTK_PARSE_VIRTUAL) == 0 &&
    (extra & VTK_PARSE_PACK) == 0)
  {
    vtkParse_AddStringToArray(&cls->SuperClasses, &cls->NumberOfSuperClasses, name);
  }
}

/* add a using declaration or directive */
static void add_using(const char* name, int is_namespace)
{
  size_t i;
  UsingInfo* item;

  item = (UsingInfo*)malloc(sizeof(UsingInfo));
  vtkParse_InitUsing(item);
  if (is_namespace)
  {
    item->Name = NULL;
    item->Scope = name;
  }
  else
  {
    i = strlen(name);
    while (i > 0 && name[i - 1] != ':')
    {
      i--;
    }
    item->Name = vtkstrdup(&name[i]);
    while (i > 0 && name[i - 1] == ':')
    {
      i--;
    }
    item->Scope = vtkstrndup(name, i);
    item->Access = access_level;
  }

  if (currentClass)
  {
    vtkParse_AddUsingToClass(currentClass, item);
  }
  else
  {
    vtkParse_AddUsingToNamespace(currentNamespace, item);
  }
}

/* start a new enum */
static void start_enum(const char* name, int is_scoped, unsigned int type, const char* basename)
{
  EnumInfo* item;

  currentEnumType = (type ? type : VTK_PARSE_INT);
  currentEnumName = "int";
  currentEnumValue = NULL;

  if (type == 0 && is_scoped)
  {
    type = VTK_PARSE_INT;
  }

  if (name)
  {
    currentEnumName = name;
    item = (EnumInfo*)malloc(sizeof(EnumInfo));
    vtkParse_InitEnum(item);
    item->Name = name;
    item->Comment = vtkstrdup(getComment());
    item->Access = access_level;

    if (getAttributes() & VTK_PARSE_WRAPEXCLUDE)
    {
      item->IsExcluded = 1;
    }

    if (getAttributes() & VTK_PARSE_DEPRECATED)
    {
      item->IsDeprecated = 1;
      item->DeprecatedReason = deprecationReason;
      item->DeprecatedVersion = deprecationVersion;
    }

    if (currentClass)
    {
      vtkParse_AddEnumToClass(currentClass, item);
    }
    else
    {
      vtkParse_AddEnumToNamespace(currentNamespace, item);
    }

    if (type)
    {
      vtkParse_AddStringToArray(
        &item->SuperClasses, &item->NumberOfSuperClasses, type_class(type, basename));
    }

    if (is_scoped)
    {
      pushClass();
      currentClass = item;
    }
  }
}

/* finish the enum */
static void end_enum(void)
{
  if (currentClass && currentClass->ItemType == VTK_ENUM_INFO)
  {
    popClass();
  }

  currentEnumName = NULL;
  currentEnumValue = NULL;
}

/* add a constant to the enum */
static void add_enum(const char* name, const char* value)
{
  static char text[2048];
  unsigned int attribs = getAttributes();
  int i;
  long j;

  if (value)
  {
    strcpy(text, value);
    currentEnumValue = value;
  }
  else if (currentEnumValue)
  {
    i = strlen(text);
    while (i > 0 && text[i - 1] >= '0' && text[i - 1] <= '9')
    {
      i--;
    }

    if (i == 0 || text[i - 1] == ' ' ||
      (i > 1 && text[i - 2] == ' ' && (text[i - 1] == '-' || text[i - 1] == '+')))
    {
      if (i > 0 && text[i - 1] != ' ')
      {
        i--;
      }
      j = (int)strtol(&text[i], NULL, 10);
      sprintf(&text[i], "%li", j + 1);
    }
    else
    {
      i = strlen(text);
      strcpy(&text[i], " + 1");
    }
    currentEnumValue = vtkstrdup(text);
  }
  else
  {
    strcpy(text, "0");
    currentEnumValue = "0";
  }

  add_constant(name, currentEnumValue, attribs, currentEnumType, currentEnumName, 2);
}

/* for a macro constant, guess the constant type, doesn't do any math */
static unsigned int guess_constant_type(const char* valstring)
{
  unsigned int valtype = 0;
  size_t k;
  int i;
  int is_name = 0;

  if (valstring == NULL || valstring[0] == '\0')
  {
    return 0;
  }

  k = vtkParse_SkipId(valstring);
  if (valstring[k] == '\0')
  {
    is_name = 1;
  }

  if (strcmp(valstring, "true") == 0 || strcmp(valstring, "false") == 0)
  {
    return VTK_PARSE_BOOL;
  }

  if (strcmp(valstring, "nullptr") == 0 || strcmp(valstring, "NULL") == 0)
  {
    return VTK_PARSE_NULLPTR_T;
  }

  if (valstring[0] == '\'')
  {
    return VTK_PARSE_CHAR;
  }

  if (strncmp(valstring, "VTK_TYPE_CAST(", 14) == 0 ||
    strncmp(valstring, "static_cast<", 12) == 0 || strncmp(valstring, "const_cast<", 11) == 0 ||
    strncmp(valstring, "(", 1) == 0)
  {
    const char* cp;
    size_t n;
    int is_unsigned = 0;

    cp = &valstring[1];
    if (valstring[0] == 'c')
    {
      cp = &valstring[11];
    }
    else if (valstring[0] == 's')
    {
      cp = &valstring[12];
    }
    else if (valstring[0] == 'V')
    {
      cp = &valstring[14];
    }

    if (strncmp(cp, "unsigned ", 9) == 0)
    {
      is_unsigned = 1;
      cp += 9;
    }

    n = strlen(cp);
    for (k = 0; k < n && cp[k] != ',' && cp[k] != '>' && cp[k] != ')'; k++)
    {
    }

    if (strncmp(cp, "long long", k) == 0)
    {
      valtype = VTK_PARSE_LONG_LONG;
    }
    else if (strncmp(cp, "__int64", k) == 0)
    {
      valtype = VTK_PARSE___INT64;
    }
    else if (strncmp(cp, "long", k) == 0)
    {
      valtype = VTK_PARSE_LONG;
    }
    else if (strncmp(cp, "short", k) == 0)
    {
      valtype = VTK_PARSE_SHORT;
    }
    else if (strncmp(cp, "signed char", k) == 0)
    {
      valtype = VTK_PARSE_SIGNED_CHAR;
    }
    else if (strncmp(cp, "char", k) == 0)
    {
      valtype = VTK_PARSE_CHAR;
    }
    else if (strncmp(cp, "int", k) == 0 || strncmp(cp, "signed", k) == 0)
    {
      valtype = VTK_PARSE_INT;
    }
    else if (strncmp(cp, "float", k) == 0)
    {
      valtype = VTK_PARSE_FLOAT;
    }
    else if (strncmp(cp, "double", k) == 0)
    {
      valtype = VTK_PARSE_DOUBLE;
    }
    else if (strncmp(cp, "char *", k) == 0)
    {
      valtype = VTK_PARSE_CHAR_PTR;
    }

    if (is_unsigned)
    {
      if (valtype == 0)
      {
        valtype = VTK_PARSE_INT;
      }
      valtype = (valtype | VTK_PARSE_UNSIGNED);
    }

    if (valtype != 0)
    {
      return valtype;
    }
  }

  /* check the current scope */
  if (is_name)
  {
    NamespaceInfo* scope = currentNamespace;
    if (namespaceDepth > 0)
    {
      scope = namespaceStack[0];
    }

    for (i = 0; i < scope->NumberOfConstants; i++)
    {
      if (strcmp(scope->Constants[i]->Name, valstring) == 0)
      {
        return scope->Constants[i]->Type;
      }
    }
  }

  /* check for preprocessor macros */
  if (is_name)
  {
    MacroInfo* macro = vtkParsePreprocess_GetMacro(preprocessor, valstring);

    if (macro && !macro->IsFunction)
    {
      return guess_constant_type(macro->Definition);
    }
  }

  /* fall back to the preprocessor to evaluate the constant */
  {
    preproc_int_t val;
    int is_unsigned;
    int result = vtkParsePreprocess_EvaluateExpression(preprocessor, valstring, &val, &is_unsigned);

    if (result == VTK_PARSE_PREPROC_DOUBLE)
    {
      return VTK_PARSE_DOUBLE;
    }
    else if (result == VTK_PARSE_PREPROC_FLOAT)
    {
      return VTK_PARSE_FLOAT;
    }
    else if (result == VTK_PARSE_PREPROC_STRING)
    {
      return VTK_PARSE_CHAR_PTR;
    }
    else if (result == VTK_PARSE_OK)
    {
      if (is_unsigned)
      {
        if ((preproc_uint_t)val <= UINT_MAX)
        {
          return VTK_PARSE_UNSIGNED_INT;
        }
        else
        {
          return VTK_PARSE_UNSIGNED_LONG_LONG;
        }
      }
      else
      {
        if (val >= INT_MIN && val <= INT_MAX)
        {
          return VTK_PARSE_INT;
        }
        else
        {
          return VTK_PARSE_LONG_LONG;
        }
      }
    }
  }

  return 0;
}

/* add a constant to the current class or namespace */
static void add_constant(const char* name, const char* value, unsigned int attributes,
  unsigned int type, const char* typeclass, int flag)
{
  ValueInfo* con = (ValueInfo*)malloc(sizeof(ValueInfo));
  vtkParse_InitValue(con);
  con->ItemType = VTK_CONSTANT_INFO;
  con->Name = name;
  con->Comment = vtkstrdup(getComment());
  con->Value = value;
  con->Attributes = attributes;
  con->Type = type;
  con->Class = type_class(type, typeclass);

  if (flag == 2)
  {
    con->IsEnum = 1;
  }

  if (flag == 1)
  {
    /* actually a macro, need to guess the type */
    ValueInfo** cptr = data->Contents->Constants;
    int n = data->Contents->NumberOfConstants;
    int i;

    con->Access = VTK_ACCESS_PUBLIC;
    if (con->Type == 0)
    {
      con->Type = guess_constant_type(con->Value);
    }

    for (i = 0; i < n; i++)
    {
      if (strcmp(cptr[i]->Name, con->Name) == 0)
      {
        break;
      }
    }

    if (i == n)
    {
      vtkParse_AddConstantToNamespace(data->Contents, con);
    }
    else
    {
      vtkParse_FreeValue(con);
    }
  }
  else if (currentClass)
  {
    con->Access = access_level;
    vtkParse_AddConstantToClass(currentClass, con);
  }
  else
  {
    con->Access = VTK_ACCESS_PUBLIC;
    vtkParse_AddConstantToNamespace(currentNamespace, con);
  }
}

/* guess the type from the ID */
static unsigned int guess_id_type(const char* cp)
{
  unsigned int t = 0;

  if (cp)
  {
    size_t i;
    const char* dp;

    i = strlen(cp);
    while (i > 0 && cp[i - 1] != ':')
    {
      i--;
    }
    dp = &cp[i];

    if (strcmp(dp, "vtkStdString") == 0 || strcmp(cp, "std::string") == 0)
    {
      t = VTK_PARSE_STRING;
    }
    else if (strcmp(dp, "vtkUnicodeString") == 0)
    {
      t = VTK_PARSE_UNICODE_STRING;
    }
    else if (strncmp(dp, "vtk", 3) == 0)
    {
      t = VTK_PARSE_OBJECT;
    }
    else if (strncmp(dp, "Q", 1) == 0 || strncmp(cp, "Qt::", 4) == 0)
    {
      t = VTK_PARSE_QOBJECT;
    }
    else
    {
      t = VTK_PARSE_UNKNOWN;
    }
  }

  return t;
}

/* add a template parameter to the current template */
static void add_template_parameter(unsigned int datatype, unsigned int extra, const char* funcSig)
{
  ValueInfo* param = (ValueInfo*)malloc(sizeof(ValueInfo));
  vtkParse_InitValue(param);
  handle_complex_type(param, 0, datatype, extra, funcSig);
  param->Name = getVarName();
  vtkParse_AddParameterToTemplate(currentTemplate, param);
}

/* set the return type for the current function */
static void set_return(
  FunctionInfo* func, unsigned int attributes, unsigned int type, const char* typeclass, int count)
{
  char text[64];
  ValueInfo* val = (ValueInfo*)malloc(sizeof(ValueInfo));

  vtkParse_InitValue(val);
  val->Attributes = attributes;
  val->Type = type;
  val->Class = type_class(type, typeclass);

  if (count)
  {
    val->Count = count;
    sprintf(text, "%i", count);
    vtkParse_AddStringToArray(&val->Dimensions, &val->NumberOfDimensions, vtkstrdup(text));
  }

  func->ReturnValue = val;

#ifndef VTK_PARSE_LEGACY_REMOVE
  func->ReturnType = val->Type;
  func->ReturnClass = val->Class;
  func->HaveHint = (count > 0);
  func->HintSize = count;
#endif
}

static int count_from_dimensions(ValueInfo* val)
{
  int count, i, n;
  const char* cp;

  /* count is the product of the dimensions */
  count = 0;
  if (val->NumberOfDimensions)
  {
    count = 1;
    for (i = 0; i < val->NumberOfDimensions; i++)
    {
      n = 0;
      cp = val->Dimensions[i];
      if (cp[0] != '\0')
      {
        while (*cp >= '0' && *cp <= '9')
        {
          cp++;
        }
        while (*cp == 'u' || *cp == 'l' || *cp == 'U' || *cp == 'L')
        {
          cp++;
        }
        if (*cp == '\0')
        {
          n = (int)strtol(val->Dimensions[i], NULL, 0);
        }
      }
      count *= n;
    }
  }

  return count;
}

/* deal with types that include function pointers or arrays */
static void handle_complex_type(ValueInfo* val, unsigned int attributes, unsigned int datatype,
  unsigned int extra, const char* funcSig)
{
  FunctionInfo* func = 0;

  /* remove specifiers like "friend" and "typedef" */
  datatype &= VTK_PARSE_QUALIFIED_TYPE;

  /* remove the pack specifier caused by "..." */
  if ((extra & VTK_PARSE_PACK) != 0)
  {
    val->IsPack = 1;
    extra ^= VTK_PARSE_PACK;
  }

  /* if "extra" was set, parentheses were involved */
  if ((extra & VTK_PARSE_BASE_TYPE) == VTK_PARSE_FUNCTION)
  {
    /* the current type becomes the function return type */
    func = getFunction();
    func->ReturnValue = (ValueInfo*)malloc(sizeof(ValueInfo));
    vtkParse_InitValue(func->ReturnValue);
    func->ReturnValue->Attributes = attributes;
    func->ReturnValue->Type = datatype;
    func->ReturnValue->Class = type_class(datatype, getTypeId());
    if (funcSig)
    {
      func->Signature = vtkstrdup(funcSig);
    }
    val->Function = func;

#ifndef VTK_PARSE_LEGACY_REMOVE
    func->ReturnType = func->ReturnValue->Type;
    func->ReturnClass = func->ReturnValue->Class;
#endif

    /* the val type is whatever was inside the parentheses */
    clearTypeId();
    setTypeId(func->Class ? "method" : "function");
    datatype = (extra & (VTK_PARSE_UNQUALIFIED_TYPE | VTK_PARSE_RVALUE));
    attributes = 0;
  }
  else if ((extra & VTK_PARSE_INDIRECT) == VTK_PARSE_BAD_INDIRECT)
  {
    datatype = (datatype | VTK_PARSE_BAD_INDIRECT);
  }
  else if ((extra & VTK_PARSE_INDIRECT) != 0)
  {
    extra = (extra & (VTK_PARSE_INDIRECT | VTK_PARSE_RVALUE));

    if ((extra & VTK_PARSE_REF) != 0)
    {
      datatype = (datatype | (extra & (VTK_PARSE_REF | VTK_PARSE_RVALUE)));
      extra = (extra & ~(VTK_PARSE_REF | VTK_PARSE_RVALUE));
    }

    if (extra != 0 && getArrayNDims() > 0)
    {
      /* pointer represents an unsized array bracket */
      datatype = add_indirection(datatype, VTK_PARSE_ARRAY);
      extra = ((extra >> 2) & VTK_PARSE_POINTER_MASK);
    }

    datatype = add_indirection(datatype, extra);
  }

  if (getArrayNDims() == 1)
  {
    if ((datatype & VTK_PARSE_POINTER_LOWMASK) != VTK_PARSE_ARRAY)
    {
      /* turn the first set of brackets into a pointer */
      datatype = add_indirection(datatype, VTK_PARSE_POINTER);
    }
    else
    {
      pushArrayFront("");
    }
  }
  else if (getArrayNDims() > 1)
  {
    if ((datatype & VTK_PARSE_POINTER_LOWMASK) != VTK_PARSE_ARRAY)
    {
      /* turn the first set of brackets into a pointer */
      datatype = add_indirection(datatype, VTK_PARSE_ARRAY);
    }
    else
    {
      pushArrayFront("");
    }
  }

  /* get the attributes */
  val->Attributes = attributes;

  /* get the data type */
  val->Type = datatype;
  val->Class = type_class(datatype, getTypeId());

  /* copy contents of all brackets to the ArgDimensions */
  val->NumberOfDimensions = getArrayNDims();
  val->Dimensions = getArray();
  clearArray();

  /* count is the product of the dimensions */
  val->Count = count_from_dimensions(val);
}

/* handle [[attributes]] */
static void handle_attribute(const char* att, int pack)
{
  /* the role means "this is what the attribute applies to" */
  int role = getAttributeRole();

  size_t l = 0;
  size_t la = 0;
  const char* args = NULL;

  if (!att)
  {
    return;
  }

  /* append the prefix from the "using" statement */
  if (getAttributePrefix())
  {
    att = vtkstrcat(getAttributePrefix(), att);
  }

  /* search for arguments */
  l = vtkParse_SkipId(att);
  while (att[l] == ':' && att[l + 1] == ':')
  {
    l += 2;
    l += vtkParse_SkipId(&att[l]);
  }
  if (att[l] == '(')
  {
    /* strip the parentheses and whitespace from the args */
    args = &att[l + 1];
    while (*args == ' ')
    {
      args++;
    }
    la = strlen(args);
    while (la > 0 && args[la - 1] == ' ')
    {
      la--;
    }
    if (la > 0 && args[la - 1] == ')')
    {
      la--;
    }
    while (la > 0 && args[la - 1] == ' ')
    {
      la--;
    }
  }

  /* check for namespace */
  if (strncmp(att, "vtk::", 5) == 0)
  {
    if (pack)
    {
      /* no current vtk attributes use '...' */
      print_parser_error("attribute takes no ...", att, l);
      exit(1);
    }
    else if (l == 16 && strncmp(att, "vtk::wrapexclude", l) == 0 && !args &&
      (role == VTK_PARSE_ATTRIB_DECL || role == VTK_PARSE_ATTRIB_CLASS))
    {
      addAttribute(VTK_PARSE_WRAPEXCLUDE);
    }
    else if (l == 16 && strncmp(att, "vtk::newinstance", l) == 0 && !args &&
      role == VTK_PARSE_ATTRIB_DECL)
    {
      addAttribute(VTK_PARSE_NEWINSTANCE);
    }
    else if (l == 13 && strncmp(att, "vtk::zerocopy", l) == 0 && !args &&
      role == VTK_PARSE_ATTRIB_DECL)
    {
      addAttribute(VTK_PARSE_ZEROCOPY);
    }
    else if (l == 13 && strncmp(att, "vtk::filepath", l) == 0 && !args &&
      role == VTK_PARSE_ATTRIB_DECL)
    {
      addAttribute(VTK_PARSE_FILEPATH);
    }
    else if (l == 15 && strncmp(att, "vtk::deprecated", l) == 0 &&
      (role == VTK_PARSE_ATTRIB_DECL || role == VTK_PARSE_ATTRIB_CLASS ||
        role == VTK_PARSE_ATTRIB_ID))
    {
      addAttribute(VTK_PARSE_DEPRECATED);
      deprecationReason = NULL;
      deprecationVersion = NULL;
      if (args)
      {
        size_t lr = vtkParse_SkipQuotes(args);
        deprecationReason = vtkstrndup(args, lr);
        if (lr < la && args[lr] == ',')
        {
          /* skip spaces and get the next argument */
          do
          {
            ++lr;
          } while (lr < la && args[lr] == ' ');
          deprecationVersion = vtkstrndup(&args[lr], vtkParse_SkipQuotes(&args[lr]));
        }
      }
    }
    else if (l == 12 && strncmp(att, "vtk::expects", l) == 0 && args &&
      role == VTK_PARSE_ATTRIB_FUNC)
    {
      /* add to the preconditions */
      vtkParse_AddStringToArray(
        &currentFunction->Preconds, &currentFunction->NumberOfPreconds, vtkstrndup(args, la));
    }
    else if (l == 13 && strncmp(att, "vtk::sizehint", l) == 0 && args &&
      role == VTK_PARSE_ATTRIB_FUNC)
    {
      /* first arg is parameter name, unless return value hint */
      ValueInfo* arg = currentFunction->ReturnValue;
      size_t n = vtkParse_SkipId(args);
      preproc_int_t count;
      int is_unsigned;
      int i;

      l = n;
      while (args[n] == ' ')
      {
        n++;
      }
      if (l > 0 && args[n] == ',')
      {
        do
        {
          n++;
        } while (args[n] == ' ');
        /* find the named parameter */
        for (i = 0; i < currentFunction->NumberOfParameters; i++)
        {
          arg = currentFunction->Parameters[i];
          if (arg->Name && strlen(arg->Name) == l && strncmp(arg->Name, args, l) == 0)
          {
            break;
          }
        }
        if (i == currentFunction->NumberOfParameters)
        {
          print_parser_error("unrecognized parameter name", args, l);
          exit(1);
        }
        /* advance args to second attribute arg */
        args += n;
        la -= n;
      }
      /* set the size hint */
      arg->CountHint = vtkstrndup(args, la);
      /* see if hint is an integer */
      if (VTK_PARSE_OK ==
        vtkParsePreprocess_EvaluateExpression(preprocessor, arg->CountHint, &count, &is_unsigned))
      {
        if (count > 0 && count < 127)
        {
          arg->CountHint = NULL;
          arg->Count = (int)count;
#ifndef VTK_PARSE_LEGACY_REMOVE
          if (arg == currentFunction->ReturnValue)
          {
            currentFunction->HaveHint = 1;
            currentFunction->HintSize = arg->Count;
          }
#endif
        }
      }
    }
    else
    {
      print_parser_error("attribute cannot be used here", att, l);
      exit(1);
    }
  }
}

/* add a parameter to the legacy part of the FunctionInfo struct */
static void add_legacy_parameter(FunctionInfo* func, ValueInfo* param)
{
#ifndef VTK_PARSE_LEGACY_REMOVE
  int i = func->NumberOfArguments;

  if (i < MAX_ARGS)
  {
    func->NumberOfArguments = i + 1;
    func->ArgTypes[i] = param->Type;
    func->ArgClasses[i] = param->Class;
    func->ArgCounts[i] = param->Count;

    /* legacy wrappers need VTK_PARSE_FUNCTION without POINTER */
    if (param->Type == VTK_PARSE_FUNCTION_PTR)
    {
      /* check for signature "void (*func)(void *)" */
      if (param->Function->NumberOfParameters == 1 &&
        param->Function->Parameters[0]->Type == VTK_PARSE_VOID_PTR &&
        param->Function->Parameters[0]->NumberOfDimensions == 0 &&
        param->Function->ReturnValue->Type == VTK_PARSE_VOID)
      {
        func->ArgTypes[i] = VTK_PARSE_FUNCTION;
      }
    }
  }
  else
  {
    func->ArrayFailure = 1;
  }
#endif
}

/* reject the function, do not output it */
static void reject_function(void)
{
  vtkParse_FreeFunction(currentFunction);
  currentFunction = (FunctionInfo*)malloc(sizeof(FunctionInfo));
  vtkParse_InitFunction(currentFunction);
  startSig();
  getMacro();
}

/* a simple routine that updates a few variables */
static void output_function(void)
{
  size_t n;
  int i, j;
  int match;

  /* reject template specializations */
  n = strlen(currentFunction->Name);
  if (currentFunction->Name[n - 1] == '>')
  {
    /* make sure there is a matching angle bracket */
    while (n > 0 && currentFunction->Name[n - 1] != '<')
    {
      n--;
    }
    if (n > 0)
    {
      reject_function();
      return;
    }
  }

  /* check return value for specifiers that apply to the function */
  if (currentFunction->ReturnValue)
  {
    if (currentFunction->ReturnValue->Attributes & VTK_PARSE_WRAPEXCLUDE)
    {
      /* remove "wrapexclude" attrib from ReturnValue, attach it to function */
      currentFunction->ReturnValue->Attributes ^= VTK_PARSE_WRAPEXCLUDE;
      currentFunction->IsExcluded = 1;
    }

    if (currentFunction->ReturnValue->Attributes & VTK_PARSE_DEPRECATED)
    {
      /* remove "deprecated" attrib from ReturnValue, attach it to function */
      currentFunction->ReturnValue->Attributes ^= VTK_PARSE_DEPRECATED;
      currentFunction->IsDeprecated = 1;
      currentFunction->DeprecatedReason = deprecationReason;
      currentFunction->DeprecatedVersion = deprecationVersion;
    }

    if (currentFunction->ReturnValue->Type & VTK_PARSE_FRIEND)
    {
      /* remove "friend" specifier from ReturnValue */
      currentFunction->ReturnValue->Type ^= VTK_PARSE_FRIEND;
      /* handle the function declaration (ignore the "friend" part) */
      output_friend_function();
      return;
    }

    if (currentFunction->ReturnValue->Type & VTK_PARSE_TYPEDEF)
    {
      /* remove 'typedef' specifier from return value */
      currentFunction->ReturnValue->Type ^= VTK_PARSE_TYPEDEF;
      /* we ignore function typedefs, they're exceedingly rare */
      reject_function();
      return;
    }

    if (currentFunction->ReturnValue->Type & VTK_PARSE_STATIC)
    {
      /* mark function or method as "static" */
      currentFunction->IsStatic = 1;
    }

    if (currentFunction->ReturnValue->Type & VTK_PARSE_VIRTUAL)
    {
      /* mark method as "virtual" */
      currentFunction->IsVirtual = 1;
    }
  }

  /* the signature */
  if (!currentFunction->Signature)
  {
    currentFunction->Signature = getSig();
  }

  /* template information */
  if (currentTemplate)
  {
    currentFunction->Template = currentTemplate;
    currentTemplate = NULL;
  }

  /* a void argument is the same as no parameters */
  if (currentFunction->NumberOfParameters == 1 &&
    (currentFunction->Parameters[0]->Type & VTK_PARSE_UNQUALIFIED_TYPE) == VTK_PARSE_VOID)
  {
    vtkParse_FreeValue(currentFunction->Parameters[0]);
    free(currentFunction->Parameters);
    currentFunction->NumberOfParameters = 0;
  }

  /* set public, protected */
  if (currentClass)
  {
    currentFunction->Access = access_level;
  }
  else
  {
    currentFunction->Access = VTK_ACCESS_PUBLIC;
  }

#ifndef VTK_PARSE_LEGACY_REMOVE
  /* a void argument is the same as no parameters */
  if (currentFunction->NumberOfArguments == 1 &&
    (currentFunction->ArgTypes[0] & VTK_PARSE_UNQUALIFIED_TYPE) == VTK_PARSE_VOID)
  {
    currentFunction->NumberOfArguments = 0;
  }

  /* if return type is void, set return class to void */
  if (currentFunction->ReturnClass == NULL &&
    (currentFunction->ReturnType & VTK_PARSE_UNQUALIFIED_TYPE) == VTK_PARSE_VOID)
  {
    currentFunction->ReturnClass = "void";
  }

  /* set legacy flags */
  if (currentClass)
  {
    currentFunction->IsPublic = (access_level == VTK_ACCESS_PUBLIC);
    currentFunction->IsProtected = (access_level == VTK_ACCESS_PROTECTED);
  }
  else
  {
    currentFunction->IsPublic = 1;
    currentFunction->IsProtected = 0;
  }

  /* check for too many parameters */
  if (currentFunction->NumberOfParameters > MAX_ARGS)
  {
    currentFunction->ArrayFailure = 1;
  }

  for (i = 0; i < currentFunction->NumberOfParameters; i++)
  {
    ValueInfo* param = currentFunction->Parameters[i];
    /* tell old wrappers that multi-dimensional arrays are bad */
    if ((param->Type & VTK_PARSE_POINTER_MASK) != 0)
    {
      if (((param->Type & VTK_PARSE_INDIRECT) == VTK_PARSE_BAD_INDIRECT) ||
        ((param->Type & VTK_PARSE_POINTER_LOWMASK) != VTK_PARSE_POINTER))
      {
        currentFunction->ArrayFailure = 1;
      }
    }

    /* allow only "void (*func)(void *)" as a valid function pointer */
    if ((param->Type & VTK_PARSE_BASE_TYPE) == VTK_PARSE_FUNCTION)
    {
      if (i != 0 || param->Type != VTK_PARSE_FUNCTION_PTR ||
        currentFunction->NumberOfParameters != 2 ||
        currentFunction->Parameters[1]->Type != VTK_PARSE_VOID_PTR ||
        param->Function->NumberOfParameters != 1 ||
        param->Function->Parameters[0]->Type != VTK_PARSE_VOID_PTR ||
        param->Function->Parameters[0]->NumberOfDimensions != 0 ||
        param->Function->ReturnValue->Type != VTK_PARSE_VOID)
      {
        currentFunction->ArrayFailure = 1;
      }
    }
  }
#endif /* VTK_PARSE_LEGACY_REMOVE */

  if (currentClass)
  {
    /* is it a delete function */
    if (currentFunction->Name && !strcmp("Delete", currentFunction->Name))
    {
      currentClass->HasDelete = 1;
    }

    currentFunction->Class = currentClass->Name;
    vtkParse_AddFunctionToClass(currentClass, currentFunction);

    currentFunction = (FunctionInfo*)malloc(sizeof(FunctionInfo));
  }
  else
  {
    /* make sure this function isn't a repeat */
    match = 0;
    for (i = 0; i < currentNamespace->NumberOfFunctions; i++)
    {
      if (currentNamespace->Functions[i]->Name &&
        strcmp(currentNamespace->Functions[i]->Name, currentFunction->Name) == 0)
      {
        if (currentNamespace->Functions[i]->NumberOfParameters ==
          currentFunction->NumberOfParameters)
        {
          for (j = 0; j < currentFunction->NumberOfParameters; j++)
          {
            if (currentNamespace->Functions[i]->Parameters[j]->Type ==
              currentFunction->Parameters[j]->Type)
            {
              if (currentFunction->Parameters[j]->Type == VTK_PARSE_OBJECT &&
                strcmp(currentNamespace->Functions[i]->Parameters[j]->Class,
                  currentFunction->Parameters[j]->Class) == 0)
              {
                break;
              }
            }
          }
          if (j == currentFunction->NumberOfParameters)
          {
            match = 1;
            break;
          }
        }
      }
    }

    if (!match)
    {
      vtkParse_AddFunctionToNamespace(currentNamespace, currentFunction);

      currentFunction = (FunctionInfo*)malloc(sizeof(FunctionInfo));
    }
  }

  vtkParse_InitFunction(currentFunction);
  startSig();
}

/* output a function that is not a method of the current class */
static void output_friend_function(void)
{
  ClassInfo* tmpc = currentClass;
  currentClass = NULL;
  output_function();
  currentClass = tmpc;
}

/* dump predefined macros to the specified file. */
static void dump_macros(const char* filename)
{
  MacroInfo* macro = NULL;
  FILE* ofile = stdout;
  int i;

  if (filename)
  {
    ofile = fopen(filename, "w");
    if (!ofile)
    {
      fprintf(stderr, "Error opening output file %s\n", filename);
      return;
    }
  }

  while ((macro = vtkParsePreprocess_NextMacro(preprocessor, macro)) != 0)
  {
    if (macro->IsFunction)
    {
      fprintf(ofile, "#define %s(", macro->Name);
      for (i = 0; i < macro->NumberOfParameters; i++)
      {
        fprintf(ofile, "%s%s", (i == 0 ? "" : ","), macro->Parameters[i]);
      }
      fprintf(ofile, ")%s%s\n", (macro->Definition ? " " : ""), macro->Definition);
    }
    else if (macro->Definition)
    {
      fprintf(ofile, "#define %s %s\n", macro->Name, macro->Definition);
    }
    else
    {
      fprintf(ofile, "#define %s\n", macro->Name);
    }
  }

  if (filename)
  {
    fclose(ofile);
  }
}

/* Set a flag to recurse into included files */
void vtkParse_SetRecursive(int option)
{
  if (option)
  {
    Recursive = 1;
  }
  else
  {
    Recursive = 0;
  }
}

/* Set the global variable that stores the current executable */
void vtkParse_SetCommandName(const char* name)
{
  CommandName = name;
}

/* Parse a header file and return a FileInfo struct */
FileInfo* vtkParse_ParseFile(const char* filename, FILE* ifile, FILE* errfile)
{
  int i, j;
  int ret;
  FileInfo* file_info;
  char* main_class;

  /* "data" is a global variable used by the parser */
  data = (FileInfo*)malloc(sizeof(FileInfo));
  vtkParse_InitFile(data);
  data->Strings = &system_strings;

  /* "preprocessor" is a global struct used by the parser */
  preprocessor = (PreprocessInfo*)malloc(sizeof(PreprocessInfo));
  vtkParsePreprocess_Init(preprocessor, filename);
  preprocessor->Strings = data->Strings;
  preprocessor->System = &system_cache;
  vtkParsePreprocess_AddStandardMacros(
    preprocessor, PredefinePlatformMacros ? VTK_PARSE_NATIVE : VTK_PARSE_UNDEF);

  /* add include files specified on the command line */
  for (i = 0; i < NumberOfIncludeDirectories; i++)
  {
    vtkParsePreprocess_IncludeDirectory(preprocessor, IncludeDirectories[i]);
  }

  /* add macros specified on the command line */
  for (i = 0; i < NumberOfDefinitions; i++)
  {
    const char* cp = Definitions[i];

    if (*cp == 'U')
    {
      vtkParsePreprocess_RemoveMacro(preprocessor, &cp[1]);
    }
    else if (*cp == 'D')
    {
      const char* definition = &cp[1];
      while (*definition != '=' && *definition != '\0')
      {
        definition++;
      }
      if (*definition == '=')
      {
        definition++;
      }
      else
      {
        definition = NULL;
      }
      vtkParsePreprocess_AddMacro(preprocessor, &cp[1], definition);
    }
  }

  /* add include files that contain macros to pre-define */
  for (i = 0; i < NumberOfMacroIncludes; i++)
  {
    vtkParsePreprocess_IncludeFile(preprocessor, MacroIncludes[i], VTK_PARSE_CURDIR_INCLUDE);
  }

  data->FileName = vtkstrdup(filename);

  clearComment();

  namespaceDepth = 0;
  currentNamespace = (NamespaceInfo*)malloc(sizeof(NamespaceInfo));
  vtkParse_InitNamespace(currentNamespace);
  data->Contents = currentNamespace;

  templateDepth = 0;
  currentTemplate = NULL;

  currentFunction = (FunctionInfo*)malloc(sizeof(FunctionInfo));
  vtkParse_InitFunction(currentFunction);
  startSig();

  parseDebug = 0;
  if (getenv("DEBUG") != NULL)
  {
    parseDebug = 1;
  }

  yyset_in(ifile);
  yyset_out(errfile);
  ret = yyparse();

  if (ret)
  {
    return NULL;
  }

  free(currentFunction);
  yylex_destroy();

  /* The main class name should match the file name */
  i = strlen(filename);
  j = i;
  while (i > 0)
  {
    --i;
    if (filename[i] == '.')
    {
      j = i;
    }
    if (filename[i] == '/' || filename[i] == '\\')
    {
      i++;
      break;
    }
  }
  main_class = (char*)malloc(j - i + 1);
  strncpy(main_class, &filename[i], j - i);
  main_class[j - i] = '\0';

  /* special treatment of the main class in the file */
  for (i = 0; i < currentNamespace->NumberOfClasses; i++)
  {
    if (strcmp(currentNamespace->Classes[i]->Name, main_class) == 0)
    {
      data->MainClass = currentNamespace->Classes[i];
      break;
    }
  }
  free(main_class);

  /* assign doxygen comments to their targets */
  assignComments(data->Contents);

  /* dump macros, for diagnostic purposes */
  if (DumpMacros)
  {
    dump_macros(DumpFileName);
  }

  vtkParsePreprocess_Free(preprocessor);
  preprocessor = NULL;
  macroName = NULL;

  file_info = data;
  data = NULL;

  return file_info;
}

/* Read a hints file and update the FileInfo */
int vtkParse_ReadHints(FileInfo* file_info, FILE* hfile, FILE* errfile)
{
  char h_cls[512];
  char h_func[512];
  unsigned int h_type, type;
  int h_value;
  FunctionInfo* func_info;
  ClassInfo* class_info;
  NamespaceInfo* contents;
  int i, j;
  int lineno = 0;
  int n;

  contents = file_info->Contents;

  /* read each hint line in succession */
  while ((n = fscanf(hfile, "%s %s %x %i", h_cls, h_func, &h_type, &h_value)) != EOF)
  {
    lineno++;
    if (n < 4)
    {
      fprintf(errfile, "Wrapping: error parsing hints file line %i\n", lineno);
      exit(1);
    }

    /* erase "ref" and qualifiers from hint type */
    type = ((h_type & VTK_PARSE_BASE_TYPE) | (h_type & VTK_PARSE_POINTER_LOWMASK));

    /* find the matching class */
    for (i = 0; i < contents->NumberOfClasses; i++)
    {
      class_info = contents->Classes[i];

      if (strcmp(h_cls, class_info->Name) == 0)
      {
        /* find the matching function */
        for (j = 0; j < class_info->NumberOfFunctions; j++)
        {
          func_info = class_info->Functions[j];

          if ((strcmp(h_func, func_info->Name) == 0) && func_info->ReturnValue &&
            (type ==
              ((func_info->ReturnValue->Type & ~VTK_PARSE_REF) & VTK_PARSE_UNQUALIFIED_TYPE)))
          {
            /* types that hints are accepted for */
            switch (func_info->ReturnValue->Type & VTK_PARSE_UNQUALIFIED_TYPE)
            {
              case VTK_PARSE_FLOAT_PTR:
              case VTK_PARSE_VOID_PTR:
              case VTK_PARSE_DOUBLE_PTR:
              case VTK_PARSE_LONG_LONG_PTR:
              case VTK_PARSE_UNSIGNED_LONG_LONG_PTR:
              case VTK_PARSE___INT64_PTR:
              case VTK_PARSE_UNSIGNED___INT64_PTR:
              case VTK_PARSE_INT_PTR:
              case VTK_PARSE_UNSIGNED_INT_PTR:
              case VTK_PARSE_SHORT_PTR:
              case VTK_PARSE_UNSIGNED_SHORT_PTR:
              case VTK_PARSE_LONG_PTR:
              case VTK_PARSE_UNSIGNED_LONG_PTR:
              case VTK_PARSE_SIGNED_CHAR_PTR:
              case VTK_PARSE_UNSIGNED_CHAR_PTR:
              case VTK_PARSE_CHAR_PTR:
              {
                if (func_info->ReturnValue->NumberOfDimensions == 0)
                {
                  char text[64];
                  sprintf(text, "%i", h_value);
                  func_info->ReturnValue->Count = h_value;
                  vtkParse_AddStringToArray(&func_info->ReturnValue->Dimensions,
                    &func_info->ReturnValue->NumberOfDimensions,
                    vtkParse_CacheString(file_info->Strings, text, strlen(text)));
#ifndef VTK_PARSE_LEGACY_REMOVE
                  func_info->HaveHint = 1;
                  func_info->HintSize = h_value;
#endif
                }
                break;
              }
              default:
              {
                fprintf(errfile, "Wrapping: unhandled hint type %#x\n", h_type);
              }
            }
          }
        }
      }
    }
  }

  return 1;
}

/* Free any caches or buffers, call just before program exits */
void vtkParse_FinalCleanup(void)
{
  vtkParse_FreeFileCache(&system_cache);
  vtkParse_FreeStringCache(&system_strings);
}

/* Free the FileInfo struct returned by vtkParse_ParseFile() */
void vtkParse_Free(FileInfo* file_info)
{
  vtkParse_FreeFile(file_info);
  // system_strings will be released at program exit
  if (file_info->Strings && file_info->Strings != &system_strings)
  {
    vtkParse_FreeStringCache(file_info->Strings);
    free(file_info->Strings);
  }
  free(file_info);
}

/** Define a preprocessor macro. Function macros are not supported.  */
void vtkParse_DefineMacro(const char* name, const char* definition)
{
  size_t n = vtkParse_SkipId(name);
  size_t l;
  char* cp;

  if (definition == NULL)
  {
    definition = "";
  }

  l = n + strlen(definition) + 2;
  cp = (char*)malloc(l + 1);
  cp[0] = 'D';
  strncpy(&cp[1], name, n);
  cp[n + 1] = '\0';
  if (definition[0] != '\0')
  {
    cp[n + 1] = '=';
    strcpy(&cp[n + 2], definition);
  }
  cp[l] = '\0';

  vtkParse_AddStringToArray(&Definitions, &NumberOfDefinitions, cp);
}

/** Undefine a preprocessor macro.  */
void vtkParse_UndefineMacro(const char* name)
{
  size_t n = vtkParse_SkipId(name);
  char* cp;

  cp = (char*)malloc(n + 2);
  cp[0] = 'U';
  strncpy(&cp[1], name, n);
  cp[n + 1] = '\0';

  vtkParse_AddStringToArray(&Definitions, &NumberOfDefinitions, cp);
}

/** Do not define any platform-specific macros.  */
void vtkParse_UndefinePlatformMacros()
{
  PredefinePlatformMacros = 0;
}

/** Add an include file to read macros from, for use with -imacro. */
void vtkParse_IncludeMacros(const char* filename)
{
  size_t n = strlen(filename);
  char* cp;

  cp = (char*)malloc(n + 1);
  strcpy(cp, filename);

  vtkParse_AddStringToArray(&MacroIncludes, &NumberOfMacroIncludes, cp);
}

/** Dump macros to the specified file (stdout if NULL). */
void vtkParse_DumpMacros(const char* filename)
{
  DumpMacros = 1;
  DumpFileName = filename;
}

/** Add an include directory, for use with the "-I" option.  */
void vtkParse_IncludeDirectory(const char* dirname)
{
  size_t n = strlen(dirname);
  char* cp;
  int i;

  for (i = 0; i < NumberOfIncludeDirectories; i++)
  {
    if (strncmp(IncludeDirectories[i], dirname, n) == 0 && IncludeDirectories[i][n] == '\0')
    {
      return;
    }
  }

  cp = (char*)malloc(n + 1);
  strcpy(cp, dirname);

  vtkParse_AddStringToArray(&IncludeDirectories, &NumberOfIncludeDirectories, cp);
}

/** Return the full path to a header file.  */
const char* vtkParse_FindIncludeFile(const char* filename)
{
  static StringCache string_cache = { 0, 0, 0, 0 };
  static PreprocessInfo info = { 0, 0, 0, 0, 0, 0, &string_cache, 0, 0, 0, 0, 0, 0, &system_cache };
  int val;
  int i;

  /* add include files specified on the command line */
  for (i = 0; i < NumberOfIncludeDirectories; i++)
  {
    vtkParsePreprocess_IncludeDirectory(&info, IncludeDirectories[i]);
  }

  return vtkParsePreprocess_FindIncludeFile(&info, filename, VTK_PARSE_SOURCE_INCLUDE, &val);
}
