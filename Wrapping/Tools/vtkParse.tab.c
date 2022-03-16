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
#define YYLAST 6600

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS 125
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS 277
/* YYNRULES -- Number of rules.  */
#define YYNRULES 675
/* YYNRULES -- Number of states.  */
#define YYNSTATES 1054
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
  3033, 3034, 3035, 3036, 3039, 3040, 3041, 3042, 3043, 3044, 3045, 3046, 3047, 3048, 3049, 3050,
  3051, 3052, 3053, 3073, 3074, 3075, 3076, 3079, 3083, 3087, 3087, 3091, 3092, 3107, 3108, 3133,
  3133, 3137, 3137, 3141, 3141, 3145, 3145, 3149, 3149, 3153, 3153, 3156, 3157, 3160, 3164, 3165,
  3168, 3171, 3172, 3173, 3174, 3177, 3177, 3181, 3182, 3185, 3186, 3189, 3190, 3197, 3198, 3199,
  3200, 3201, 3202, 3203, 3204, 3205, 3206, 3207, 3208, 3211, 3212, 3213, 3214, 3215, 3216, 3217,
  3218, 3219, 3220, 3221, 3222, 3223, 3224, 3225, 3226, 3227, 3228, 3229, 3230, 3231, 3232, 3233,
  3234, 3235, 3236, 3237, 3238, 3239, 3240, 3241, 3242, 3243, 3244, 3247, 3248, 3249, 3250, 3251,
  3252, 3253, 3254, 3255, 3256, 3257, 3258, 3259, 3260, 3261, 3262, 3263, 3264, 3265, 3266, 3267,
  3268, 3269, 3270, 3271, 3272, 3273, 3274, 3275, 3276, 3279, 3280, 3281, 3282, 3283, 3284, 3285,
  3286, 3287, 3294, 3295, 3298, 3299, 3300, 3301, 3301, 3302, 3305, 3306, 3309, 3310, 3311, 3312,
  3348, 3348, 3349, 3350, 3351, 3352, 3354, 3355, 3358, 3359, 3360, 3361, 3364, 3365, 3366, 3369,
  3370, 3372, 3373, 3375, 3376, 3379, 3380, 3383, 3384, 3385, 3389, 3388, 3402, 3403, 3406, 3406,
  3408, 3408, 3412, 3412, 3414, 3414, 3416, 3416, 3420, 3420, 3425, 3426, 3428, 3429, 3432, 3433,
  3436, 3437, 3440, 3441, 3442, 3443, 3444, 3445, 3446, 3447, 3447, 3447, 3447, 3447, 3448, 3449,
  3450, 3451, 3452, 3455, 3458, 3459, 3462, 3465, 3465, 3465 };
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

#define YYPACT_NINF -878
#define YYTABLE_NINF -629

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const short yypact[] = { -878, 93, 232, -878, -878, 1545, -878, 410, 424, 448, 526, 27, 550,
  560, 201, 216, 265, -878, -878, -878, 161, -878, -878, -878, -878, -878, -878, -878, 196, -878,
  76, -878, 3534, -878, -878, 6179, 609, 1173, -878, -878, -878, -878, -878, -878, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, -878, -878, 188, -878, -878, -878, -878, -878, -878, 5882,
  -878, 235, 235, 235, 235, -878, 251, 6179, -878, 215, -878, 219, 1349, 1553, 311, 1415, 136, 254,
  -878, 194, 5981, -878, -878, -878, -878, 130, 131, -878, -878, -878, -878, -878, 391, -878, -878,
  1182, 320, 3900, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, -878, 40, -878, -878, -878, -878, -878, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, 69,
  1415, 72, 83, 166, 168, -878, 186, 202, -878, 420, -878, -878, -878, -878, -878, 1688, 311, 311,
  6179, 130, -878, -878, -878, -878, -878, -878, -878, 343, 72, 83, 166, 168, 186, 202, -878, -878,
  -878, 1415, 1415, 389, 402, -878, 1349, 1415, 311, 311, 6401, 357, 5424, -878, 6401, -878, 158,
  369, 1415, -878, -878, -878, -878, -878, -878, 5882, -878, -878, 6080, 130, 399, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, 6179, -878, -878, -878, -878, -878, -878, 324, 400, 311, 311,
  311, -878, -878, -878, -878, 194, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878,
  -878, -878, -878, 1349, -878, -878, -878, -878, -878, -878, 1713, -878, 314, 273, -878, -878,
  -878, -878, -878, -878, -878, -878, 122, -878, -878, -878, 46, -878, 412, -878, -878, 1948, 2070,
  -878, -878, 300, -878, 2192, 3046, 2314, -878, -878, -878, -878, -878, -878, 6485, 5622, 6485,
  1404, -878, -878, -878, -878, -878, -878, 1600, -878, 2436, 768, 413, -878, 415, -878, 414, -878,
  -878, -878, 5242, 1349, -878, -878, 427, -878, 130, -878, -878, -878, -878, -878, -878, 123, -878,
  1465, 838, 311, 311, 391, 428, 1270, -878, -878, -878, 353, -878, 1415, 6080, 1713, 1415, 435,
  2558, 432, 1516, 1182, -878, -878, -878, 69, -878, -878, -878, -878, -878, 6401, 5622, 6401, 1404,
  -878, -878, -878, -878, 371, -878, 454, -878, 5761, -878, 454, 441, -878, 1349, 187, -878, -878,
  -878, 449, 459, 1600, -878, 453, 130, -878, -878, -878, -878, -878, -878, 5860, 701, 469, 261,
  465, -878, 1182, -878, 479, 3168, -878, -878, 472, -878, -878, -878, -878, 39, -878, 6278, 152,
  542, -878, -878, -878, -878, -878, -878, -878, -878, -878, 487, -878, 130, 98, 490, 291, 6485,
  6485, 226, 277, -878, -878, -878, -878, 492, 311, -878, -878, -878, 391, 592, 486, 489, 57, -878,
  -878, 493, -878, 491, -878, -878, -878, -878, -878, -878, 499, -878, -878, 301, 1292, -878, -878,
  503, -878, -878, 311, 311, 1465, -878, -878, -878, -878, -878, -878, -878, -878, 358, -878, -878,
  6179, 508, -878, -878, 1349, 512, -878, 120, -878, -878, 513, 537, -878, 311, -878, -878, -878,
  432, 4632, 521, 155, 522, 353, 5860, -878, 371, -878, -878, -878, -878, 32, -878, 517, -878, -878,
  -878, 518, 159, -878, -878, -878, -878, -878, 4876, -878, -878, 1130, -878, -878, 391, 371, 525,
  -878, 524, 465, 269, 311, -878, 556, 69, 539, -878, -878, -878, -878, -878, 1415, 1415, 1415,
  -878, 311, 311, 6179, 130, 131, -878, -878, -878, -878, 130, 152, -878, 4022, 4144, 4266, -878,
  534, -878, -878, -878, 544, 547, -878, 131, -878, 538, -878, 546, 6179, -878, 540, 543, -878,
  -878, -878, -878, -878, -878, -878, -878, -878, 557, -878, -878, -878, 440, 558, -878, 629, 583,
  -878, -878, -878, -878, 1270, 564, -878, -878, 4, 1415, 583, 583, 2680, -878, -878, 565, -878,
  -878, -878, 670, 391, 571, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, -878, 572, -878, -878,
  -878, 324, -878, -878, 518, -878, 509, -878, 573, 131, -878, 4754, -878, 4876, -878, -878, -878,
  -878, 404, -878, 316, -878, -878, -878, -878, 1182, -878, -878, -878, -878, 300, -878, -878, -878,
  -878, -878, 1600, -878, -878, -878, -878, -878, 130, -878, -878, -878, -878, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, -878, 432, -878, 130, -878, -878, 5363, -878, 1415, -878,
  -878, -878, 1415, -878, 1292, -878, -878, -878, -878, 580, -878, -878, -878, -878, -878, 454, 601,
  6179, -878, -878, 399, -878, -878, -878, -878, -878, -878, 432, 586, -878, -878, -878, -878, -878,
  -878, 432, -878, 5120, -878, 3656, -878, -878, -878, -878, -878, -878, -878, -878, -878, 325,
  -878, 588, 273, 5860, 588, -878, 587, 598, -878, 172, 701, -878, -878, -878, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, 5463, -878, 235, -878, -878, -878, 599, 400, 1349, 5562, 130,
  583, 1292, 583, 558, 4876, 3778, -878, 655, -878, -878, -878, 130, -878, 4388, 4632, 4510, 646,
  605, 602, 4876, 611, -878, -878, -878, -878, -878, 4876, 432, 5860, -878, -878, -878, -878, -878,
  616, 130, -878, 588, -878, -878, 5661, -878, -878, -878, -878, 5463, -878, -878, 400, 5760, -878,
  -878, -878, -878, 1349, 1713, -878, -878, -878, 4876, 105, -878, -878, 623, 617, -878, -878, -878,
  -878, -878, -878, -878, 4876, -878, 4876, 622, 4998, -878, -878, -878, -878, -878, -878, -878,
  1887, 235, 5760, 583, 5760, 630, -878, -878, 632, 314, 79, -878, -878, 6377, 66, -878, -878, -878,
  4998, -878, 345, 523, 1800, -878, -878, 1887, -878, -878, 1713, -878, 631, -878, -878, -878, -878,
  -878, 6377, -878, -878, 131, -878, 391, -878, -878, -878, -878, -878, 105, 112, -878, -878, 246,
  -878, 1800, -878, 1770, -878, 2802, -878, -878, -878, 523, -878, -878, 2924, 3290, 377, 67, 1770,
  296, -878, -878, -878, 5860, -878, -878, -878, -878, 74, 377, 5860, 3412, -878, -878 };

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] = { 3, 0, 4, 1, 473, 0, 485, 440, 441, 442, 437, 369, 438,
  439, 444, 445, 443, 53, 52, 54, 114, 400, 401, 392, 395, 396, 398, 399, 397, 391, 393, 218, 0,
  361, 414, 0, 0, 0, 358, 446, 447, 448, 449, 450, 455, 456, 457, 458, 451, 452, 453, 454, 459, 460,
  22, 356, 5, 19, 20, 13, 11, 12, 9, 37, 17, 380, 44, 483, 10, 16, 380, 0, 483, 14, 135, 7, 6, 8, 0,
  18, 0, 0, 0, 0, 207, 0, 0, 15, 0, 338, 473, 0, 0, 0, 0, 473, 413, 340, 357, 0, 473, 388, 389, 390,
  179, 293, 405, 409, 412, 473, 473, 474, 116, 115, 0, 394, 0, 440, 441, 442, 437, 438, 439, 674,
  675, 585, 580, 581, 582, 579, 583, 584, 586, 587, 444, 445, 443, 644, 552, 551, 553, 572, 555,
  557, 556, 558, 559, 560, 561, 564, 565, 563, 562, 568, 571, 554, 573, 574, 566, 550, 549, 570,
  569, 525, 526, 567, 577, 576, 575, 578, 527, 528, 529, 658, 530, 531, 532, 538, 539, 533, 534,
  535, 536, 537, 540, 541, 542, 543, 544, 545, 546, 547, 548, 656, 655, 668, 644, 662, 659, 663,
  673, 164, 522, 644, 521, 516, 661, 515, 517, 518, 519, 520, 523, 524, 660, 667, 666, 657, 664,
  665, 646, 652, 654, 653, 644, 0, 0, 440, 441, 442, 437, 379, 438, 439, 397, 393, 380, 483, 380,
  483, 473, 0, 473, 413, 0, 179, 373, 375, 374, 378, 377, 376, 644, 33, 365, 363, 364, 368, 367,
  366, 372, 371, 370, 0, 0, 0, 475, 339, 0, 0, 341, 342, 293, 0, 51, 485, 293, 110, 117, 0, 0, 26,
  38, 23, 483, 25, 27, 0, 24, 28, 0, 179, 257, 246, 644, 189, 245, 191, 192, 190, 210, 483, 0, 213,
  21, 417, 354, 197, 195, 225, 345, 0, 341, 342, 343, 59, 344, 58, 0, 348, 346, 347, 349, 416, 350,
  359, 380, 483, 380, 483, 136, 208, 0, 473, 407, 386, 301, 303, 180, 0, 289, 274, 179, 477, 477,
  477, 404, 294, 461, 462, 471, 463, 380, 436, 435, 495, 486, 0, 3, 646, 0, 0, 631, 630, 170, 162,
  0, 0, 0, 638, 640, 636, 362, 473, 394, 293, 51, 293, 117, 345, 380, 380, 151, 147, 143, 0, 146, 0,
  0, 0, 154, 0, 152, 0, 485, 156, 155, 0, 0, 385, 384, 0, 289, 179, 473, 382, 383, 62, 40, 49, 410,
  473, 0, 0, 59, 0, 484, 0, 122, 106, 118, 113, 473, 475, 0, 0, 0, 0, 0, 0, 264, 0, 0, 229, 228,
  479, 227, 255, 351, 352, 353, 619, 293, 51, 293, 117, 198, 196, 387, 380, 469, 209, 221, 475, 0,
  193, 221, 327, 475, 0, 0, 276, 286, 275, 0, 0, 0, 317, 0, 179, 466, 485, 465, 467, 464, 472, 406,
  0, 0, 495, 489, 492, 0, 3, 4, 0, 649, 651, 0, 645, 648, 650, 669, 0, 167, 0, 0, 0, 473, 670, 30,
  647, 672, 608, 608, 608, 415, 0, 143, 179, 410, 0, 473, 293, 293, 0, 327, 475, 341, 342, 32, 0, 0,
  3, 159, 160, 476, 0, 525, 526, 0, 510, 509, 0, 507, 0, 508, 217, 514, 158, 157, 42, 288, 292, 381,
  63, 0, 61, 39, 48, 57, 473, 59, 0, 0, 108, 365, 363, 364, 368, 369, 367, 366, 0, 120, 475, 0, 112,
  411, 473, 0, 258, 259, 0, 644, 244, 0, 473, 410, 0, 233, 485, 226, 264, 0, 0, 410, 0, 473, 408,
  402, 470, 302, 223, 224, 214, 230, 222, 0, 219, 298, 328, 0, 321, 199, 194, 475, 285, 290, 0, 642,
  279, 0, 299, 318, 478, 469, 0, 153, 0, 488, 495, 501, 357, 497, 499, 4, 31, 29, 671, 168, 165, 0,
  0, 0, 431, 430, 429, 0, 179, 293, 424, 428, 181, 182, 179, 0, 163, 0, 0, 0, 138, 142, 145, 140,
  112, 0, 0, 137, 293, 148, 321, 36, 4, 0, 513, 0, 0, 512, 511, 503, 504, 66, 67, 68, 45, 473, 0,
  102, 103, 104, 100, 50, 93, 98, 179, 46, 55, 473, 111, 122, 123, 119, 105, 340, 0, 179, 179, 0,
  211, 270, 265, 266, 271, 355, 252, 480, 0, 634, 597, 626, 602, 627, 628, 632, 603, 607, 606, 601,
  604, 605, 624, 596, 625, 620, 623, 360, 598, 599, 600, 43, 41, 109, 112, 403, 232, 231, 225, 215,
  333, 330, 331, 0, 250, 0, 293, 595, 592, 593, 277, 588, 590, 591, 621, 0, 282, 304, 468, 490, 487,
  494, 0, 498, 496, 500, 35, 170, 473, 432, 433, 434, 426, 319, 171, 477, 423, 380, 174, 179, 613,
  615, 616, 639, 611, 612, 610, 614, 609, 641, 637, 139, 141, 144, 264, 34, 179, 505, 506, 0, 65, 0,
  101, 473, 99, 0, 95, 0, 56, 121, 124, 646, 0, 128, 260, 262, 261, 248, 221, 267, 0, 235, 234, 257,
  256, 608, 619, 608, 107, 479, 264, 336, 332, 324, 325, 326, 323, 322, 264, 291, 0, 589, 0, 283,
  281, 305, 300, 308, 502, 169, 166, 380, 304, 320, 183, 179, 425, 183, 177, 0, 0, 473, 393, 0, 82,
  80, 71, 77, 64, 79, 73, 72, 76, 74, 69, 70, 0, 78, 0, 204, 205, 75, 0, 338, 0, 0, 179, 179, 0,
  179, 47, 0, 127, 126, 247, 212, 269, 473, 179, 253, 0, 0, 0, 240, 0, 0, 0, 0, 594, 618, 643, 617,
  622, 0, 264, 427, 295, 185, 172, 184, 315, 0, 179, 175, 183, 149, 161, 0, 83, 85, 88, 86, 0, 84,
  87, 0, 0, 200, 81, 475, 206, 0, 0, 96, 94, 97, 125, 0, 268, 272, 236, 0, 629, 633, 242, 233, 241,
  216, 481, 337, 251, 284, 0, 0, 296, 316, 178, 309, 91, 483, 89, 0, 0, 0, 179, 0, 0, 475, 203, 0,
  274, 0, 254, 635, 0, 236, 334, 485, 306, 186, 187, 304, 150, 0, 483, 90, 0, 92, 483, 0, 201, 0,
  644, 273, 239, 237, 238, 0, 419, 243, 293, 220, 482, 309, 188, 297, 311, 310, 0, 314, 644, 646,
  410, 131, 0, 483, 0, 202, 0, 421, 380, 418, 479, 312, 313, 0, 0, 0, 60, 0, 410, 132, 249, 380,
  420, 307, 646, 134, 129, 60, 0, 422, 0, 130, 133 };

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] = { -878, -878, -292, -878, -878, 731, -61, -878, -878, -878, -878,
  -723, -67, -2, -34, -878, -878, -878, -878, 18, -304, -66, -658, -878, -878, -878, -878, -57, -56,
  -63, -142, -878, -878, 65, -51, -50, -25, -878, -878, 15, -361, -878, -878, 62, -878, -878, -878,
  -216, -624, -43, -77, -319, 248, 99, -878, -878, -878, -878, 250, -41, 284, -878, 0, -878, -3,
  -878, -878, -878, -878, -878, 2, -878, -878, -878, -878, -878, -878, 781, 117, -773, -878, -878,
  -878, 759, -878, -878, -878, -29, -160, 35, -78, -878, -878, -206, -395, -878, -878, -256, -254,
  -434, -422, -878, -878, 43, -878, -878, -184, -878, -202, -878, -878, -878, -74, -878, -878, -878,
  -878, -52, -878, -878, -878, -878, -39, -878, 89, -527, -878, -878, -878, -109, -878, -878, -191,
  -878, -878, -878, -878, -878, -878, 1, 386, -245, 388, -878, 47, -62, -543, -878, -190, -878,
  -561, -878, -794, -878, -878, -218, -878, -878, -878, -337, -878, -878, -362, -878, -878, 61,
  -878, -878, -878, 1056, 885, 1068, 71, -878, -878, 195, 405, -5, -878, 33, -878, 208, -32, -38,
  -878, 11, 793, -878, -878, -401, -878, 19, 244, -878, -878, -84, -877, -878, -878, -878, -878,
  -878, -878, -878, -878, -878, -878, 323, 176, 213, -323, 460, -878, 470, -878, 207, -878, 992,
  -878, -417, -878, -293, -878, -781, -878, -878, -878, -54, -878, -247, -878, -878, -878, 348, 212,
  -878, -878, -878, -878, -878, 106, 209, 179, -535, -690, -878, -209, -11, -460, -878, -12, -878,
  5, -878, -870, -878, -513, -878, -456, -878, -878, -878, -196, -878, -878, -878, 370, -878, -161,
  -342, -878, -340, 58, -506, -878, -540, -878 };

