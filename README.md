# scanportsd
daemon for scan ports
all inforamtion about statuses will be write to database

# compile
Test compile was in Ubuntu 14.04

	$ qmake && make

# command line

	--help - this help
	--createdb - create new database on your mysql server by root
	--updatedb - will be update your database
	--example-config - print example
	--run - run scaning

# init database

	$ ./scanportsd --createdb
	$ ./scanportsd --updatedb

if you has database already then just update:
 
 	$ ./scanportsd --updatedb

# example config

	$ ./scanportsd --example-config

# run

	$ sudo ./scanportsd --run > /dev/null
