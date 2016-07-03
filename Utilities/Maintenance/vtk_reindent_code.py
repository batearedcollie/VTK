#!/usr/bin/env python
"""
Usage: python vtk_reindent_code.py [--test] <file1> [<file2> ...]

This script takes old-style "Whitesmiths" indented VTK source files as
input, and re-indents the braces according to the new VTK style.
Only the brace indentation is modified.

If called with the --test option, then it will print an error message
for each file that it would modify, but it will not actually modify the
files.

Written by David Gobbi on Sep 30, 2015.
"""

import sys
import os
import re

def reindent(filename, dry_run=False):
    """Reindent a file from Whitesmiths style to Allman style"""

    # The first part of this function clears all strings and comments
    # where non-grammatical braces might be hiding.  These changes will
    # not be saved back to the file, they just simplify the parsing.

    # look for ', ", /*, and //
    keychar = re.compile(r"""[/"']""")
    # comments of the form /* */
    c_comment = re.compile(r"\/\*(\*(?!\/)|[^*])*\*\/")
    c_comment_start = re.compile(r"\/\*(\*(?!\/)|[^*])*$")
    c_comment_end = re.compile(r"^(\*(?!\/)|[^*])*\*\/")
    # comments of the form //
    cpp_comment = re.compile(r"\/\/.*")
    # string literals ""
    string_literal = re.compile(r'"([^\\"]|\\.)*"')
    string_literal_start = re.compile(r'"([^\\"]|\\.)*\\$')
    string_literal_end = re.compile(r'^([^\\"]|\\.)*"')
    # character literals ''
    char_literal = re.compile(r"'([^\\']|\\.)*'")
    char_literal_start = re.compile(r"'([^\\']|\\.)*\\$")
    char_literal_end = re.compile(r"^([^\\']|\\.)*'")

    # read the file
    try:
        f = open(filename)
        lines = f.readlines()
        f.close()
    except:
        sys.stderr.write(filename + ": ")
        sys.stderr.write(str(sys.exc_info()[1]) + "\n")
        sys.exit(1)

    # convert strings to "", char constants to '', and remove comments
    n = len(lines) # 'lines' is the input
    newlines = []  # 'newlines' is the output

    cont = None    # set if e.g. we found /* and we are looking for */

    for i in range(n):
        line = lines[i].rstrip()

        if cont is not None:
            # look for closing ' or " or */
            match = cont.match(line)
            if match:
                # found closing ' or " or */
                line = line[match.end():]
                cont = None
            else:
                # this whole line is in the middle of a string or comment
                if cont is c_comment_end:
                    # still looking for */, clear the whole line
                    newlines.append("")
                    continue
                else:
                    # still looking for ' or ", set line to backslash
                    newlines.append('\\')
                    continue

        # start at column 0 and search for ', ", /*, or //
        pos = 0
        while True:
            match = keychar.search(line, pos)
            if match is None:
                break
            pos = match.start()
            end = match.end()
            # was the match /* ... */ ?
            match = c_comment.match(line, pos)
            if match:
                line = line[0:pos] + " " + line[match.end():]
                pos += 1
                continue
            # does the line have /* ... without the */ ?
            match = c_comment_start.match(line, pos)
            if match:
                if line[-1] == '\\':
                    line = line[0:pos] + ' \\'
                else:
                    line = line[0:pos]
                cont = c_comment_end
                break
            # does the line have // ?
            match = cpp_comment.match(line, pos)
            if match:
                if line[-1] == '\\':
                    line = line[0:pos] + ' \\'
                else:
                    line = line[0:pos]
                break
            # did we find "..." ?
            match = string_literal.match(line, pos)
            if match:
                line = line[0:pos] + "\"\"" + line[match.end():]
                pos += 2
                continue
            # did we find "... without the final " ?
            match = string_literal_start.match(line, pos)
            if match:
                line = line[0:pos] + "\"\"\\"
                cont = string_literal_end
                break
            # did we find '...' ?
            match = char_literal.match(line, pos)
            if match:
                line = line[0:pos] + "\' \'" + line[match.end():]
                pos += 3
                continue
            # did we find '... without the final ' ?
            match = char_literal_start.match(line, pos)
            if match:
                line = line[0:pos] + "\' \'\\"
                cont = char_literal_end
                break
            # if we got to here, we found / that wasn't /* or //
            pos += 1

        # strip any trailing whitespace!
        newlines.append(line.rstrip())

    # The second part of this function looks for braces in the simplified
    # code that we wrote to "newlines" after removing the contents of all
    # string literals, character literals, and comments.

    # Whenever we encounter an opening brace, we push its position onto a
    # stack.  Whenever we encounter the matching closing brace, we indent
    # the braces as a pair.

    # For #if directives, we check whether there are mismatched braces
    # within the conditional block, and if so, we print a warning and reset
    # the stack to the depth that it had at the start of the block.

    # For #define directives, we save the stack and then restart counting
    # braces until the end of the #define.  Then we restore the stack.

    # all changes go through this function
    lines_changed = {} # keeps track of each line that was changed
    def changeline(i, newtext, lines_changed=lines_changed):
         if newtext != lines[i]:
              lines[i] = newtext
              lines_changed[i] = newtext

    # we push a tuple (delim, row, col, newcol) onto this stack whenever
    # we find a {, (, or [ delimiter, this keeps track of where we found
    # the delimeter and what column we want to move it to
    stack = []
    lastdepth = 0

    # this is a superstack that allows us to save the entire stack when we
    # enter into an #if conditional block
    dstack = []

    # these are syntactic elements we need to look for
    directive = re.compile(r" *# *(..)")
    label = re.compile(r"""(\s*(case[(\s]+)?(' '|""|\w|\s*::\s*)+[)\s]*:)+$""")
    cflow = re.compile(r"\s*(if|else|for|do|while|switch)(\W|\Z)")
    delims = re.compile(r"[{}()\[\]]")
    spaces = re.compile(r" *")
    cplusplus = re.compile(r" *# *ifdef  *__cplusplus")

    lastpos = 0  # previous indentation column
    newpos = 0   # current indentation column
    continuation = False   # true if line continues an unfinished statement
    new_context = True     # also set when we enter a #define statement
    in_else = False        # set if in an #else
    in_define = False      # set if in #define
    in_assign = False      # set to deal with "= {" or #define x {"
    leaving_define = False # set if at the end of a #define
    save_stack = None      # save stack when entering a #define

    for i in range(n):
        line = newlines[i]
        pos = 0 # column position

        # restore stack when leaving #define
        if leaving_define:
            stack, lastpos, newpos, continuation = save_stack
            save_stack = None
            in_define = False
            leaving_define = False

        # handle #if conditionals
        is_directive = False
        in_else = False
        match = directive.match(line)
        if match:
            is_directive = True
            if match.groups()[0] == 'if':
                dstack.append((list(stack), line))
            elif match.groups()[0] in ('en', 'el'):
                oldstack, dline = dstack.pop()
                if len(stack) > len(oldstack) and not cplusplus.match(dline):
                    sys.stderr.write(filename + ":" + str(i) + ": ")
                    sys.stderr.write("mismatched delimiter in \"" +
                                     dline + "\" block\n")
                if match.groups()[0] == 'el':
                    in_else = True
                    stack = oldstack
                    dstack.append((list(stack), line))
            elif match.groups()[0] == 'de':
                in_define = True
                leaving_define = False
                save_stack = (stack, lastpos, newpos, continuation)
                stack = []
                new_context = True

        # remove backslash at end of line, if present
        if len(line) > 0 and line[-1] == '\\':
            line = line[0:-1].rstrip()
        elif in_define:
            leaving_define = True

        if not is_directive and len(line) > 0 and not continuation:
            # what is the indentation of the current line?
            match = spaces.match(line)
            newpos = match.end()

        # check for end of statement vs. continuation of statement
        if len(line) > 0:
            # if #define, {, }, ;, flow control keyword, or label
            if (new_context or line[-1] in ('{', '}', ';') or
                cflow.match(line) or label.match(line)):
                continuation = False
                new_context = False
            elif not is_directive:
                continuation = True

        # search for braces
        while True:
            match = delims.search(line, pos)
            if match is None:
                break
            pos = match.start()
            delim = line[pos]
            if delim in ('(', '['):
                # save delim, row, col, and current indentation
                stack.append((delim, i, pos, newpos))
            elif delim == '{':
                if in_assign:
                    # do not adjust braces for initializer lists
                    stack.append((delim, i, pos, pos))
                elif ((in_else or in_define) and spaces.sub("", line) == "{"):
                    # for opening braces that might have no match
                    indent = " "*lastpos
                    changeline(i, spaces.sub(indent, lines[i], count=1))
                    stack.append((delim, i, lastpos, lastpos))
                else:
                    # save delim, row, col, and previous indentation
                    stack.append((delim, i, pos, lastpos))
                newpos = pos + 2
                lastpos = newpos
            else:
                # found a ')', ']', or '}' delimiter, so pop its partner
                try:
                    ldelim, j, k, newpos = stack.pop()
                except IndexError:
                    ldelim = ""
                if ldelim != {'}':'{', ')':'(', ']':'['}[delim]:
                    sys.stderr.write(filename + ":" + str(i) + ": ")
                    sys.stderr.write("mismatched \'" + delim + "\'\n")
                # adjust the indentation of matching '{', '}'
                if (ldelim == '{' and delim == '}' and
                    spaces.sub("", lines[i][0:pos]) == "" and
                    spaces.sub("", lines[j][0:k]) == ""):
                    indent = " "*newpos
                    changeline(i, spaces.sub(indent, lines[i], count=1))
                    changeline(j, spaces.sub(indent, lines[j], count=1))
            pos += 1

        # check for " = " and #define assignments for the sake of
        # the { inializer list } that might be on the following line
        if len(line) > 0:
            if (line[-1] == '=' or
                (is_directive and in_define and not leaving_define)):
                in_assign = True
            elif not is_directive:
                in_assign = False

        lastpos = newpos

    if len(dstack) != 0:
        sys.stderr.write(filename + ": ")
        sys.stderr.write("mismatched #if conditional.\n")

    if len(stack) != 0:
        sys.stderr.write(filename + ":" + str(stack[0][1]) + ": ")
        sys.stderr.write("no match for " + stack[0][0] +
                         " before end of file.\n")

    if lines_changed:
        # remove any trailing whitespace
        trailing = re.compile(r" *$")
        for i in range(n):
            lines[i] = trailing.sub("", lines[i])
        while n > 0 and lines[n-1].rstrip() == "":
            n -= 1
        if dry_run:
            errcount = len(lines_changed)
            line_numbers = list(lines_changed.keys())
            line_numbers.sort()
            line_numbers = [str(l + 1) for l in line_numbers[0:10] ]
            if errcount > len(line_numbers):
                line_numbers.append("...")
            sys.stderr.write("Warning: " + filename +
                             ": incorrect brace indentation on " +
                             str(errcount) +
                             (" lines: ", "line: ")[errcount == 1] +
                             ", ".join(line_numbers) + "\n")
        else:
            # rewrite the file
            ofile = open(filename, 'w')
            ofile.writelines(lines)
            ofile.close()
        return True

    return False


if __name__ == "__main__":

    # ignore generated files
    ignorefiles = ["lex.yy.c", "vtkParse.tab.c"]

    files = []
    opt_ignore = False # ignore all further options
    opt_test = False # the --test option

    for arg in sys.argv[1:]:
        if arg[0:1] == '-' and not opt_ignore:
            if arg == '--':
                opt_ignore = True
            elif arg == '--test':
                opt_test = True
            else:
                sys.stderr.write("%s: unrecognized option %s\n" %
                                 (os.path.split(sys.argv[0])[-1], arg))
                sys.exit(1)
        elif os.path.split(arg)[-1] not in ignorefiles:
            files.append(arg)

    # if --test was set, whenever a file needs modification, we set
    # "failed" and continue checking the rest of the files
    failed = False

    for filename in files:
        # repeat until no further changes occur
        while reindent(filename, dry_run=opt_test):
            if opt_test:
                failed = True
                break

    if failed:
        sys.exit(1)