/* YYDEFGOTO[NTERM-NUM].  */
static const short yydefgoto[] = { -1, 1, 2, 4, 56, 281, 58, 59, 60, 389, 61, 62, 63, 283, 65, 273,
  66, 806, 546, 301, 410, 411, 549, 545, 676, 677, 867, 928, 929, 682, 683, 804, 800, 684, 68, 69,
  70, 418, 71, 284, 421, 566, 563, 564, 890, 285, 811, 969, 1022, 73, 74, 507, 515, 508, 382, 383,
  793, 966, 384, 75, 265, 76, 286, 664, 287, 498, 364, 767, 493, 766, 494, 495, 853, 496, 856, 497,
  923, 772, 645, 917, 918, 962, 988, 288, 80, 81, 82, 932, 877, 878, 84, 430, 817, 85, 451, 452,
  829, 453, 86, 455, 595, 596, 597, 435, 436, 737, 705, 821, 981, 954, 955, 983, 295, 296, 893, 456,
  837, 879, 822, 949, 309, 583, 428, 571, 572, 576, 577, 701, 896, 702, 819, 979, 462, 463, 609,
  464, 465, 754, 912, 289, 340, 401, 460, 745, 402, 403, 773, 990, 341, 756, 342, 450, 845, 913,
  1012, 991, 920, 468, 851, 457, 836, 600, 846, 602, 740, 741, 830, 904, 905, 685, 89, 240, 241,
  432, 92, 93, 94, 270, 441, 271, 225, 97, 98, 272, 404, 302, 100, 101, 102, 103, 591, 885, 105,
  352, 449, 106, 107, 226, 1008, 1009, 1029, 1042, 639, 640, 776, 850, 641, 108, 109, 110, 347, 348,
  349, 350, 616, 592, 351, 568, 6, 393, 394, 470, 471, 580, 581, 985, 986, 274, 275, 111, 356, 478,
  479, 480, 481, 482, 763, 624, 625, 537, 718, 719, 720, 749, 750, 839, 751, 722, 648, 786, 787,
  911, 584, 841, 723, 724, 752, 825, 365, 727, 826, 824, 728, 505, 503, 504, 729, 753, 360, 367,
  489, 490, 491, 221, 222, 223, 224 };

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const short yytable[] = { 95, 236, 78, 64, 253, 77, 87, 297, 298, 299, 238, 282, 396, 307,
  397, 334, 99, 485, 278, 469, 72, 331, 330, 67, 104, 605, 314, 501, 416, 372, 242, 417, 276, 697,
  599, 361, 598, 631, 96, 308, 83, 603, 366, 346, 649, 650, 244, 902, 516, 590, 239, 472, 473, 237,
  245, 386, 707, 915, 511, 840, 318, 567, 395, 368, 604, 326, 484, 696, 243, 820, 547, 725, 510,
  242, 328, 865, 91, 547, 774, 369, 370, 242, 357, 922, 774, 454, 312, 315, 115, 290, 220, 387, 989,
  3, 242, 601, 67, 291, 1003, 477, 661, 547, 777, 362, 362, 303, 1007, 196, 268, 810, 735, 243,
  -379, 83, 329, 123, 124, 327, 1013, 243, 116, 668, 123, 124, 313, 316, 547, 578, 726, 1028, 123,
  124, 243, 429, 1004, 1005, 605, -379, 217, 586, 336, 337, 540, 736, 397, 501, 630, 526, 691, 91,
  965, 655, 467, -491, 601, 363, 363, -373, 547, -491, 334, 254, 255, 256, 257, 11, 258, 259, -375,
  548, 332, 980, 454, 669, 359, 604, 731, 371, 643, 260, 261, 262, -373, 376, 374, 378, 112, 113,
  812, 744, 734, 626, 539, -375, 656, 657, 1014, 585, 375, 587, 377, -60, 644, 967, -60, 454, 334,
  33, 215, 343, 380, 219, 331, 200, 338, 590, 116, 774, 334, 381, 200, 38, 217, 196, 615, 548, -60,
  733, 200, -60, 424, 663, -2, 344, 774, 857, 312, 345, -341, 242, 582, 218, 344, 38, -341, 216,
  345, 431, 114, 547, 1044, -374, 859, -378, 446, 840, 326, 731, -60, 239, 419, -60, 237, 312, 420,
  742, 858, 743, 840, 414, 840, -377, 55, 443, 313, 445, -374, 243, -378, 614, 447, 292, 242, 336,
  337, 242, 311, -376, 442, 607, 444, 277, 215, 774, 608, 219, -377, 242, 425, 547, 317, 313, 903,
  -372, 325, 237, 291, 415, 327, -372, 906, 919, -376, 509, 476, 509, 454, 304, -371, 386, 243,
  -339, 83, 243, -371, 218, -173, 312, 658, 216, 672, 673, 674, 659, 706, 243, 774, 293, 294, 38,
  398, 334, 513, 514, -176, 412, 551, 331, 548, -60, -173, 343, -60, 318, 948, 945, -342, 338, 91,
  300, 774, 332, -342, 1036, 899, 313, 901, -370, -491, 433, 958, 414, 434, -370, -491, 721, -493,
  960, 964, 1036, 518, 1049, -493, 522, 344, 660, 961, 725, 345, -329, 1049, 326, 242, 312, -117,
  894, 38, 420, 731, -60, 318, 1040, -60, 21, 22, 675, 978, 552, 522, 415, 459, 589, 1047, 698,
  1040, 355, 303, -278, 519, 461, 242, 415, -280, 358, 844, 1052, 414, 579, -329, 764, 243, 313,
  374, 844, 318, 852, 311, -329, 414, 785, 785, 785, 269, 237, 951, 553, 415, 412, 388, 725, 623,
  509, 509, 844, 312, -117, -117, -329, 243, 420, 407, 518, 689, 311, 415, 690, 217, 217, 891, 678,
  679, 680, 217, 217, 217, 725, 415, 1032, 1033, 201, 1019, 854, 317, 1020, 554, 406, 593, 594, 688,
  406, 636, 392, 313, 217, 646, -373, 332, 570, 319, 519, 322, 324, 412, 538, -153, 427, 638, 977,
  437, -375, 634, 842, -365, 843, 412, 483, 523, 524, -365, 525, -373, 975, 832, 833, 834, 835,
  -363, 311, 637, 541, 555, 921, -363, -374, -375, 217, 215, 215, 574, 219, 219, -263, 215, 215,
  215, 219, 219, 219, 268, 552, -364, 501, 610, -329, 771, 1015, -364, 1016, -374, 1000, 538, 242,
  613, 215, 977, 312, 219, 612, 1017, 218, 218, 1018, 621, 216, 216, 218, 218, 218, 775, 216, 216,
  216, 619, 627, 317, 629, 279, 553, 406, 651, 406, 217, 654, 311, 662, 665, 218, 509, 666, 243,
  216, 667, 670, 313, 671, 215, 608, 311, 219, 686, 693, -378, 246, 247, 248, 249, 231, 250, 251,
  695, 317, 700, 699, 730, 732, 738, 565, 938, -368, 454, 739, 758, 785, 242, -368, -377, -378, 759,
  218, 317, 762, 659, 216, 765, 742, -376, 790, 322, 324, 791, 794, 317, -367, 796, 311, 406, 797,
  406, -367, 242, -377, 215, -366, 799, 219, 802, 803, 338, -366, 809, -376, 243, 818, 820, 827,
  322, 324, 622, 1035, 413, 823, 743, 892, 795, 976, 895, 454, 454, 1021, 406, 785, 721, 785, 916,
  218, 501, 924, 243, 216, -335, 925, 937, 946, 1051, 254, 255, 256, 257, 11, 258, 259, 501, 252,
  953, 956, 957, 438, 439, 440, 1037, 959, 1039, 260, 261, 262, 963, 406, 406, 980, 623, 982, 987,
  999, 1026, 1048, 1001, 57, 864, 855, 1011, 318, 868, 869, 889, 454, 943, 801, 870, 871, 311, 33,
  263, 808, 721, 994, 872, 652, 873, 792, 653, 716, 617, 317, 778, 79, 311, 971, 518, 848, 876, 984,
  246, 247, 248, 249, 231, 250, 251, 694, 721, 828, 413, 1010, 898, 813, 716, 38, 947, 1002, 543,
  542, 755, 838, 95, 1031, 875, 866, 717, 874, 880, 334, 831, 406, 936, 217, 519, 331, 330, 282,
  884, 692, 474, 914, 72, 242, 55, 67, 33, 642, 322, 324, 475, 717, 757, 716, 716, 716, 620, 907,
  910, 900, 96, 939, 760, 611, 0, 413, 0, 897, 0, 1027, 246, 247, 248, 249, 231, 250, 251, 413, 0,
  326, 334, 243, 38, 0, 0, 0, 331, 1034, 328, 0, 717, 717, 717, 0, 0, 0, 0, 0, 883, 95, 0, 931, 927,
  215, 930, 933, 219, 312, 95, 0, 0, 269, 939, 0, 339, 935, 33, 334, 0, 334, 0, 333, 67, 331, 0,
  331, 996, 565, 329, 0, 326, 327, 282, 0, 0, 0, 218, 96, 318, 0, 216, 0, 0, 992, 0, 313, 96, 0, 0,
  939, 95, 939, 970, 0, 716, 95, 322, 931, 927, 95, 930, 933, 268, 0, 312, 518, 972, 0, 1023, 326,
  0, 974, 1025, 968, 973, 883, 1030, 0, 237, 0, 0, 0, 327, 0, 940, 847, 439, 440, 96, 0, 0, 717, 0,
  96, 0, 318, 95, 96, 95, 1038, 0, 0, 313, 519, 0, 310, 0, 242, 320, 0, 995, 0, 0, 704, 0, 0, 414,
  993, 0, 997, 1043, 237, 518, 1006, 0, 5, 217, 0, 0, 0, 242, 883, 0, 1050, 96, 940, 96, 0, 0, 0, 0,
  0, 1024, 0, 716, 243, 716, 414, 0, 414, 0, 0, 0, 0, 415, 385, 761, 0, 0, 0, 519, 0, 414, 0, 0, 0,
  333, 0, 243, 0, 322, 324, 0, 0, 940, 0, 0, 0, 0, 0, 0, 717, 0, 717, 0, 415, 0, 415, 0, 0, 88, 0,
  412, 0, 405, 716, 215, 406, 405, 219, 415, 426, 90, 0, 716, 716, 716, 311, 0, 0, 716, 305, 333, 0,
  0, 0, 323, 716, 0, 0, 0, 335, 266, 412, 333, 412, 0, 0, 0, 218, 353, 354, 717, 216, 267, 0, 0, 0,
  412, 0, 0, 717, 717, 717, 0, 0, 0, 717, 0, 0, 716, 0, 466, 0, 717, 379, 406, 0, 0, 0, 0, 0, 217,
  716, 311, 716, 0, 716, 0, 217, 217, 125, 126, 127, 128, 129, 130, 131, 132, 133, 0, 0, 379, 320,
  0, 0, 217, 717, 0, 0, 0, 321, 0, 716, 0, 0, 0, 405, 0, 405, 717, 0, 717, 0, 717, 254, 255, 256,
  257, 11, 258, 259, 0, 466, 246, 247, 248, 249, 231, 250, 251, 0, 0, 260, 261, 262, 0, 0, 0, 317,
  717, 0, 215, 0, 0, 219, 0, 0, 0, 215, 215, 0, 219, 219, 0, 0, 310, 333, 0, 0, 0, 33, 263, 458,
  264, 0, 215, 0, 0, 219, 317, 305, 317, 323, 405, 218, 405, 0, 0, 216, 0, 0, 218, 218, 0, 317, 216,
  216, 0, 466, 0, 0, 0, 0, 0, 406, 0, 38, 0, 218, 0, 0, 406, 216, 0, 0, 269, 0, 405, 517, 0, 0, 556,
  557, 558, 559, 560, 561, 562, 0, 0, 373, 379, 0, 0, 0, 55, 0, 0, 385, 260, 261, 262, 267, 254,
  255, 256, 257, 11, 258, 259, 0, 0, 0, 0, 405, 405, 0, 0, 0, 458, 0, 260, 261, 262, 0, 0, 0, 390,
  391, 678, 679, 680, 0, 448, 0, 0, 0, 681, 0, 267, 267, 0, 422, 0, 423, 321, 0, 0, 0, 33, 0, 0,
  606, 0, 267, 0, 267, 0, 0, 517, 254, 255, 256, 257, 11, 258, 259, 0, 0, 0, 0, 0, 0, 506, 0, 0, 0,
  0, 260, 261, 262, 0, 0, 0, 0, 38, 0, 0, 0, 0, 405, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 306, 0, 0, 544,
  413, 33, 0, 0, 0, 0, 550, 0, 0, 55, 254, 255, 256, 257, 11, 258, 259, 0, 0, 0, 0, 254, 255, 256,
  257, 11, 258, 259, 260, 261, 262, 413, 0, 413, 333, 0, 0, 512, 0, 260, 261, 262, 0, 0, 0, 0, 413,
  0, 0, 267, 0, 0, 0, 0, 0, 0, 33, 0, 606, 521, 0, 0, 0, 0, 0, 0, 55, 33, 0, 805, 0, 254, 255, 256,
  257, 11, 258, 259, 0, 0, 814, 815, 0, 569, 0, 0, 573, 0, 0, 260, 261, 262, 38, 647, 0, 267, 0, 0,
  267, 0, 0, 0, 0, 38, 588, 0, 0, 0, 0, 0, 0, 0, 306, 420, 0, 0, 267, 33, 0, 0, 0, 55, 254, 255,
  256, 257, 11, 258, 259, 0, 0, 0, 55, 0, 0, 0, 618, 0, 0, 0, 260, 261, 262, 0, 0, 0, 687, 0, 267,
  0, 0, 7, 8, 9, 10, 11, 12, 13, 0, 254, 255, 256, 257, 11, 258, 259, 635, 0, 33, 14, 15, 16, 703,
  17, 18, 19, 20, 260, 261, 262, 21, 22, 23, 24, 25, 26, 27, 28, 29, 0, 30, 31, 32, 0, 0, 0, 0, 33,
  34, 35, 36, 37, 0, 0, 38, 33, 254, 255, 256, 257, 11, 258, 259, 0, 0, 0, 0, 267, 0, 0, 0, 0, 0, 0,
  260, 261, 262, 0, 0, 0, 0, 0, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
  53, 54, 405, 33, 0, 0, 0, 0, 0, 0, 0, 517, 0, 55, 0, 0, 0, 0, 0, 0, 941, 942, 798, 944, 0, 0, 0,
  0, 0, 0, 0, 333, 0, 0, 807, 0, 0, 882, 0, 0, 0, 0, 768, 769, 770, 254, 255, 256, 257, 11, 258,
  259, 0, 0, 267, 267, 267, 0, 0, 0, 0, 405, 0, 260, 261, 262, 0, 0, 55, 0, 254, 255, 256, 257, 11,
  258, 259, 0, 0, 0, 0, 0, 333, 0, 0, 0, 306, 0, 260, 261, 262, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 573, 0, 0, 998, 0, 31, 0, 0, 849, 934, 0, 33, 267, 0, 333, 0, 333, 379, 934, 0, 0, 0, 254,
  255, 256, 257, 11, 258, 259, 0, 0, 0, 0, 0, 0, 0, 0, 333, 0, 0, 260, 261, 262, 887, 0, 0, 0, 0,
  333, 0, 0, 55, 254, 255, 256, 257, 11, 258, 259, 0, 934, 0, 0, 0, 0, 934, 0, 0, 33, 934, 260, 261,
  262, 0, 379, 458, 55, 0, 0, 0, 0, 0, 0, 0, 0, 405, 0, 0, 0, 0, 0, 0, 405, 0, 0, 0, 0, 0, 33, 0, 0,
  926, 0, 881, 38, 886, 934, 0, 934, 888, 0, 0, 0, 0, 0, 90, 0, 267, 0, 0, 0, 267, 1019, 267, 0,
  1020, 0, 0, 0, 0, 0, 0, 606, 0, 38, 0, 0, 703, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 408, 1019,
  0, 0, 1020, 0, 0, 0, 0, 0, 17, 18, 19, 0, 0, 0, 266, 21, 22, 23, 24, 25, 26, 27, 234, 29, 0, 235,
  267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 117, 118, 119, 120, 267, 121,
  122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
  141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157,
  158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
  177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 39,
  40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 486, 196, 0, 197, 198, 199, 200, 0, 487,
  202, 203, 488, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0, 121,
  122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
  141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157,
  158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
  177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 39,
  40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 486, 196, 492, 197, 198, 199, 200, 0, 487,
  202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0, 121,
  122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
  141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157,
  158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
  177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 39,
  40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 486, 196, 0, 197, 198, 199, 200, 0, 487,
  202, 203, 499, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0, 121,
  122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
  141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157,
  158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
  177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 39,
  40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 486, 196, 0, 197, 198, 199, 200, 502, 487,
  202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0, 121,
  122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
  141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157,
  158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
  177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 39,
  40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 486, 196, 520, 197, 198, 199, 200, 0, 487,
  202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0, 121,
  122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
  141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157,
  158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
  177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 39,
  40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 486, 196, 575, 197, 198, 199, 200, 0, 487,
  202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0, 121,
  122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
  141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157,
  158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
  177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 39,
  40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 486, 196, 816, 197, 198, 199, 200, 0, 487,
  202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0, 121,
  122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
  141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157,
  158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
  177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 39,
  40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 486, 196, 1041, 197, 198, 199, 200, 0,
  487, 202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0,
  121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0,
  157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
  195, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 486, 196, 1045, 197, 198, 199,
  200, 0, 487, 202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119,
  120, 0, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138,
  139, 140, 141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0,
  0, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
  195, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 500, 196, 0, 197, 198, 199, 200,
  0, 487, 202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0,
  121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0,
  157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
  195, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 628, 196, 0, 197, 198, 199, 200,
  0, 487, 202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0,
  121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0,
  157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
  195, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 1046, 196, 0, 197, 198, 199, 200,
  0, 487, 202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0,
  121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0,
  157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
  195, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 1053, 196, 0, 197, 198, 199, 200,
  0, 487, 202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0,
  121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0,
  157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
  195, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 196, 0, 197, 198, 199, 200, 0,
  201, 202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0,
  121, 122, 369, 370, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 708, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0,
  157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 779, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 0, 709, 0,
  39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 908, 610, 909, 780, 711, 781, 371, 0,
  783, 202, 714, 0, 204, 205, 784, 207, 208, 209, 210, 211, 212, 213, 715, 117, 118, 119, 120, 0,
  121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0,
  157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
  195, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 196, 0, 197, 198, 199, 200, 0,
  487, 202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0,
  121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0,
  157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
  195, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 196, 0, 197, 198, 199, 200, 0,
  0, 202, 203, 0, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 117, 118, 119, 120, 0, 121,
  122, 369, 370, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 708, 138, 139, 140,
  141, 142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157,
  158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 779, 174, 175, 176,
  177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 0, 709, 0, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 610, 0, 780, 711, 781, 371, 782, 783, 202,
  714, 0, 204, 205, 784, 207, 208, 209, 210, 211, 212, 213, 715, 117, 118, 119, 120, 0, 121, 122,
  369, 370, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 708, 138, 139, 140, 141,
  142, 143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157, 158,
  159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 779, 174, 175, 176, 177,
  178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 0, 709, 0, 39, 40, 41,
  42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 610, 0, 780, 711, 781, 371, 788, 783, 202, 714,
  0, 204, 205, 784, 207, 208, 209, 210, 211, 212, 213, 715, 117, 118, 119, 120, 0, 121, 122, 369,
  370, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 708, 138, 139, 140, 141, 142,
  143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157, 158, 159,
  160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 779, 174, 175, 176, 177, 178,
  179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 0, 709, 0, 39, 40, 41, 42,
  43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 610, 0, 780, 711, 781, 371, 789, 783, 202, 714, 0,
  204, 205, 784, 207, 208, 209, 210, 211, 212, 213, 715, 117, 118, 119, 120, 0, 121, 122, 369, 370,
  125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 708, 138, 139, 140, 141, 142, 143,
  144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157, 158, 159, 160,
  161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 779, 174, 175, 176, 177, 178, 179,
  180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 0, 709, 0, 39, 40, 41, 42, 43,
  44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 610, 0, 780, 711, 781, 371, 0, 783, 202, 714, 950, 204,
  205, 784, 207, 208, 209, 210, 211, 212, 213, 715, 117, 118, 119, 120, 0, 121, 122, 369, 370, 125,
  126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 708, 138, 139, 140, 141, 142, 143, 144,
  145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157, 158, 159, 160, 161,
  162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 779, 174, 175, 176, 177, 178, 179, 180,
  181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 0, 709, 0, 39, 40, 41, 42, 43, 44, 45,
  46, 47, 48, 49, 50, 51, 52, 53, 0, 610, 0, 780, 711, 781, 371, 0, 783, 202, 714, 952, 204, 205,
  784, 207, 208, 209, 210, 211, 212, 213, 715, 117, 118, 119, 120, 0, 121, 122, 369, 370, 125, 126,
  127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 708, 138, 139, 140, 141, 142, 143, 144, 145,
  146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157, 158, 159, 160, 161, 162,
  163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 362, 174, 175, 176, 177, 178, 179, 180, 181,
  182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 0, 709, 0, 39, 40, 41, 42, 43, 44, 45, 46,
  47, 48, 49, 50, 51, 52, 53, 0, 610, 0, 710, 711, 712, 371, 0, 713, 202, 714, 0, 204, 205, 363,
  207, 208, 209, 210, 211, 212, 213, 715, -592, -592, -592, -592, 0, -592, -592, -592, -592, -592,
  -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592,
  -592, -592, -592, -592, -592, 0, -592, -592, -592, -592, -592, -592, 0, -592, -592, -592, -592, 0,
  0, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592,
  -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592,
  -592, -592, -592, -592, 0, -592, 0, -592, -592, -592, -592, -592, -592, -592, -592, -592, -592,
  -592, -592, -592, -592, -592, 0, -592, 0, -628, -592, -592, -592, 0, -592, -592, -592, 0, -592,
  -592, -592, -592, -592, -592, -592, -592, -592, -592, -592, 117, 118, 119, 120, 0, 121, 122, 369,
  370, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 708, 138, 139, 140, 141, 142,
  143, 144, 145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157, 158, 159,
  160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 746, 174, 175, 176, 177, 178,
  179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 0, 709, 0, 39, 40, 41, 42,
  43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 610, 0, 0, 711, 0, 371, 0, 747, 202, 714, 0, 204,
  205, 748, 207, 208, 209, 210, 211, 212, 213, 715, 117, 118, 119, 120, 0, 121, 122, 369, 370, 125,
  126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 708, 138, 139, 140, 141, 142, 143, 144,
  145, 146, 0, 147, 148, 149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157, 158, 159, 160, 161,
  162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 0, 174, 175, 176, 177, 178, 179, 180, 181,
  182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 0, 709, 0, 39, 40, 41, 42, 43, 44, 45, 46,
  47, 48, 49, 50, 51, 52, 53, 0, 610, 0, 0, 711, 0, 371, 0, 713, 202, 714, 0, 204, 205, 0, 207, 208,
  209, 210, 211, 212, 213, 715, 117, 118, 119, 120, 0, 121, 122, 369, 370, 125, 126, 127, 128, 129,
  130, 131, 132, 133, 134, 135, 136, 708, 138, 139, 140, 141, 142, 143, 144, 145, 146, 0, 147, 148,
  149, 150, 151, 152, 0, 153, 154, 155, 156, 0, 0, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166,
  167, 168, 169, 170, 171, 172, 0, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186,
  187, 188, 189, 190, 191, 192, 0, 0, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
  0, 610, 0, 0, 711, 0, 371, 0, 0, 202, 714, 0, 204, 205, 0, 207, 208, 209, 210, 211, 212, 213, 715,
  227, 228, 229, 230, 231, 232, 233, 0, 0, 527, 0, 0, 0, 0, 0, 0, 0, 0, 134, 135, 136, 0, 17, 18,
  19, 20, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 234, 29, 0, 235, 0, 0, 0, 0, 0, 0, 33, 34, 0, 0, 0,
  528, 529, 0, 0, 0, 0, 0, 170, 171, 172, 530, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183,
  184, 185, 186, 187, 188, 189, 190, 191, 192, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
  50, 51, 52, 53, 0, 0, 0, 531, 0, 532, 533, 0, 534, 202, 535, 0, 204, 205, 536, 207, 208, 209, 210,
  211, 212, 213, 7, 8, 9, 10, 11, 12, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 15, 16, 0, 17, 18,
  19, 20, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 234, 29, 860, 861, 31, 32, 0, 0, 0, 0, 33, 34, 35, 0,
  862, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 255, 256, 257, 11, 258, 259, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 260, 261, 262, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 863, 7, 8, 9,
  10, 11, 12, 13, 33, 0, 0, 0, 55, 0, 0, 0, 0, 0, 0, 14, 15, 16, 0, 17, 18, 19, 20, 0, 0, 0, 21, 22,
  23, 24, 25, 26, 27, 234, 29, 860, 235, 31, 279, 0, 38, 0, 0, 33, 34, 0, 0, 280, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 408, 0, 0, 0, 409, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 39, 40, 41,
  42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 7, 8, 9, 10, 11, 12, 13, 0, 0, 0, 0, 0, 55, 0, 0,
  0, 0, 0, 14, 15, 16, 0, 17, 18, 19, 20, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 234, 29, 0, 235, 31,
  0, 0, 0, 0, 0, 33, 34, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 255, 256, 257, 11, 258, 259, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 260, 261, 262, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
  50, 51, 52, 53, 7, 8, 9, 10, 11, 12, 13, 33, 0, 0, 0, 0, 55, 0, 0, 0, 0, 0, 14, 15, 16, 0, 17, 18,
  19, 20, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 234, 29, 0, 235, 31, 279, 0, 38, 0, 0, 33, 34, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 409, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 7, 8, 9, 10, 11, 12, 13, 0, 0,
  0, 527, 0, 55, 0, 0, 0, 0, 0, 14, 15, 16, 0, 17, 18, 19, 20, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27,
  234, 29, 0, 235, 31, 0, 0, 0, 0, 0, 33, 34, 0, 0, 0, 0, 528, 529, 0, 0, 0, 0, 0, 170, 171, 172,
  530, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
  192, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 0, 0, 0, 531, 0, 532,
  533, 0, 534, 202, 535, 55, 204, 205, 536, 207, 208, 209, 210, 211, 212, 213, 227, 228, 229, 230,
  231, 232, 233, 21, 22, 23, 24, 25, 26, 27, 234, 29, 399, 235, 134, 135, 136, 0, 17, 18, 19, 20,
  400, 0, 0, 21, 22, 23, 24, 25, 26, 27, 234, 29, 0, 235, 31, 279, 0, 0, 0, 0, 33, 34, 0, 0, 280, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 0, 0,
  0, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 227, 228, 229, 230, 231,
  232, 233, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 134, 135, 136, 0, 17, 18, 19, 20, 0, 0, 0, 21, 22, 23,
  24, 25, 26, 27, 234, 29, 0, 235, 31, 0, 0, 0, 0, 0, 33, 34, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 39, 40, 41, 42, 43, 44,
  45, 46, 47, 48, 49, 50, 51, 52, 53, 227, 228, 229, 230, 231, 232, 233, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 134, 135, 136, 0, 17, 18, 19, 20, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 234, 29, 0, 235, 31,
  0, 0, 0, 0, 0, 33, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 227,
  228, 229, 230, 231, 232, 233, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 134, 135, 136, 0, 17, 18, 19, 20,
  0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 234, 29, 0, 235, 0, 0, 0, 0, 0, 0, 33, 34, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 227, 228, 229, 230, 231, 232, 233, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 134, 135, 136, 0, 632, 0, 633, 20, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 234,
  29, 0, 235, 0, 0, 0, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
  51, 52, 53, 227, 228, 229, 230, 231, 232, 233, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 134, 135, 136, 0,
  0, 0, 0, 0, 0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 234, 29, 0, 235, 0, 0, 0, 0, 0, 0, 33, 34, 0, 0,
  0, 0, 0, 21, 22, 23, 24, 25, 26, 27, 234, 29, 399, 235, 0, 0, 0, 0, 0, 0, 0, 0, 400, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 38, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 343, 0, 0, 0, 0,
  0, 0, 0, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, -287, 0, 0, 0, 0, 0, 0, 0,
  0, 344, 0, 0, 0, 345, 21, 22, 23, 24, 25, 26, 27, 234, 29, 399, 235, 0, 0, 0, 0, 0, 0, 0, 0, 400,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 343, 0, 0, 0, 0, 0,
  0, 0, 0, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  344, 0, 0, 0, 345 };

