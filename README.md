# stm32slides

Beamer slide decks for **SKEE3223**, *Introduction to ARM Cortex-M
Microprocessors* (Universiti Teknologi Malaysia). There is one deck for each
chapter of the course book. They cover the ARM Cortex-M4 on the STM32F446RE
(Nucleo-F446RE board).

## What is in this repository

| Path | What it is |
|---|---|
| `stm01.tex` ... `stm14.tex` | The slide sources. One deck per chapter. |
| `stm01.pdf` ... `stm14.pdf` | The built slides, ready to use. |
| `styles/` | The Flux beamer theme (`gray` style) and `lstlangarm.sty`, the ARM assembly listing language. |
| `assets/` | The images and figures the decks include. |

Only the files needed to read and rebuild the slides are here.

## Decks

| Deck | Pages | Chapter |
|---|---|---|
| `stm01` | 52 | From Digital Systems to Microcontrollers |
| `stm02` | 53 | The ARM Cortex-M4 Programmer's Model |
| `stm03` | 40 | Memory, Addressing and Pointers |
| `stm04` | 23 | Embedded C and CMSIS |
| `stm05` | 49 | ARM Assembly Fundamentals |
| `stm06` | 36 | Data Processing |
| `stm07` | 46 | Control Flow, Functions and the Stack |
| `stm08` | 26 | General Purpose Input Output |
| `stm09` | 30 | Interrupts and Event-Driven Programming |
| `stm10` | 33 | Timers, Timing and PWM |
| `stm11` | 14 | ADC and Analog Interfacing |
| `stm12` | 23 | UART Communication |
| `stm13` | 19 | Synchronous Serial Interfaces: SPI and I²C |
| `stm14` | 19 | Designing, Building and Verifying an Embedded System |

Page counts include the section-divider pages that beamer inserts.

## Build

Each deck is a standalone file. Compile it twice so the table of contents
resolves:

```bash
pdflatex stm02.tex
pdflatex stm02.tex
```

You need a TeX Live installation with `beamer`, `tikz`, `listings` and the
`roboto` and `beramono` fonts. Run the command from the repository root,
because the decks load `styles/` and `assets/` by relative path.

## How a deck is organised

- **Order follows the book.** Each `\section` matches a section of the
  chapter, in the same order.
- **Opening frames:** why the chapter matters, then its objectives.
- **Closing frames:** a summary, then a pointer to the next chapter.
- **Addendum.** Some decks end with `\section{Addendum}`. These frames are
  extra material that the book does not cover yet. To hide them, move
  `\end{document}` up to just before `\section{Addendum}`.
- **Shared look.** The TikZ block styles (`blk`, `cpublk`, `memblk`, `cell`,
  `lbl`, `flow`, `busline`) are repeated at the top of every deck. The slides
  use bright colours for projection.
- **Running example.** LED on PA5, user button on PC13, 16 MHz clock.

## Known problems with the theme

These fail silently. The deck compiles with no error, so check the PDF.

1. A frame body that starts with `{\small ...}` is swallowed into the title-bar
   subtitle. Start the body with `\leavevmode`.
2. A title and subtitle that are too wide together blank the whole title bar.
   Shorten one of them.
3. A TikZ picture wider than its column overflows into the next column.
4. `lstlisting` and `\verb` need `\begin{frame}[fragile]`.
5. `\newcolumntype` must be in the preamble, never inside a frame.
6. Use `language={[ARM]Assembler}` for ARM listings, not `language=ARM`.
