FastRoute4-lefdef
======================

**FastRoute4-lefdef** is an open-source global router.

The algorithm base is from FastRoute4, and the underlying infrastructure comes from [Rsyn](https://github.com/RsynTeam/rsyn-x/)



The FastRoute4 version was received June 15, 2019, with the BSD-3 open source license as given in the [website](http://home.eng.iastate.edu/~cnchu/FastRoute.html#License).

[Rsyn](https://github.com/RsynTeam/rsyn-x/) version is based on commit `1087c918e4fa14db84fc3b4c91210db96b07bb4c` and release under Apache License, Version 2.0 as given in its [repository](https://github.com/RsynTeam/rsyn-x/blob/master/README.md).

Any code additional to rsyn will clearly specify its license in each file and in this README. We ask contributors to seriously consider using the BSD-3 Licence.

## Getting Started
### Pre-Requisite

- GCC compiler
- boost library
- cmake3.1

### How to Compile

````
cd FastRoute4-lefdef/
make PARALLEL=nthreads CMAKE_OPT='optional parameters for cmake'
````

### Basic Usage

Your command line to run FastRoute4-lefdef should look like this:

````
./FRlefdef --no-gui --script path/to/script.rsyn
````

You can find a script example in `rsyn/support/fastroute/example.rsyn`

#### Script details

The basic format of a script to run FastRoute is show below:

````
open "generic" {
	"lefFiles" : "example.lef",
	"defFiles" : "example.def"
};
run "rsyn.fastRoute" {"outfile" : "example.guide", "adjustment" : 0.X, "maxRoutingLayer" : Y};
````

FastRoute have three parameters. Details of each one of them can be found below:
- outfile: name of the file with the generated guides
- adjustment: percentage reduction in capacity of each edge
- maxRoutingLayer: maximum routing layer available for FastRoute

If you need more than one LEF/DEF file, you can write a script like that:

````
open "generic" {
	"lefFiles" : ["example1.lef", "example2.lef", ..., "exampleN.lef"],
	"defFiles" : ["example1.def", "example2.def", ..., "exampleN.def"]
};
run "rsyn.fastRoute" {"outfile" : "example.guide", "adjustment" : 0.X, "maxRoutingLayer" : Y};
````

You can use either the absolute path or the relative path (relative to the script file) for LEF/DEF files.

Copyright (c) 2019, Federal University of Rio Grande do Sul (UFRGS)
All rights reserved.
