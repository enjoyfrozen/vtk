#!/usr/bin/env python

"""
Given a VTK module (full name including "vtk", e.g. vtkCommonCore),
this program will write the contents of a .pyi stub file to stdout.
It is still experimental and might produce garbage.
"""

from vtkmodules.vtkCommonCore import vtkObject
import re
import sys
import inspect
import importlib

# regular expressions
string = re.compile(r"""("([^\\"]|\\.)*"|'([^\\']|\\.)*')""")
indent = re.compile(r"[ \t]+\S")
has_self = re.compile(r"[(]self[,)]")

# important characters for rapidly parsing code
keychar = re.compile(r"[\'\"{}\[\]()\n]")

# VTK methods have a special type
vtkmethod = type(vtkObject.IsA)

def isvtkmethod(m):
    return (type(m) == vtkmethod)

ismethod = inspect.isroutine

isclass = inspect.isclass

def parse_error(message, text, begin, pos):
    """Print a parse error, syntax or otherwise.
    """
    end = text.find('\n', pos)
    if end == -1:
        end = len(text)
    sys.stderr.write("Error: " + message + ":\n")
    sys.stderr.write(text[begin:end] + "\n");
    sys.stderr.write('-' * (pos - begin) + "^\n")

def push_signature(o, l, signature):
    """Process a method signature and add it to the list.
    """
    signature = re.sub(r"\s+", " ", signature)
    if signature.startswith('C++:'):
        # if C++ method is static, mark Python signature static
        if isvtkmethod(o) and signature.find(" static ") != -1 and len(l) > 0:
            if not l[-1].startswith("@staticmethod"):
                l[-1] = "@staticmethod\n" + l[-1]
    elif signature.startswith(o.__name__ + "("):
        if isvtkmethod(o) and not has_self.search(signature):
            if not signature.startswith("@staticmethod"):
                signature = "@staticmethod\n" + signature
        l.append(signature)

def get_signatures(o):
    """Return a list of method signatures found in the docstring.
    """
    doc = o.__doc__
    signatures = [] # output method signatures
    begin = 0 # beginning of current signature
    pos = 0 # current position in docstring
    delim_stack = [] # keep track of bracket depth

    # loop through docstring using longest strides possible
    # (this will go line-by-line or until first ( ) { } [ ] " ' )
    while pos < len(doc):
        # look for the next "character of insterest" in docstring
        match = keychar.search(doc, pos)
        # did we find a match before the end of docstring?
        if match:
            # get new position
            pos,end = match.span()
            # take different action, depending on char
            c = match.group()
            if c in '\"\'':
                # skip over a string literal
                m = string.match(doc, pos)
                if m:
                    pos,end = m.span()
                else:
                    parse_error("Unterminated string", doc, begin, pos)
                    break
            elif c in '{[(':
                # descend into a bracketed expression (push stack)
                delim_stack.append({'{':'}','[':']','(':')'}[c])
            elif c in '}])':
                # ascend out of a bracketed expression (pop stack)
                if not delim_stack or c != delim_stack.pop():
                    parse_error("Unmatched bracket", doc, begin, pos)
                    break
            elif c == '\n' and not (delim_stack or indent.match(doc, end)):
                # a newline not followed by an indent marks end of signature,
                # except for within brackets
                signature = doc[begin:pos].strip()
                if signature and signature not in signatures:
                    push_signature(o, signatures, signature)
                    begin = end
                else:
                    # blank line means no more signatures in docstring
                    break
        else:
            # reached the end of the docstring
            end = len(doc)
            if not delim_stack:
                signature = doc[begin:pos].strip()
                if signature and signature not in signatures:
                    push_signature(o, signatures, signature)
            else:
                parse_error("Unmatched bracket", doc, begin, pos)
                break

        # advance position within docstring and return to head of loop
        pos = end

    return signatures

