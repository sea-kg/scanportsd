QT	+= core
QT	-= gui
QT	+= sql
QT	+= network

TARGET = scanportsd
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
OBJECTS_DIR = tmp/
MOC_DIR = tmp/

SOURCES +=  \
	src/main.cpp \
	src/ping.cpp \
	src/database/updater.cpp \
	src/database/servers.cpp \


HEADERS += \
	src/ping.h \
	src/database/updater.h \
	src/database/servers.h \
	
