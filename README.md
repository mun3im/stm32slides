# Slides — lecture companion to the book

Beamer decks that follow *Introduction to ARM Cortex-M Microprocessors*
chapter for chapter. One deck per chapter, `stm01.tex` … `stm14.tex` — named
`stmNN` (not `chNN`) to stay distinct from the book's own `chapterN.tex` and
from `../ARM Neo/`'s `chN-*.tex` decks.

- **Template** is the Flux beamer theme, style **`gray`** (charcoal headers,
  green accent) — distinguishing these from `../ARM Neo/`, which follows the
  coordinator's 13 modules in `red`.
- **Scope** is *lecture companion*: the chapter's spine, not a reading
  substitute. 17–27 slides per chapter.
- **Running hardware** matches the book: Nucleo-F446RE, LED on PA5, button on
  PC13, core at 16 MHz.

## Build

```bash
pdflatex stm01.tex
pdflatex stm01.tex    # twice, for \tableofcontents
```

Prefix with `caffeinate -i` for long unattended runs.

## Structure

Every deck follows the book's chapter template:

| Slides | Content | Source in the book |
|---|---|---|
| 1–2 | Title, table of contents | — |
| 3 | Why This Chapter Matters | `\begin{whymatters}` |
| 4 | Objectives | `\begin{objlist}` |
| 5–n | Body: the chapter's figures, tables and worked examples | `\section`s |
| n+1 | Summary | — |
| last | Design Challenge | `\designchallenge{...}` |

The scaffolding frames (why/objectives/design challenge) are **extracted
directly from the book** by `gen.py`, so they cannot drift out of sync. Rerun
the generator after editing a chapter's `whymatters`, `objlist` or
`designchallenge`.

## `gen.py`

Lives in `/tmp/gen.py` during a session; regenerate it if lost. It provides:

- `scaffold(n)` — pulls title, whymatters, objectives and design challenge
  out of `chapter<n>.tex`
- `head_for(n, d)` — the shared preamble, retitled, with `style=gray`
- `frames_common(n, d)` — titlepage, TOC, whymatters, objectives
- `frame_dc(n, d)` — design challenge and closing slide

Two things it handles that are easy to get wrong:

- **`\ref{...}` in a design challenge** does not resolve in a standalone deck
  and renders as `??`. `REFMAP` replaces known labels with readable prose.
- **`\verb` in a whymatters** requires `\begin{frame}[fragile]`; the generator
  adds it automatically.

## Deck sizes

| Deck | Pages | Chapter |
|---|---|---|
| stm01 | 27 | From Digital Systems to Microcontrollers |
| stm02 | 25 | The ARM Cortex-M4 Programmer's Model |
| stm03 | 24 | Memory, Addressing and Pointers |
| stm04 | 21 | Embedded C and CMSIS |
| stm05 | 23 | ARM Assembly Fundamentals |
| stm06 | 22 | Memory, Arithmetic and Bit Manipulation |
| stm07 | 19 | Control Flow, Functions and the Stack |
| stm08 | 24 | General Purpose Input Output |
| stm09 | 22 | Interrupts and Event-Driven Programming |
| stm10 | 22 | Timers, Timing and PWM |
| stm11 | 17 | ADC and Analog Interfacing |
| stm12 | 19 | UART Communication |
| stm13 | 18 | SPI and I²C |
| stm14 | 21 | Designing, Building and Verifying |

## Theme gotchas

All of these compile **silently** — no error, no warning — so page through the
built PDF before teaching from it.

1. **A frame body starting with a brace group** (`{\small ...}`) is swallowed
   and rendered as a *subtitle in the title bar*. Put `\leavevmode` first.
2. **A second `\\` inside a nested `{...}` group in a TikZ node** breaks with
   `\tikzscope@linewidth undefined`, reported at `\end{frame}`.
3. **`{\small ...}` around a nested `itemize`** eats the frame title and list.
4. **`\usebeamercolor[fg]{title}` on a `[plain]` frame** falls through to an
   unstyled blue. Use `\color{primary}`.
5. **`\newcolumntype` must live in the preamble**, never inside a frame.
6. **`lstlisting` and `\verb` need `[fragile]`.**
7. **`\alert` inside a narrow `p{}` cell** desynchronises table rows. Shade the
   column with `>{\columncolor{...}}` instead.
8. **A TikZ picture wider than its column** silently overflows into its
   neighbour — and a `scale=` factor can introduce this without warning.
9. **`lstlangarm.sty` references `\color{mauve}` but never defines it**, and
   sets `commentstyle=green!20`, illegible on the pink listing background. Both
   are fixed in the preamble.
10. **Wide, short figures** (bit diagrams, register maps) are crushed by a
    `height=` constraint in a two-column layout. Give them full width, stacked.
11. **A `\begin{frame}[title]{subtitle}` pair whose combined length is too wide**
    silently blanks the entire title bar (title text disappears, only the logo
    box remains) — this happens with or without `[plain]`, and produces no
    warning in the log. Reproduced in isolation: the same title alone or the
    same subtitle alone renders fine; only the *combination* overflows the
    fixed-width `beamercolorbox` header, which does not wrap. Fix: shorten the
    subtitle (or the title) until the pair fits on one line.