def get_constructors(c):
    """Get constructors from the class documentation.
    """
    constructors = []
    name = c.__name__
    doc = c.__doc__

    if not doc or not doc.startswith(name + "("):
        return constructors
    signatures = get_signatures(c)
    for signature in signatures:
        if signature.startswith(name + "("):
            signature = re.sub("-> " + name, "-> None", signature)
            if signature.startswith(name + "()"):
                constructors.append(re.sub(name + r"\(", "__init__(self", signature, 1))
            else:
                constructors.append(re.sub(name + r"\(", "__init__(self, ", signature, 1))
    return constructors

def add_indent(s, indent):
    """Add the given indent before every line in the string.
    """
    return indent + re.sub(r"\n(?=([^\n]))", "\n" + indent, s)

def make_def(s, indent):
    """Generate a method definition stub from the signature and an indent.
    The indent is a string (tabs or spaces).
    """
    pos = 0
    out = ""
    while pos < len(s) and s[pos] == '@':
        end = s.find('\n', pos) + 1
        if end == 0:
            end = len(s)
        out += indent
        out += s[pos:end]
        pos = end
    if pos < len(s):
        out += indent
        out += "def "
        out += s[pos:]
        out += ": ..."
    return out

def class_pyi(c):
    """Generate all the method stubs for a class.
    """
    bases = [b.__name__ for b in c.__bases__]
    out = "class " + c.__name__ + "(" + ", ".join(bases) + "):\n"
    count = 0

    # do all nested classes (these are usually enum types)
    items = sorted(c.__dict__.items())
    others = []
    for m,o in items:
        if isclass(o) and m == o.__name__:
            out += add_indent(class_pyi(o), "    ")
            count += 1
        else:
            others.append((m, o))

    # do all constants
    items = others
    others = []
    for m,o in items:
        if not m.startswith("__") and not ismethod(o) and not isclass(o):
            out += "    " + m + ":" + type(o).__name__ + "\n"
            count += 1
        else:
            others.append((m,o))

    # do the __init__ methods
    constructors = get_constructors(c)
    if len(constructors) == 0:
        #if hasattr(c, "__init__") and not issubclass(c, int):
        #    out += "    def __init__() -> None: ...\n"
        #    count += 1
        pass
    else:
        count += 1
        if len(constructors) == 1:
            out += make_def(constructors[0], "    ") + "\n"
        else:
            for overload in constructors:
                out += make_def("@overload\n" + overload, "    ") + "\n"

    # do the methods
    items = others
    others = []
    for m,o in items:
        if ismethod(o):
            signatures = get_signatures(o)
            if len(signatures) == 0:
                continue
            count += 1
            if len(signatures) == 1:
                 out += make_def(signatures[0], "    ") + "\n"
                 continue
            for overload in signatures:
                 out += make_def("@overload\n" + overload, "    ") + "\n"
        else:
            others.append((m, o))

    if count == 0:
        out = out[0:-1] + " ...\n"

    return out

def module_pyi(mod):
    """Generate the contents of a .pyi file for a VTK module.
    """
    sys.stdout.write("from typing import overload, Any, Callable\n\n")
    sys.stdout.write("any = Any\n")
    sys.stdout.write("callable = Callable[..., None]\n")
    sys.stdout.write("\n")

    # do all constants
    items = sorted(mod.__dict__.items())
    others = []
    for m,o in items:
        if not m.startswith("__") and not ismethod(o) and not isclass(o):
            sys.stdout.write(m + ":" + type(o).__name__ + "\n")
        else:
            others.append((m,o))
    if len(items) > len(others):
       sys.stdout.write("\n")

    # do all classes
    items = others
    others = []
    for m,o in items:
        if isclass(o) and m == o.__name__:
            sys.stdout.write(class_pyi(o))
            sys.stdout.write("\n")
        else:
            others.append((m, o))

    # do all aliases
    items = others
    others = []
    for m,o in items:
        if isclass(o) and m != o.__name__:
            sys.stdout.write(m + " = " + o.__name__ + "\n")
        else:
            others.append((m, o))

mod = importlib.import_module("vtkmodules." + sys.argv[1])
module_pyi(mod)
