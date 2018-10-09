import math
env = math

def setenv(pak):
    global env
    env = pak

writebuf = ""
def modexec(string):
    global writebuf
    global env
    def write(string2):
        global writebuf
        writebuf += string2

    exec(string)
    writebuf2 = writebuf
    writebuf = ""
    return writebuf2

def getindent(lines, indentchar=' '):
    def getindentln(line):
        if len(line) < 1:
            return -1
        tabcounter = 0
        for i in range(len(line)):
            curchar = line[i]
            nextchar = ''
            try:
                nextchar = line[i+1]
            except:
                return 0

            if curchar == indentchar:
                tabcounter += 1

            if nextchar != indentchar:
                return tabcounter


    tabcounter = 111111111
    for ln in lines:
        r = getindentln(ln)
        if r == -1: continue
        if r < tabcounter: tabcounter = r

    return tabcounter


def indent(lines, count=1, indentchar=' '):
    ret = []
    if count < 0:
        ret = [ x[-count : len(x)] for x in lines ]
    else:
        ret = [ (' '*count) + x for x in lines ]

    return ret

execbuf = []
def mod(string, multiline_delims=("/*<python>", "</python>*/"), delims=("<python>", "</python>")):
    output_lines = []
    def output(string):
        output_lines.append(string)
    def mergeoutput():
        return "\n".join(output_lines)
    def found(ln, delim):
        return (ln.find(delim) != -1)
    def evalpyth(string):
        return eval(string)

    def execpyth(lines):
        global execbuf
        if len(lines) < 1:
            return "\x1B[31m" + "Error" + "\x1B[0m"
        ln = lines[0]
        if found(ln, multiline_delims[1]):
            #return execbuf
            execbuf2 = execbuf
            execbuf = []
            ident = getindent(execbuf2)
            env.setindent(ident)
            return modexec(
            "\n".join(
                    indent(execbuf2, -ident)
                )
            )
            #return getindent(execbuf)
            #return "\n".join(execbuf)

        execbuf.append(ln)

        del lines[0]
        return execpyth(lines)

    def interpretln(lines):
        if len(lines) < 1:
            return mergeoutput()
        ln = lines[0]
        if found(ln, multiline_delims[0]):
            del lines[0]
            ln = execpyth(lines)
        elif found(ln, delims[0]) and found(ln, delims[1]):
            return evalpyth (ln [ln.find(delims[0]) : ln.find(delims[1])] )

        output(ln)
        del lines[0]

        return interpretln(lines)

    return interpretln(string.split('\n'))
