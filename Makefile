# json file name
json = data_time.json

# how many loop before a backup is create for the json file
backup = 50

# wait time between each sort
time = 0



# DO NOT EDIT BELOW THIS LINE
# -----------------------------------------------------------------------------
# file to build
bogo_in = sort.cpp

# compiled file
compiled = sort.out

# compiler flags
compiler = g++
flags = -I. -std=c++17

# default target
all: reset-run-forever

# targets
build:
	$(compiler) $(flags) $(bogo_in) -o $(compiled)

run-tick: build
	./$(compiled) false false $(json) $(time) $(backup)
run-forever: build
	./$(compiled) false true $(json) $(time) $(backup)

# reset:
reset: build
	rm -f $(json)
	./$(compiled) $(json)
# run the program with a fresh json file
reset-run-tick: reset run-tick
reset-run-forever: reset run-forever
# run the program with a backup
backup-run-tick: resort-backup run-tick
backup-run-forever: resort-backup run-forever

# "clean" is not produced by make
.PHONY: clean clean-all resort-backup

# loads a backup json file
resort-backup:
	cp $(json).bak $(json)

# delete built file
clean:
	rm -f $(compiled) output.txt
clean-all:
	rm -f $(compiled) output.txt $(json) $(json).bak



