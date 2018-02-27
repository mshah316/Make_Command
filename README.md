# Make_Command
I implement my own linux make command using an acyclic graph algorithm
To begin the make proccess of linking files girst I parse through the makefile looking 
for depdendecy relationships to setup my acyclic graph. Once I have found dependecies I check for loops in
graph and exit if they exist because the graph needs to be acyclic to impliment the make command.

Commands Available:
  time: This command reports the current value of the clock.
  touch <filename>: This command changes the time stamp on the specifed filename to the current
  clock value is and increments the clock. 
  timestamp <filename>: This prints the time stamp on the specifed file.
  timestamps: Prints the timestamps of all files.
  make <target>: This command takes the target and brings it up to date.
  quit: quits the fakemake program.