static const short yycheck[] = { 5, 35, 5, 5, 36, 5, 5, 81, 82, 83, 35, 78, 268, 91, 268, 99, 5,
  359, 72, 342, 5, 99, 99, 5, 5, 459, 92, 367, 275, 225, 35, 276, 70, 573, 456, 196, 453, 493, 5,
  91, 5, 458, 203, 105, 504, 505, 35, 828, 385, 450, 35, 344, 345, 35, 35, 245, 583, 851, 377, 749,
  92, 422, 268, 224, 459, 99, 358, 573, 35, 3, 3, 584, 376, 78, 99, 798, 5, 3, 639, 10, 11, 86, 114,
  856, 645, 339, 91, 92, 12, 78, 32, 252, 962, 0, 99, 457, 78, 78, 19, 53, 517, 3, 645, 64, 64, 86,
  983, 103, 37, 105, 78, 78, 85, 78, 99, 10, 11, 99, 988, 86, 44, 64, 10, 11, 91, 92, 3, 431, 584,
  1006, 10, 11, 99, 294, 55, 56, 570, 110, 32, 443, 10, 11, 398, 111, 398, 485, 107, 394, 565, 78,
  923, 512, 342, 107, 516, 116, 116, 85, 3, 113, 244, 3, 4, 5, 6, 7, 8, 9, 85, 102, 99, 105, 426,
  116, 116, 570, 102, 108, 26, 21, 22, 23, 110, 237, 12, 239, 25, 26, 694, 606, 591, 483, 398, 110,
  513, 514, 990, 442, 236, 444, 238, 103, 50, 926, 106, 459, 290, 49, 32, 78, 244, 32, 290, 108, 84,
  616, 44, 778, 302, 244, 108, 85, 116, 103, 471, 102, 103, 588, 108, 106, 284, 523, 0, 111, 795,
  778, 241, 115, 102, 244, 436, 32, 111, 85, 108, 32, 115, 301, 52, 3, 1031, 85, 795, 85, 332, 945,
  290, 102, 103, 244, 102, 106, 244, 268, 106, 106, 793, 108, 958, 274, 960, 85, 114, 327, 241, 329,
  110, 244, 110, 469, 332, 46, 287, 10, 11, 290, 91, 85, 326, 102, 328, 103, 116, 854, 107, 116,
  110, 302, 287, 3, 92, 268, 829, 102, 110, 287, 287, 274, 290, 108, 837, 854, 110, 375, 352, 377,
  570, 102, 102, 509, 287, 102, 287, 290, 108, 116, 26, 332, 102, 116, 29, 30, 31, 107, 581, 302,
  897, 102, 103, 85, 269, 425, 380, 381, 44, 274, 412, 425, 102, 103, 50, 78, 106, 385, 897, 890,
  102, 84, 287, 108, 921, 290, 108, 1021, 824, 332, 826, 102, 107, 45, 905, 376, 48, 108, 113, 584,
  107, 912, 921, 1037, 385, 1039, 113, 388, 111, 108, 913, 900, 115, 112, 1048, 425, 397, 398, 103,
  817, 85, 106, 102, 103, 432, 1025, 106, 32, 33, 104, 946, 412, 413, 376, 339, 449, 1036, 574,
  1038, 24, 397, 103, 385, 105, 425, 388, 108, 103, 108, 1049, 431, 432, 112, 625, 397, 398, 12,
  108, 466, 772, 241, 112, 443, 648, 649, 650, 37, 425, 900, 412, 413, 376, 105, 962, 482, 513, 514,
  108, 459, 102, 103, 112, 425, 106, 103, 466, 104, 268, 431, 107, 360, 361, 810, 29, 30, 31, 366,
  367, 368, 988, 443, 1017, 1018, 110, 103, 774, 274, 106, 413, 272, 32, 33, 554, 276, 495, 102,
  459, 387, 497, 85, 425, 426, 93, 466, 95, 96, 431, 397, 102, 106, 495, 941, 108, 85, 495, 107,
  102, 109, 443, 103, 103, 102, 108, 105, 110, 938, 13, 14, 15, 16, 102, 332, 495, 102, 102, 854,
  108, 85, 110, 429, 360, 361, 103, 360, 361, 109, 366, 367, 368, 366, 367, 368, 477, 554, 102, 891,
  103, 112, 638, 32, 108, 34, 110, 976, 454, 566, 109, 387, 998, 570, 387, 108, 45, 360, 361, 48,
  107, 360, 361, 366, 367, 368, 640, 366, 367, 368, 113, 104, 376, 113, 44, 554, 375, 102, 377, 485,
  102, 398, 102, 3, 387, 659, 112, 566, 387, 112, 109, 570, 113, 429, 107, 412, 429, 106, 102, 85,
  3, 4, 5, 6, 7, 8, 9, 107, 412, 84, 109, 102, 102, 108, 418, 883, 102, 883, 112, 106, 841, 638,
  108, 85, 110, 113, 429, 431, 84, 107, 429, 104, 106, 85, 102, 242, 243, 102, 104, 443, 102, 113,
  459, 442, 113, 444, 108, 664, 110, 485, 102, 106, 485, 107, 37, 84, 108, 105, 110, 638, 107, 3,
  102, 270, 271, 482, 1020, 274, 109, 108, 102, 664, 940, 84, 940, 941, 992, 476, 899, 900, 901,
  105, 485, 1035, 109, 664, 485, 113, 102, 102, 47, 1045, 3, 4, 5, 6, 7, 8, 9, 1051, 103, 67, 109,
  113, 311, 312, 313, 1023, 109, 1025, 21, 22, 23, 109, 513, 514, 105, 761, 113, 109, 102, 102,
  1038, 103, 5, 798, 776, 986, 772, 798, 798, 806, 998, 887, 681, 798, 798, 554, 49, 50, 690, 962,
  970, 798, 508, 798, 659, 509, 584, 477, 554, 646, 5, 570, 926, 772, 766, 798, 954, 3, 4, 5, 6, 7,
  8, 9, 570, 988, 737, 376, 984, 822, 695, 609, 85, 896, 979, 403, 402, 612, 745, 798, 1012, 798,
  798, 584, 798, 798, 884, 740, 589, 877, 698, 772, 884, 884, 875, 798, 566, 351, 850, 798, 819,
  114, 798, 49, 495, 414, 415, 351, 609, 616, 648, 649, 650, 479, 839, 841, 825, 798, 884, 621, 464,
  -1, 431, -1, 819, -1, 1001, 3, 4, 5, 6, 7, 8, 9, 443, -1, 884, 935, 819, 85, -1, -1, -1, 935,
  1019, 884, -1, 648, 649, 650, -1, -1, -1, -1, -1, 798, 875, -1, 875, 875, 698, 875, 875, 698, 883,
  884, -1, -1, 477, 935, -1, 104, 875, 49, 972, -1, 974, -1, 99, 875, 972, -1, 974, 971, 690, 884,
  -1, 935, 884, 970, -1, -1, -1, 698, 875, 941, -1, 698, -1, -1, 968, -1, 883, 884, -1, -1, 972,
  926, 974, 926, -1, 749, 931, 522, 931, 931, 935, 931, 931, 862, -1, 940, 941, 926, -1, 993, 974,
  -1, 931, 997, 926, 926, 875, 1009, -1, 931, -1, -1, -1, 935, -1, 884, 761, 552, 553, 926, -1, -1,
  749, -1, 931, -1, 998, 972, 935, 974, 1024, -1, -1, 940, 941, -1, 91, -1, 983, 94, -1, 970, -1,
  -1, 579, -1, -1, 992, 970, -1, 972, 1029, 974, 998, 983, -1, 4, 891, -1, -1, -1, 1006, 931, -1,
  1042, 972, 935, 974, -1, -1, -1, -1, -1, 995, -1, 839, 983, 841, 1023, -1, 1025, -1, -1, -1, -1,
  992, 245, 622, -1, -1, -1, 998, -1, 1038, -1, -1, -1, 244, -1, 1006, -1, 636, 637, -1, -1, 974,
  -1, -1, -1, -1, -1, -1, 839, -1, 841, -1, 1023, -1, 1025, -1, -1, 5, -1, 992, -1, 272, 890, 891,
  855, 276, 891, 1038, 291, 5, -1, 899, 900, 901, 883, -1, -1, 905, 90, 290, -1, -1, -1, 95, 912,
  -1, -1, -1, 100, 37, 1023, 302, 1025, -1, -1, -1, 891, 109, 110, 890, 891, 37, -1, -1, -1, 1038,
  -1, -1, 899, 900, 901, -1, -1, -1, 905, -1, -1, 945, -1, 342, -1, 912, 241, 914, -1, -1, -1, -1,
  -1, 1027, 958, 940, 960, -1, 962, -1, 1034, 1035, 12, 13, 14, 15, 16, 17, 18, 19, 20, -1, -1, 268,
  269, -1, -1, 1051, 945, -1, -1, -1, 94, -1, 988, -1, -1, -1, 375, -1, 377, 958, -1, 960, -1, 962,
  3, 4, 5, 6, 7, 8, 9, -1, 403, 3, 4, 5, 6, 7, 8, 9, -1, -1, 21, 22, 23, -1, -1, -1, 992, 988, -1,
  1027, -1, -1, 1027, -1, -1, -1, 1034, 1035, -1, 1034, 1035, -1, -1, 332, 425, -1, -1, -1, 49, 50,
  339, 52, -1, 1051, -1, -1, 1051, 1023, 240, 1025, 242, 442, 1027, 444, -1, -1, 1027, -1, -1, 1034,
  1035, -1, 1038, 1034, 1035, -1, 469, -1, -1, -1, -1, -1, 1043, -1, 85, -1, 1051, -1, -1, 1050,
  1051, -1, -1, 862, -1, 476, 385, -1, -1, 3, 4, 5, 6, 7, 8, 9, -1, -1, 226, 398, -1, -1, -1, 114,
  -1, -1, 509, 21, 22, 23, 226, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, 513, 514, -1, -1, -1, 426, -1,
  21, 22, 23, -1, -1, -1, 263, 264, 29, 30, 31, -1, 333, -1, -1, -1, 37, -1, 263, 264, -1, 278, -1,
  280, 269, -1, -1, -1, 49, -1, -1, 459, -1, 278, -1, 280, -1, -1, 466, 3, 4, 5, 6, 7, 8, 9, -1, -1,
  -1, -1, -1, -1, 373, -1, -1, -1, -1, 21, 22, 23, -1, -1, -1, -1, 85, -1, -1, -1, -1, 589, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, 43, 44, -1, -1, 404, 992, 49, -1, -1, -1, -1, 411, -1, -1, 114, 3, 4,
  5, 6, 7, 8, 9, -1, -1, -1, -1, 3, 4, 5, 6, 7, 8, 9, 21, 22, 23, 1023, -1, 1025, 638, -1, -1, 378,
  -1, 21, 22, 23, -1, -1, -1, -1, 1038, -1, -1, 378, -1, -1, -1, -1, -1, -1, 49, -1, 570, 388, -1,
  -1, -1, -1, -1, -1, 114, 49, -1, 685, -1, 3, 4, 5, 6, 7, 8, 9, -1, -1, 696, 697, -1, 424, -1, -1,
  427, -1, -1, 21, 22, 23, 85, 498, -1, 424, -1, -1, 427, -1, -1, -1, -1, 85, 445, -1, -1, -1, -1,
  -1, -1, -1, 44, 106, -1, -1, 445, 49, -1, -1, -1, 114, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, 114, -1,
  -1, -1, 477, -1, -1, -1, 21, 22, 23, -1, -1, -1, 551, -1, 477, -1, -1, 3, 4, 5, 6, 7, 8, 9, -1, 3,
  4, 5, 6, 7, 8, 9, 495, -1, 49, 21, 22, 23, 577, 25, 26, 27, 28, 21, 22, 23, 32, 33, 34, 35, 36,
  37, 38, 39, 40, -1, 42, 43, 44, -1, -1, -1, -1, 49, 50, 51, 52, 53, -1, -1, 85, 49, 3, 4, 5, 6, 7,
  8, 9, -1, -1, -1, -1, 546, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, -1, -1, -1, -1, -1, 85, -1, 87,
  88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 855, 49, -1, -1, -1, -1, -1, -1,
  -1, 772, -1, 114, -1, -1, -1, -1, -1, -1, 885, 886, 676, 888, -1, -1, -1, -1, -1, -1, -1, 884, -1,
  -1, 688, -1, -1, 798, -1, -1, -1, -1, 632, 633, 634, 3, 4, 5, 6, 7, 8, 9, -1, -1, 632, 633, 634,
  -1, -1, -1, -1, 914, -1, 21, 22, 23, -1, -1, 114, -1, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1,
  935, -1, -1, -1, 44, -1, 21, 22, 23, 49, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 695,
  -1, -1, 973, -1, 43, -1, -1, 767, 875, -1, 49, 695, -1, 972, -1, 974, 883, 884, -1, -1, -1, 3, 4,
  5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, -1, 995, -1, -1, 21, 22, 23, 802, -1, -1, -1, -1, 1006,
  -1, -1, 114, 3, 4, 5, 6, 7, 8, 9, -1, 926, -1, -1, -1, -1, 931, -1, -1, 49, 935, 21, 22, 23, -1,
  940, 941, 114, -1, -1, -1, -1, -1, -1, -1, -1, 1043, -1, -1, -1, -1, -1, -1, 1050, -1, -1, -1, -1,
  -1, 49, -1, -1, 860, -1, 798, 85, 800, 972, -1, 974, 804, -1, -1, -1, -1, -1, 798, -1, 800, -1,
  -1, -1, 804, 103, 806, -1, 106, -1, -1, -1, -1, -1, -1, 998, -1, 85, -1, -1, 896, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, 102, 103, -1, -1, 106, -1, -1, -1, -1, -1, 25, 26, 27, -1, -1,
  -1, 862, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1, 42, 862, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, 4, 5, 6, 887, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
  19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42,
  43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
  67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
  91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, -1, 105, 106, 107, 108, -1, 110, 111, 112,
  113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13,
  14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37,
  38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
  62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
  86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107,
  108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1,
  8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
  33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56,
  57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
  81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103,
  -1, 105, 106, 107, 108, -1, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123,
  124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
  28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75,
  76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
  100, 101, 102, 103, -1, 105, 106, 107, 108, 109, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119,
  120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1,
  -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
  71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94,
  95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115,
  116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1,
  42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
  66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
  90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, -1, 110, 111,
  112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36,
  37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
  61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
  85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
  107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5,
  6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54,
  55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
  79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102,
  103, 104, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122,
  123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
  26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49,
  50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
  74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97,
  98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117,
  118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43,
  44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
  68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
  92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, -1, 105, 106, 107, 108, -1, 110, 111, 112, -1,
  114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38,
  39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
  63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86,
  87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, -1, 105, 106, 107, 108,
  -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8,
  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
  -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
  58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
  82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, -1,
  105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
  3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
  29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52,
  53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
  77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
  101, 102, 103, -1, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120,
  121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1,
  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
  72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
  96, 97, 98, 99, 100, 101, -1, 103, -1, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116,
  117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
  19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42,
  43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
  67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90,
  91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, -1, 110, 111,
  112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36,
  37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
  61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
  85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, 105, 106, 107,
  108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1,
  8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
  33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56,
  57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
  81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1,
  105, 106, 107, 108, -1, -1, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
  3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
  29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52,
  53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
  77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
  101, -1, 103, -1, 105, 106, 107, 108, 109, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120,
  121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1,
  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
  72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95,
  96, 97, 98, 99, 100, 101, -1, 103, -1, 105, 106, 107, 108, 109, 110, 111, 112, -1, 114, 115, 116,
  117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
  19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42,
  43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
  67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90,
  91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, 105, 106, 107, 108, 109, 110, 111, 112,
  -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13,
  14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37,
  38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
  62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85,
  -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, 105, 106, 107, 108,
  -1, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8,
  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
  -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
  58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
  82, 83, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1,
  105, 106, 107, 108, -1, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
  3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
  29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52,
  53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
  77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
  101, -1, 103, -1, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120,
  121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1,
  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
  72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95,
  96, 97, 98, 99, 100, 101, -1, 103, -1, 105, 106, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116,
  117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
  19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37, 38, 39, 40, -1, 42,
  43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
  67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90,
  91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, -1, 106, -1, 108, -1, 110, 111, 112,
  -1, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13,
  14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, -1, 35, 36, 37,
  38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
  62, 63, -1, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, -1, 85,
  -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, -1, 106, -1, 108,
  -1, 110, 111, 112, -1, 114, 115, -1, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5, 6, -1, 8, 9,
  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
  -1, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, 45, -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
  58, 59, 60, 61, 62, 63, -1, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
  82, 83, -1, -1, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, 103, -1, -1,
  106, -1, 108, -1, -1, 111, 112, -1, 114, 115, -1, 117, 118, 119, 120, 121, 122, 123, 124, 3, 4, 5,
  6, 7, 8, 9, -1, -1, 12, -1, -1, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25, 26, 27, 28, -1, -1,
  -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1, 42, -1, -1, -1, -1, -1, -1, 49, 50, -1, -1, -1, 54,
  55, -1, -1, -1, -1, -1, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
  79, 80, 81, 82, 83, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1,
  -1, -1, 105, -1, 107, 108, -1, 110, 111, 112, -1, 114, 115, 116, 117, 118, 119, 120, 121, 122,
  123, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25, 26, 27,
  28, -1, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, -1, -1, -1, -1, 49, 50, 51,
  -1, 53, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, 21, 22, 23, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101,
  102, 3, 4, 5, 6, 7, 8, 9, 49, -1, -1, -1, 114, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25, 26, 27,
  28, -1, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, -1, 85, -1, -1, 49, 50, -1,
  -1, 53, -1, -1, -1, -1, -1, -1, -1, -1, -1, 102, -1, -1, -1, 106, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
  100, 101, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, 114, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25,
  26, 27, 28, -1, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1, 42, 43, -1, -1, -1, -1, -1, 49,
  50, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, 21, 22, 23, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
  100, 101, 3, 4, 5, 6, 7, 8, 9, 49, -1, -1, -1, -1, 114, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25,
  26, 27, 28, -1, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1, 42, 43, 44, -1, 85, -1, -1, 49,
  50, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 106, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97,
  98, 99, 100, 101, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, 12, -1, 114, -1, -1, -1, -1, -1, 21, 22, 23,
  -1, 25, 26, 27, 28, -1, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1, 42, 43, -1, -1, -1, -1,
  -1, 49, 50, -1, -1, -1, -1, 54, 55, -1, -1, -1, -1, -1, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
  71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 85, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95,
  96, 97, 98, 99, 100, 101, -1, -1, -1, -1, 105, -1, 107, 108, -1, 110, 111, 112, 114, 114, 115,
  116, 117, 118, 119, 120, 121, 122, 123, 3, 4, 5, 6, 7, 8, 9, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 21, 22, 23, -1, 25, 26, 27, 28, 51, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1, 42,
  43, 44, -1, -1, -1, -1, 49, 50, -1, -1, 53, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 87, 88,
  89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, -1, -1, -1, -1, -1, 85, -1, 87, 88, 89, 90,
  91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, 21, 22, 23, -1, 25, 26, 27, 28, -1, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1,
  42, 43, -1, -1, -1, -1, -1, 49, 50, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 85, -1, 87, 88, 89,
  90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, 21, 22, 23, -1, 25, 26, 27, 28, -1, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  -1, 42, 43, -1, -1, -1, -1, -1, 49, 50, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 85, -1, 87, 88,
  89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, 21, 22, 23, -1, 25, 26, 27, 28, -1, -1, -1, 32, 33, 34, 35, 36, 37, 38, 39,
  40, -1, 42, -1, -1, -1, -1, -1, -1, 49, 50, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 85, -1, 87,
  88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, 25, -1, 27, 28, -1, -1, -1, 32, 33, 34, 35, 36, 37, 38,
  39, 40, -1, 42, -1, -1, -1, -1, -1, -1, 49, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 85, -1,
  87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, 21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, 32, 33, 34, 35, 36, 37,
  38, 39, 40, -1, 42, -1, -1, -1, -1, -1, -1, 49, 50, -1, -1, -1, -1, -1, 32, 33, 34, 35, 36, 37,
  38, 39, 40, 41, 42, -1, -1, -1, -1, -1, -1, -1, -1, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1, 85,
  -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 78, -1, -1, -1, -1, -1, -1, -1,
  -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, -1, -1, -1, -1, -1, -1, -1,
  -1, 111, -1, -1, -1, 115, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, -1, -1, -1, -1, -1, -1, -1,
  -1, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, 78, -1, -1, -1, -1, -1, -1, -1, -1, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97,
  98, 99, 100, 101, -1, -1, -1, -1, -1, -1, -1, -1, -1, 111, -1, -1, -1, 115 };

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] = { 0, 126, 127, 0, 128, 342, 343, 3, 4, 5, 6, 7, 8, 9, 21, 22,
  23, 25, 26, 27, 28, 32, 33, 34, 35, 36, 37, 38, 39, 40, 42, 43, 44, 49, 50, 51, 52, 53, 85, 87,
  88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 114, 129, 130, 131, 132, 133, 135,
  136, 137, 138, 139, 141, 144, 159, 160, 161, 163, 164, 174, 175, 184, 186, 187, 189, 208, 209,
  210, 211, 214, 215, 218, 223, 264, 294, 295, 296, 297, 299, 300, 301, 302, 304, 306, 307, 310,
  311, 312, 313, 314, 316, 317, 320, 321, 332, 333, 334, 354, 25, 26, 52, 12, 44, 3, 4, 5, 6, 8, 9,
  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
  35, 36, 37, 38, 39, 40, 42, 43, 44, 45, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
  62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
  86, 103, 105, 106, 107, 108, 110, 111, 112, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
  333, 334, 365, 366, 367, 397, 398, 399, 400, 401, 305, 322, 3, 4, 5, 6, 7, 8, 9, 39, 42, 139, 144,
  161, 164, 296, 297, 302, 304, 310, 316, 3, 4, 5, 6, 8, 9, 103, 307, 3, 4, 5, 6, 8, 9, 21, 22, 23,
  50, 52, 185, 294, 296, 297, 301, 302, 304, 308, 140, 352, 353, 308, 103, 352, 44, 53, 130, 137,
  138, 164, 170, 187, 189, 208, 264, 310, 316, 46, 102, 103, 237, 238, 237, 237, 237, 108, 144, 310,
  316, 102, 342, 44, 215, 242, 245, 295, 300, 302, 304, 146, 302, 304, 306, 307, 301, 295, 296, 301,
  342, 301, 110, 139, 144, 161, 164, 175, 215, 297, 311, 320, 342, 10, 11, 84, 202, 265, 273, 275,
  78, 111, 115, 270, 335, 336, 337, 338, 341, 318, 342, 342, 24, 355, 307, 103, 397, 393, 393, 64,
  116, 191, 383, 393, 394, 393, 10, 11, 108, 387, 294, 12, 308, 352, 308, 352, 295, 139, 161, 179,
  180, 183, 202, 273, 393, 105, 134, 294, 294, 102, 344, 345, 218, 222, 223, 297, 41, 51, 266, 269,
  270, 309, 311, 334, 103, 102, 106, 145, 146, 297, 301, 302, 304, 354, 266, 162, 102, 106, 165,
  294, 294, 352, 310, 202, 106, 247, 393, 216, 352, 298, 45, 48, 228, 229, 108, 301, 301, 301, 303,
  308, 352, 308, 352, 215, 242, 342, 319, 276, 219, 220, 222, 223, 224, 240, 284, 295, 297, 267,
  105, 257, 258, 260, 261, 202, 273, 282, 335, 346, 347, 346, 346, 336, 338, 308, 53, 356, 357, 358,
  359, 360, 103, 127, 394, 102, 110, 113, 395, 396, 397, 104, 193, 195, 196, 198, 200, 190, 113,
  102, 396, 109, 389, 390, 388, 342, 176, 178, 270, 145, 176, 294, 308, 308, 177, 284, 295, 302,
  304, 104, 296, 302, 103, 102, 105, 354, 12, 54, 55, 64, 105, 107, 108, 110, 112, 116, 364, 365,
  218, 222, 102, 267, 265, 342, 148, 143, 3, 102, 147, 342, 146, 302, 304, 297, 102, 3, 4, 5, 6, 7,
  8, 9, 167, 168, 306, 166, 165, 342, 294, 297, 248, 249, 294, 103, 104, 250, 251, 145, 302, 348,
  349, 387, 246, 377, 266, 145, 266, 294, 308, 314, 315, 340, 32, 33, 225, 226, 227, 344, 225, 286,
  287, 288, 344, 219, 224, 295, 102, 107, 259, 103, 391, 108, 109, 273, 354, 339, 185, 294, 113,
  358, 107, 300, 307, 362, 363, 127, 104, 102, 113, 107, 383, 25, 27, 164, 296, 302, 304, 310, 327,
  328, 331, 332, 26, 50, 203, 189, 342, 373, 373, 373, 102, 177, 183, 102, 165, 176, 176, 102, 107,
  108, 344, 102, 127, 188, 3, 112, 112, 64, 116, 109, 113, 29, 30, 31, 104, 149, 150, 29, 30, 31,
  37, 154, 155, 158, 294, 106, 342, 146, 104, 107, 344, 317, 102, 306, 107, 398, 400, 393, 109, 84,
  252, 254, 342, 301, 231, 354, 250, 24, 85, 105, 106, 107, 110, 112, 124, 333, 334, 365, 366, 367,
  371, 372, 379, 380, 381, 383, 384, 387, 391, 102, 102, 102, 165, 314, 78, 111, 230, 108, 112, 289,
  290, 106, 108, 344, 268, 64, 110, 116, 368, 369, 371, 381, 392, 262, 367, 274, 340, 106, 113, 359,
  301, 84, 361, 387, 104, 194, 192, 294, 294, 294, 320, 202, 271, 275, 270, 329, 271, 203, 64, 105,
  107, 109, 110, 116, 371, 374, 375, 109, 109, 102, 102, 178, 181, 104, 316, 113, 113, 342, 106,
  157, 158, 107, 37, 156, 202, 142, 342, 168, 105, 105, 171, 398, 249, 202, 202, 104, 217, 107, 255,
  3, 232, 243, 109, 386, 382, 385, 102, 228, 221, 291, 290, 13, 14, 15, 16, 285, 241, 269, 370, 369,
  378, 107, 109, 108, 277, 287, 300, 195, 342, 330, 283, 284, 197, 346, 308, 199, 271, 250, 271, 41,
  42, 53, 102, 131, 136, 138, 151, 152, 153, 159, 160, 174, 184, 187, 189, 212, 213, 214, 242, 264,
  294, 295, 297, 310, 316, 294, 342, 294, 154, 169, 394, 102, 239, 225, 84, 253, 316, 247, 373, 377,
  373, 348, 250, 292, 293, 250, 372, 102, 104, 375, 376, 263, 278, 308, 277, 105, 204, 205, 271,
  281, 335, 204, 201, 109, 102, 342, 138, 152, 153, 187, 189, 212, 264, 295, 310, 237, 102, 222,
  242, 297, 202, 202, 155, 202, 368, 47, 254, 271, 244, 113, 383, 113, 67, 234, 235, 109, 113, 368,
  109, 368, 250, 206, 109, 271, 204, 182, 136, 144, 172, 189, 213, 310, 316, 310, 344, 222, 224,
  400, 256, 105, 233, 113, 236, 231, 350, 351, 109, 207, 379, 272, 280, 352, 144, 172, 310, 237,
  144, 202, 102, 344, 103, 257, 19, 55, 56, 310, 321, 323, 324, 233, 354, 279, 379, 277, 32, 34, 45,
  48, 103, 106, 145, 173, 352, 144, 352, 102, 393, 321, 325, 270, 280, 400, 400, 393, 394, 147, 145,
  352, 145, 173, 104, 326, 308, 348, 104, 102, 173, 145, 147, 308, 394, 173, 102 };

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
  331, 331, 331, 331, 331, 332, 332, 333, 333, 333, 333, 333, 333, 333, 333, 333, 334, 334, 334,
  334, 334, 334, 334, 334, 334, 334, 334, 334, 334, 334, 334, 335, 335, 335, 335, 336, 337, 339,
  338, 340, 340, 341, 341, 343, 342, 345, 344, 347, 346, 349, 348, 351, 350, 353, 352, 354, 354,
  355, 356, 356, 357, 358, 358, 358, 358, 360, 359, 361, 361, 362, 362, 363, 363, 364, 364, 364,
  364, 364, 364, 364, 364, 364, 364, 364, 364, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365,
  365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365, 365,
  365, 365, 365, 365, 365, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366,
  366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 366, 367, 367, 367,
  367, 367, 367, 367, 367, 367, 368, 368, 369, 369, 369, 370, 369, 369, 371, 371, 372, 372, 372,
  372, 372, 372, 372, 372, 372, 372, 373, 373, 374, 374, 374, 374, 375, 375, 375, 376, 376, 377,
  377, 378, 378, 379, 379, 380, 380, 380, 382, 381, 383, 383, 385, 384, 386, 384, 388, 387, 389,
  387, 390, 387, 392, 391, 393, 393, 394, 394, 395, 395, 396, 396, 397, 397, 397, 397, 397, 397,
  397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 397, 398, 399, 399, 400, 401, 401, 401 };

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
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0, 4, 0, 1, 1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,
  0, 2, 0, 2, 4, 2, 1, 3, 0, 1, 2, 3, 0, 3, 0, 1, 1, 2, 1, 3, 2, 2, 3, 3, 1, 1, 1, 1, 2, 2, 2, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 0, 2, 0, 2, 1, 1, 1, 1, 1, 0, 4, 1, 1, 0, 4, 0, 5, 0, 4, 0, 4, 0, 4, 0, 4, 0, 2, 0, 2,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 4, 3, 1, 1, 1 };

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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const unsigned char yyconflp[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 235, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  237, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 239, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 229, 0, 0, 0, 0, 0, 0, 0, 0, 231, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 11, 13, 15, 0, 17,
  19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65,
  67, 0, 69, 71, 73, 75, 77, 79, 0, 81, 83, 85, 87, 0, 0, 89, 91, 93, 95, 97, 99, 101, 103, 105,
  107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137, 139, 141, 143,
  145, 147, 149, 151, 153, 155, 157, 159, 0, 161, 0, 163, 165, 167, 169, 171, 173, 175, 177, 179,
  181, 183, 185, 187, 189, 191, 0, 193, 0, 0, 195, 197, 199, 0, 201, 203, 205, 0, 207, 209, 211,
  213, 215, 217, 219, 221, 223, 225, 227, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short yyconfl[] = { 0, 410, 0, 410, 0, 410, 0, 321, 0, 628, 0, 628, 0, 628, 0, 628, 0,
  628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0,
  628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0,
  628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0,
  628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0,
  628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0,
  628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0,
  628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0,
  628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0,
  628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 628, 0, 304, 0, 304, 0,
  304, 0, 314, 0, 410, 0, 410, 0 };

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
      ((*yyvalp).integer) = VTK_PARSE_OSTREAM;
    }

    break;

    case 439:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_ISTREAM;
    }

    break;

    case 440:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_UNKNOWN;
    }

    break;

    case 441:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_OBJECT;
    }

    break;

    case 442:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_QOBJECT;
    }

    break;

    case 443:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_NULLPTR_T;
    }

    break;

    case 444:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_SSIZE_T;
    }

    break;

    case 445:

    {
      typeSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      ((*yyvalp).integer) = VTK_PARSE_SIZE_T;
    }

    break;

    case 446:

    {
      postSig("auto ");
      ((*yyvalp).integer) = 0;
    }

    break;

    case 447:

    {
      postSig("void ");
      ((*yyvalp).integer) = VTK_PARSE_VOID;
    }

    break;

    case 448:

    {
      postSig("bool ");
      ((*yyvalp).integer) = VTK_PARSE_BOOL;
    }

    break;

    case 449:

    {
      postSig("float ");
      ((*yyvalp).integer) = VTK_PARSE_FLOAT;
    }

    break;

    case 450:

    {
      postSig("double ");
      ((*yyvalp).integer) = VTK_PARSE_DOUBLE;
    }

    break;

    case 451:

    {
      postSig("char ");
      ((*yyvalp).integer) = VTK_PARSE_CHAR;
    }

    break;

    case 452:

    {
      postSig("char16_t ");
      ((*yyvalp).integer) = VTK_PARSE_CHAR16_T;
    }

    break;

    case 453:

    {
      postSig("char32_t ");
      ((*yyvalp).integer) = VTK_PARSE_CHAR32_T;
    }

    break;

    case 454:

    {
      postSig("wchar_t ");
      ((*yyvalp).integer) = VTK_PARSE_WCHAR_T;
    }

    break;

    case 455:

    {
      postSig("int ");
      ((*yyvalp).integer) = VTK_PARSE_INT;
    }

    break;

    case 456:

    {
      postSig("short ");
      ((*yyvalp).integer) = VTK_PARSE_SHORT;
    }

    break;

    case 457:

    {
      postSig("long ");
      ((*yyvalp).integer) = VTK_PARSE_LONG;
    }

    break;

    case 458:

    {
      postSig("__int64 ");
      ((*yyvalp).integer) = VTK_PARSE___INT64;
    }

    break;

    case 459:

    {
      postSig("signed ");
      ((*yyvalp).integer) = VTK_PARSE_INT;
    }

    break;

    case 460:

    {
      postSig("unsigned ");
      ((*yyvalp).integer) = VTK_PARSE_UNSIGNED_INT;
    }

    break;

    case 464:

    {
      ((*yyvalp).integer) =
        ((((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.integer) |
          (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 465:

    {
      postSig("&");
      ((*yyvalp).integer) = VTK_PARSE_REF;
    }

    break;

    case 466:

    {
      postSig("&&");
      ((*yyvalp).integer) = (VTK_PARSE_RVALUE | VTK_PARSE_REF);
    }

    break;

    case 467:

    {
      postSig("*");
    }

    break;

    case 468:

    {
      ((*yyvalp).integer) =
        (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer);
    }

    break;

    case 469:

    {
      ((*yyvalp).integer) = VTK_PARSE_POINTER;
    }

    break;

    case 470:

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

    case 472:

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

    case 473:

    {
      setAttributeRole(VTK_PARSE_ATTRIB_DECL);
    }

    break;

    case 474:

    {
      clearAttributeRole();
    }

    break;

    case 475:

    {
      setAttributeRole(VTK_PARSE_ATTRIB_ID);
    }

    break;

    case 476:

    {
      clearAttributeRole();
    }

    break;

    case 477:

    {
      setAttributeRole(VTK_PARSE_ATTRIB_REF);
    }

    break;

    case 478:

    {
      clearAttributeRole();
    }

    break;

    case 479:

    {
      setAttributeRole(VTK_PARSE_ATTRIB_FUNC);
    }

    break;

    case 480:

    {
      clearAttributeRole();
    }

    break;

    case 481:

    {
      setAttributeRole(VTK_PARSE_ATTRIB_ARRAY);
    }

    break;

    case 482:

    {
      clearAttributeRole();
    }

    break;

    case 483:

    {
      setAttributeRole(VTK_PARSE_ATTRIB_CLASS);
    }

    break;

    case 484:

    {
      clearAttributeRole();
    }

    break;

    case 487:

    {
      setAttributePrefix(NULL);
    }

    break;

    case 490:

    {
      setAttributePrefix(vtkstrcat(
        (((yyGLRStackItem const*)yyvsp)[YYFILL(-1)].yystate.yysemantics.yysval.str), "::"));
    }

    break;

    case 495:

    {
      markSig();
    }

    break;

    case 496:

    {
      handle_attribute(
        cutSig(), (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.integer));
    }

    break;

    case 497:

    {
      ((*yyvalp).integer) = 0;
    }

    break;

    case 498:

    {
      ((*yyvalp).integer) = VTK_PARSE_PACK;
    }

    break;

    case 503:

    {
      ((*yyvalp).str) = "()";
    }

    break;

    case 504:

    {
      ((*yyvalp).str) = "[]";
    }

    break;

    case 505:

    {
      ((*yyvalp).str) = " new[]";
    }

    break;

    case 506:

    {
      ((*yyvalp).str) = " delete[]";
    }

    break;

    case 507:

    {
      ((*yyvalp).str) = "<";
    }

    break;

    case 508:

    {
      ((*yyvalp).str) = ">";
    }

    break;

    case 509:

    {
      ((*yyvalp).str) = ",";
    }

    break;

    case 510:

    {
      ((*yyvalp).str) = "=";
    }

    break;

    case 511:

    {
      ((*yyvalp).str) = ">>";
    }

    break;

    case 512:

    {
      ((*yyvalp).str) = ">>";
    }

    break;

    case 513:

    {
      ((*yyvalp).str) = vtkstrcat(
        "\"\" ", (((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
    }

    break;

    case 515:

    {
      ((*yyvalp).str) = "%";
    }

    break;

    case 516:

    {
      ((*yyvalp).str) = "*";
    }

    break;

    case 517:

    {
      ((*yyvalp).str) = "/";
    }

    break;

    case 518:

    {
      ((*yyvalp).str) = "-";
    }

    break;

    case 519:

    {
      ((*yyvalp).str) = "+";
    }

    break;

    case 520:

    {
      ((*yyvalp).str) = "!";
    }

    break;

    case 521:

    {
      ((*yyvalp).str) = "~";
    }

    break;

    case 522:

    {
      ((*yyvalp).str) = "&";
    }

    break;

    case 523:

    {
      ((*yyvalp).str) = "|";
    }

    break;

    case 524:

    {
      ((*yyvalp).str) = "^";
    }

    break;

    case 525:

    {
      ((*yyvalp).str) = " new";
    }

    break;

    case 526:

    {
      ((*yyvalp).str) = " delete";
    }

    break;

    case 527:

    {
      ((*yyvalp).str) = "<<=";
    }

    break;

    case 528:

    {
      ((*yyvalp).str) = ">>=";
    }

    break;

    case 529:

    {
      ((*yyvalp).str) = "<<";
    }

    break;

    case 530:

    {
      ((*yyvalp).str) = ".*";
    }

    break;

    case 531:

    {
      ((*yyvalp).str) = "->*";
    }

    break;

    case 532:

    {
      ((*yyvalp).str) = "->";
    }

    break;

    case 533:

    {
      ((*yyvalp).str) = "+=";
    }

    break;

    case 534:

    {
      ((*yyvalp).str) = "-=";
    }

    break;

    case 535:

    {
      ((*yyvalp).str) = "*=";
    }

    break;

    case 536:

    {
      ((*yyvalp).str) = "/=";
    }

    break;

    case 537:

    {
      ((*yyvalp).str) = "%=";
    }

    break;

    case 538:

    {
      ((*yyvalp).str) = "++";
    }

    break;

    case 539:

    {
      ((*yyvalp).str) = "--";
    }

    break;

    case 540:

    {
      ((*yyvalp).str) = "&=";
    }

    break;

    case 541:

    {
      ((*yyvalp).str) = "|=";
    }

    break;

    case 542:

    {
      ((*yyvalp).str) = "^=";
    }

    break;

    case 543:

    {
      ((*yyvalp).str) = "&&";
    }

    break;

    case 544:

    {
      ((*yyvalp).str) = "||";
    }

    break;

    case 545:

    {
      ((*yyvalp).str) = "==";
    }

    break;

    case 546:

    {
      ((*yyvalp).str) = "!=";
    }

    break;

    case 547:

    {
      ((*yyvalp).str) = "<=";
    }

    break;

    case 548:

    {
      ((*yyvalp).str) = ">=";
    }

    break;

    case 549:

    {
      ((*yyvalp).str) = "typedef";
    }

    break;

    case 550:

    {
      ((*yyvalp).str) = "typename";
    }

    break;

    case 551:

    {
      ((*yyvalp).str) = "class";
    }

    break;

    case 552:

    {
      ((*yyvalp).str) = "struct";
    }

    break;

    case 553:

    {
      ((*yyvalp).str) = "union";
    }

    break;

    case 554:

    {
      ((*yyvalp).str) = "template";
    }

    break;

    case 555:

    {
      ((*yyvalp).str) = "public";
    }

    break;

    case 556:

    {
      ((*yyvalp).str) = "protected";
    }

    break;

    case 557:

    {
      ((*yyvalp).str) = "private";
    }

    break;

    case 558:

    {
      ((*yyvalp).str) = "const";
    }

    break;

    case 559:

    {
      ((*yyvalp).str) = "volatile";
    }

    break;

    case 560:

    {
      ((*yyvalp).str) = "static";
    }

    break;

    case 561:

    {
      ((*yyvalp).str) = "thread_local";
    }

    break;

    case 562:

    {
      ((*yyvalp).str) = "constexpr";
    }

    break;

    case 563:

    {
      ((*yyvalp).str) = "inline";
    }

    break;

    case 564:

    {
      ((*yyvalp).str) = "virtual";
    }

    break;

    case 565:

    {
      ((*yyvalp).str) = "explicit";
    }

    break;

    case 566:

    {
      ((*yyvalp).str) = "decltype";
    }

    break;

    case 567:

    {
      ((*yyvalp).str) = "default";
    }

    break;

    case 568:

    {
      ((*yyvalp).str) = "extern";
    }

    break;

    case 569:

    {
      ((*yyvalp).str) = "using";
    }

    break;

    case 570:

    {
      ((*yyvalp).str) = "namespace";
    }

    break;

    case 571:

    {
      ((*yyvalp).str) = "operator";
    }

    break;

    case 572:

    {
      ((*yyvalp).str) = "enum";
    }

    break;

    case 573:

    {
      ((*yyvalp).str) = "throw";
    }

    break;

    case 574:

    {
      ((*yyvalp).str) = "noexcept";
    }

    break;

    case 575:

    {
      ((*yyvalp).str) = "const_cast";
    }

    break;

    case 576:

    {
      ((*yyvalp).str) = "dynamic_cast";
    }

    break;

    case 577:

    {
      ((*yyvalp).str) = "static_cast";
    }

    break;

    case 578:

    {
      ((*yyvalp).str) = "reinterpret_cast";
    }

    break;

    case 592:

    {
      postSig("< ");
    }

    break;

    case 593:

    {
      postSig("> ");
    }

    break;

    case 595:

    {
      postSig(">");
    }

    break;

    case 597:

    {
      chopSig();
      postSig("::");
    }

    break;

    case 601:

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

    case 602:

    {
      postSig(":");
      postSig(" ");
    }

    break;

    case 603:

    {
      postSig(".");
    }

    break;

    case 604:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      postSig(" ");
    }

    break;

    case 605:

    {
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      postSig(" ");
    }

    break;

    case 607:

    {
      chopSig();
      postSig(" ");
    }

    break;

    case 611:

    {
      postSig("< ");
    }

    break;

    case 612:

    {
      postSig("> ");
    }

    break;

    case 613:

    {
      postSig(">");
    }

    break;

    case 615:

    {
      postSig("= ");
    }

    break;

    case 616:

    {
      chopSig();
      postSig(", ");
    }

    break;

    case 618:

    {
      chopSig();
      postSig(";");
    }

    break;

    case 626:

    {
      postSig("= ");
    }

    break;

    case 627:

    {
      chopSig();
      postSig(", ");
    }

    break;

    case 628:

    {
      chopSig();
      if (getSig()[getSigLength() - 1] == '<')
      {
        postSig(" ");
      }
      postSig("<");
    }

    break;

    case 629:

    {
      chopSig();
      if (getSig()[getSigLength() - 1] == '>')
      {
        postSig(" ");
      }
      postSig("> ");
    }

    break;

    case 632:

    {
      postSigLeftBracket("[");
    }

    break;

    case 633:

    {
      postSigRightBracket("] ");
    }

    break;

    case 634:

    {
      postSig("[[");
    }

    break;

    case 635:

    {
      chopSig();
      postSig("]] ");
    }

    break;

    case 636:

    {
      postSigLeftBracket("(");
    }

    break;

    case 637:

    {
      postSigRightBracket(") ");
    }

    break;

    case 638:

    {
      postSigLeftBracket("(");
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      postSig("*");
    }

    break;

    case 639:

    {
      postSigRightBracket(") ");
    }

    break;

    case 640:

    {
      postSigLeftBracket("(");
      postSig((((yyGLRStackItem const*)yyvsp)[YYFILL(0)].yystate.yysemantics.yysval.str));
      postSig("&");
    }

    break;

    case 641:

    {
      postSigRightBracket(") ");
    }

    break;

    case 642:

    {
      postSig("{ ");
    }

    break;

    case 643:

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

#define yypact_value_is_default(Yystate) (!!((Yystate) == (-878)))

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
