import re, os, textwrap

BOOK = '/Volumes/Evo/Lecture Notes/SKEE3223 Book'
HEAD = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'preamble.tex')).read()

def scaffold(n):
    s = open(os.path.join(BOOK, 'chapter%d.tex' % n)).read()
    d = {}
    d['title'] = re.search(r'\\chapter\{(.+?)\}', s).group(1)
    m = re.search(r'\\begin\{whymatters\}(.*?)\\end\{whymatters\}', s, re.S)
    d['why'] = ' '.join(m.group(1).split()) if m else ''
    m = re.search(r'\\begin\{objlist\}(.*?)\\end\{objlist\}', s, re.S)
    d['obj'] = [' '.join(x.split()) for x in re.split(r'\\item\s', m.group(1))[1:]] if m else []
    m = re.search(r'\\designchallenge\{(.*?)\n\}', s, re.S)
    d['dc'] = m.group(1).strip() if m else ''
    return d

REFMAP = {
    'eq:cpu_time': 'the CPU-time equation',
    'eq:array_address': 'the array-address formula',
    'sec:embedded_systems': 'Section 1.1',
    'sec:sequential_trace': 'the execution trace',
    'sec:register_set': 'the register set',
    'sec:memory_mapped_io': 'the memory-mapped I/O section',
}

def wrapdc(dc):
    """Turn a designchallenge body into slide-safe content.

    Cross-references into the book do not resolve in a standalone deck, so
    \\ref{...} becomes readable prose rather than rendering as '??'.
    """
    def deref(m):
        lab = m.group(1)
        return REFMAP.get(lab, lab.split(':')[-1].replace('_', ' '))
    dc = re.sub(r'(?:Equation|Section|Table|Figure|Chapter)~\\ref\{([^}]+)\}', deref, dc)
    dc = re.sub(r'\\ref\{([^}]+)\}', deref, dc)
    parts = [' '.join(p.split()) for p in dc.split(r'\par\medskip\noindent')]
    return [p for p in parts if p]

def head_for(n, d):
    h = HEAD
    h = h.replace('\\title{From Digital Systems to Microcontrollers}', '\\title{%s}' % d['title'])
    h = h.replace('--- Chapter 1}', '--- Chapter %d}' % n)
    return h

def frames_common(n, d):
    """The four scaffolding frames every chapter shares."""
    out = []
    out.append("""\\titlepage

\\begin{frame}{Table of Contents}
    \\tableofcontents
\\end{frame}

%%===============================================================================
\\section{Why This Matters}
%%===============================================================================

\\begin{frame}%s{Why This Chapter Matters}
    \\leavevmode
    %s
\\end{frame}

\\begin{frame}{Objectives}
    \\leavevmode
    {\\small After completing this chapter, you should be able to:}

    \\vspace{2mm}
    {\\small
    \\begin{enumerate}\\itemsep2pt
%s
    \\end{enumerate}}
\\end{frame}
""" % ('[fragile]' if '\\verb' in d['why'] else '',
       d['why'],
       '\n'.join('        \\item %s' % o for o in d['obj'])))
    return ''.join(out)

def frame_dc(n, d):
    parts = wrapdc(d['dc'])
    brief = parts[0] if parts else ''
    rest  = parts[1:] if len(parts) > 1 else []
    body = "    \\begin{block}{The brief}\n        %s\n    \\end{block}\n" % brief
    for p in rest:
        body += "\n    \\vspace{2mm}\n    {\\small %s}\n" % p
    return """%%===============================================================================
\\section{Design Challenge}
%%===============================================================================

\\begin{frame}{Design Challenge}{To be worked on paper, before any code}
    \\leavevmode
%s\\end{frame}

\\begin{frame}[plain]
    \\begin{center}
        \\vfill
        {\\usebeamerfont{title}\\color{primary}\\Huge Questions?}
        \\vfill
        {\\small\\color{Gray} \\emph{Introduction to ARM Cortex-M Microprocessors} --- Chapter %d}
        \\vfill
    \\end{center}
\\end{frame}

\\end{document}
""" % (body, n)

if __name__ == '__main__':
    import sys
    n = int(sys.argv[1])
    d = scaffold(n)
    print(head_for(n, d) + frames_common(n, d))
