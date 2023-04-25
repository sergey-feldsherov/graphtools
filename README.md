# graphtools

## TODO

* write some tests 
* renumerate
	* rewrite in C
* backnumerate
	* add to project
* edgelist2mtx
	* add detection of undirected edgelist graphs
	* add a toggle to check for indices being 1-based or 0-based
	* add a toggle to write IDs "as-is" (if range is valid) or
	to choose to renumerate (push out 0-degree vertices)
	* progress bar? (with file size check?)
	* think about re-implementing format checking (double file read = bad)
		* add block file reading?
* graphstat
  * implement in C or C++?
* convert
	* is it really needed actually?
	* C hashmap implementation to store graph internally
	* How to hint the input and output graph formats?
		* If the hint will be done with an option, then
		probably long options should be used for
		readability. Current option parser can only handle
		short options (aka flags).

## renumerate

Read an edgelist and renumerate it (squash vertex set indices so all isolated
vertices are "pushed out" from the graph (or move to the end of index set).

## backnumerate

TODO

## edgelist2mtx

Read an edgelist file, write to a MatrixMarket-formatted file.

## graphstat

Read a graph stored as an ASCII text file (or STDIN) and print some information about it.

## convert

Read a graph from a file and write it to a file (or STDOUT) in another format.

