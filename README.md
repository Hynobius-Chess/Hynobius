<div align="center">
    <img src="docs/img/icon-128.png">
    <br>
    <h3>Hynobius</h3>
    A chess engine with tutorials, documents, scientific testing and web analysis interface.
</div>

# Hynobius

A C++ chess engine with **tutorials**, **documents**, **scientifc testing** and **web analysis interface**.

- Search optimizations.
- Experimentations and scientific testing pipeline.
- Web-based analysis.

---

## Overview

### [Full Documents Right Here](https://hynobius-chess.github.io/docs/)

### Hynobius Chess Engine

It is a chess engine focus on:

- Effective search. (alpha-beta, move ordering, TT, iterative deepening, aspiration window, LMR).
- Basic evaluations. (material score, PST)

### Hynobius Testing

It is a testing system using github actions, **[Cute Chess](https://github.com/cutechess/cutechess)**, and self-hosted runner.

### Hynobius Analysis

It is a website held by githib pages, and use `WASM` to compile the engine.

Try **[Hynobius Analysis](https://hynobius-chess.github.io/Hynobius-analysis/)** right now!

---

## Features

### Search

- Alpha-beta prunting with iterative deepeing.
- Move ordering (PV move, TT move, killer moves).
- Transportation table (TT).
- Aspiration window.
- Late move reduction (LMR).
- Quietscene search with check evasions.

### Evaluation

- Material scores.
- Basic Piece-square tables (PST).

### Scientific Testing

- Automatic AB testing with **[Cute Chess](https://github.com/cutechess/cutechess)** and GitHub actions.
- Elo tracking.
- Self-hosted runner.

### Web Analysis

- WASM-compiled engine running is browser.
- Interface board and PGN analysis.

---

## Usage

### CLI & UCI

Build:

```bash
cmake -B build
cmake --build build
```

Run:

```bash
./Hynobius.exe
```

Use UCI commands:

```bash
isready
position startpos moves e2e4
go depth 6
```

Use CLI commands

```bash
./Hynobius.exe --search-with-info
./Hynobius.exe --perft
```

---

## Project Structure

---

## Acknowledgements

### Resources

- **Bhupen** with his mate-in-one FEN uploaded **[on kaggle.com](https://www.kaggle.com/datasets/ancientaxe/mate-in-one-chess?resource=download)**.

### Tools

- **[Cute Chess](https://github.com/cutechess/cutechess)** with its easy-to-use interface for playing chess.
- **[Lichess](https://Lichess.org)** with its platform to set up bots and play with other engines.

### Inspirations

- **[Stockfish](https://github.com/official-stockfish/stockfish)** with its clean structure and implementations.

### AIs

- **[ChatGPT (project attached)](https://chatgpt.com/g/g-p-69381984e7988191afa09bafbf015c43-c-cheng-shi-shi-zuo-xi-yang-qi-fen-xi-yin-qing/project)** with its detailed help.
