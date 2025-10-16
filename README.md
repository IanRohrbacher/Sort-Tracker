# Sort Tracker

A small C++ project to benchmark and track sorting algorithms over time. The program collects timing data and writes JSON output files in the repository so you can visualize or analyze sorting performance across runs.

This repository contains a compact runner and some utility code that uses the header-only JSON library by nlohmann to serialize results.

## Repository layout

- `sort.cpp` - main runner that executes sorts and records timings.
- `time.cpp` - timing helpers used by the runner.
- `sortAlgorithm.hpp` - sorting algorithm implementations / helpers.
- `nlohmann/` - bundled header-only nlohmann::json library used for JSON I/O.
- `data.json` - primary data file (sample / result store).
- `data_time.json` - per-run timing results.
- `Makefile` - convenience build + run recipe (Linux/WSL/macOS).

## Requirements

- A C++17 capable compiler (g++, clang++, ect.).
- `make` (recommended on Unix-like systems). On Windows you can build with MinGW or use Visual Studio.
- Optional: `screen` (Linux) to run the process detached.

## Build & run

On Linux (recommended):

```sh
make
# The Makefile in this project is intended to start a fresh dataset and run continuously
```

If you prefer to compile manually using g++:

```sh
g++ -std=c++17 -I nlohmann sort.cpp time.cpp -o sort_tracker
./sort_tracker
```

Note: The exact runtime flags or files produced depend on the Makefile and platform. The project outputs/updates `data.json` and `data_time.json` as it runs.

## Makefile targets

The included `Makefile` provides a few configurable variables at the top and several convenient targets you can run with `make`.

Configurable variables (defaults are set in the Makefile):

- `json` — JSON output filename (default: `data_time.json`).
- `backup` — how many loops before creating a backup of the JSON file (default: `50`).
- `time` — wait time between each sort/tick (default: `0`).

Common make targets and what they do:

- `make` / `make all` — default target. The Makefile's default runs `reset-run-forever` (reset the JSON and run continuously).
- `make build` — compile the code to the compiled filename (`sort.out` by default).
- `make run-tick` — build and run a single tick: it executes the compiled binary with arguments that run one iteration and then exit.
- `make run-forever` — build and run the program in continuous mode (the program itself controls whether it loops).
- `make reset` — build, remove the current JSON file, then run the program once to create/initialize a fresh JSON file.
- `make reset-run-tick` / `make reset-run-forever` — compound targets that reset then run (single tick or forever respectively).
- `make backup-run-tick` / `make backup-run-forever` — restore from `$(json).bak` and then run the chosen mode.
- `make resort-backup` — copy `$(json).bak` to `$(json)` to restore a previous run.
- `make clean` — remove the compiled binary and `output.txt`.
- `make clean-all` — remove the compiled binary, `output.txt`, and both `$(json)` and `$(json).bak`.

Examples:

```sh
# Start fresh and run continuously (default):
make reset-run-forever

# Build and run a single tick while writing to a custom file:
make json=custom.json run-tick

# Run forever but wait 2 seconds between sorts:
make time=2 run-forever
```

## Running in background (Linux / screen)

If you want the program to keep running after you disconnect from the machine, use `screen`:

```sh
screen    # start a new screen session
make      # or run the produced binary inside the screen session
# Press Ctrl-a then d to detach
screen -ls           # list screen sessions
screen -r <session>  # reattach a session
screen -XS <session> quit  # stop a session by id
```

## Data files

- `data.json` and `data_time.json` hold results produced by the runner. Keep backups of these files if you need to preserve historic runs (`*.bak` files may already be present).
- You can parse these JSON files using scripts, import them into spreadsheets, or use a small visualization that reads the JSON output.

## Credits

- JSON serialization via the excellent header-only library nlohmann/json: https://github.com/nlohmann/json
	- The `nlohmann/` folder in this repo includes the required headers from that project. Please consult their repo for license and detailed usage notes.

## Contributing

If you'd like to improve this project open a PR with your changes.
