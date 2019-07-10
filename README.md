# Logic circuit simulator

This is a logic circuit simulator. It was developed as a project for a university exam, and is now released in the public domain in the hope that it might be helpful to someone.

It features a simulation mode, which simulates the circuit tick-by-tick, and an analysis mode, which finds logic cones as well as the longest and shortest path. Some example files are included in `input/`.

🇮🇹 In `doc/relazione.tex` è inclusa una relazione LaTeX in italiano sugli algoritmi e le strutture dati usate e le scelte progettuali.

The project is released as-is, and no further development will be done.

## Usage

```sh
cmake .
make
```

## Tests

```sh
src/run_tests.sh
```