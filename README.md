# scanportsd
daemon for scan ports
all inforamtion about statuses will be write to database

# compile
Test compile was in Ubuntu 14.04

	$ qmake && make

# init database

	$ ./scanportsd --createdb
	$ ./scanportsd --updatedb

# example config

	$ ./scanportsd --example-config

# command line

	--help - this help
	--createdb - create new database on your mysql server by root
	--updatedb - will be update your database
	--example-config - print example
	--run - run scaning

# run

	$ sudo ./scanportsd --run > /dev/null
