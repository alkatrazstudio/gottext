include src/symver.mk

######

NAME := gottext
TITLE := GotText

SRC_DIR := src
TEST_FILE := test/test.php
DOC_DIR := doc
DOC_DIR_SOURCE := ${DOC_DIR}/source
DOC_DIR_API := ${DOC_DIR}/api

EXTENSION := ${NAME}.so
EXTENSION_DIR := $(shell php-config --extension-dir)

INI := ${NAME}.ini
ifndef INI_DIR
	ifneq ($(shell which php5enmod 2>/dev/null),)
		INI_DIR := /etc/php5/mods-available
	else
		ifeq ($(shell test -d /etc/php.d && echo 1), 1)
			INI_DIR := /etc/php.d
		endif
	endif
endif

SOURCES := $(wildcard ${SRC_DIR}/*.cpp)
LIBS := phpcpp

COMPILER ?= g++
ifndef LINKER
	LINKER := ${COMPILER}
endif

COMPILER_FLAGS := -c -fPIC -std=c++0x -Wall
LINKER_FLAGS := -shared

BUILDTIME := $(shell date +'%s')

DEFINES += \
	VER_MAJ=${VER_MAJ} \
	VER_MIN=${VER_MIN} \
	VER_PAT=${VER_PAT} \
	VERSION_STR=\"${VER_MAJ}.${VER_MIN}.${VER_PAT}\" \
	BUILD_TIMESTAMP=${BUILDTIME}

ifdef THREAD_SAFE
	LIBS += boost_thread
else
	DEFINES += GOTTEXT_NO_THREADSAFE
endif

ifdef NATIVE_FILE
	DEFINES += GOTTEXT_EXT_NATIVE_FILE
	SOURCES := $(filter-out ${SRC_DIR}/phpreadstream.cpp,${SOURCES})
endif

ifdef DEBUG
	COMPILER_FLAGS += -g
	LINKER_FLAGS +=
	DEFINES += GOTTEXT_EXT_DEBUG
else
	COMPILER_FLAGS += -ffunction-sections -fdata-sections -fvisibility=hidden -O3
	LINKER_FLAGS += -Wl,-O3,-s,-gc-sections
	ifeq ($(COMPILER),g++)
		COMPILER_FLAGS += -fuse-linker-plugin -flto
		LINKER_FLAGS += -fuse-linker-plugin -flto
	endif
endif

ifdef BOOST_REGEX
	DEFINES += GOTTEXT_BOOST_REGEX
	LIBS += boost_regex
endif

OBJECTS := $(SOURCES:%.cpp=%.o)

CD := cd
CP := cp -R
RM := rm -f
MKDIR := mkdir -p
ECHO := echo

######

.PHONY: all
all: ${OBJECTS} ${EXTENSION}

${EXTENSION}: ${OBJECTS}
	${LINKER} ${LINKER_FLAGS} -o $@ ${OBJECTS} $(addprefix -l, ${LIBS})

${OBJECTS}:
	${COMPILER} ${COMPILER_FLAGS} $(addprefix -D, ${DEFINES}) -o $@ ${@:%.o=%.cpp}

.PHONY: install
install:
	${MKDIR} ${EXTENSION_DIR}
	${CP} ${EXTENSION} ${EXTENSION_DIR}/
	if [ "${INI_DIR}" ]; \
	then \
		${MKDIR} ${INI_DIR}; \
		${CP} ${SRC_DIR}/${INI} ${INI_DIR}/; \
	fi
	
.PHONY: uninstall
uninstall:
	${RM} ${EXTENSION_DIR}/${EXTENSION}
	${RM} ${INI_DIR}/${INI}

.PHONY: clean
clean:
	${RM} ${EXTENSION} ${OBJECTS}

.PHONY: test
test:
	php -dextension=./${EXTENSION} ${TEST_FILE}

.PHONY: test_installed
test_installed:
	php ${TEST_FILE}

.PHONY: doc
doc: ${DOC_DIR_API}/index.html
${DOC_DIR_API}/index.html: ${DOC_DIR_SOURCE}/${NAME}.php
	${ECHO} | apigen generate \
		--source ${DOC_DIR_SOURCE} \
		--destination ${DOC_DIR_API} \
		--title="${TITLE} Documentation" \
		--debug
