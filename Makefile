ROOT_DIR := $(patsubst %/,%,$(dir $(realpath $(lastword $(MAKEFILE_LIST)))))
SRC_DIR := ${ROOT_DIR}/src

include ${SRC_DIR}/semver.mk

######

NAME := gottext
TITLE := GotText

CP := cp -R
RM := rm -f
RM_EMPTY_DIR := rm -d
MKDIR := mkdir -p
ECHO := echo

EXTENSION := ${NAME}.so
DIST_DIR := dist

ifeq ($(filter test test_installed, ${MAKECMDGOALS}),)

	PHP_VER ?= $(shell php-config --version)
	PHP_VER_SHORT := $(shell ${ECHO} "${PHP_VER}" | sed -e 's/\./ /g')
	PHP_VER_MAJ := $(word 1, ${PHP_VER_SHORT})
	PHP_VER_MIN := $(word 2, ${PHP_VER_SHORT})

	ifeq (${PHP_VER_MAJ},)
		$(error Cannot determine the major version of PHP)
	endif
	ifeq (${PHP_VER_MIN},)
		$(error Cannot determine the minor version of PHP)
	endif
	ifeq (${PHP_VER_MAJ}, 7)
		PHPENMOD := phpenmod
		MODS_SUBDIR := php/${PHP_VER_MAJ}.${PHP_VER_MIN}
	else
		$(error Unsupported PHP version: ${PHP_VER_MAJ}.${PHP_VER_MIN})
	endif

	EXTENSION_DIR := $(shell php-config --extension-dir)

	INI := ${NAME}.ini
	ifndef INI_DIR
		ifneq ($(shell which ${PHPENMOD} 2>/dev/null),)
			INI_DIR := /etc/${MODS_SUBDIR}/mods-available
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

	COMPILER_FLAGS := -c -fPIC -std=c++11 -Wall
	LINKER_FLAGS := -shared

	BUILDTIME := $(shell date +'%s')

	DEFINES += \
		VER_MAJ=${VER_MAJ} \
		VER_MIN=${VER_MIN} \
		VER_PAT=${VER_PAT} \
		VERSION_STR=\"${VER_MAJ}.${VER_MIN}.${VER_PAT}\" \
		PHP_VER_MAJ=${PHP_VER_MAJ} \
		PHP_VER_MIN=${PHP_VER_MIN} \
		PHP_VERSION_STR=\"${PHP_VER_MAJ}.${PHP_VER_MIN}\" \
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
		COMPILER_FLAGS += -ffunction-sections -fdata-sections -fvisibility=hidden -O2
		LINKER_FLAGS += -Wl,-s,-gc-sections
	endif

	ifdef BOOST_REGEX
		DEFINES += GOTTEXT_BOOST_REGEX
		LIBS += boost_regex
	endif

	ifdef PHPCPP_ROOT
		COMPILER_FLAGS += -I ${PHPCPP_ROOT}/include
		LINKER_FLAGS += -L ${PHPCPP_ROOT}/lib
	endif

	OBJECTS := ${SOURCES:${SRC_DIR}/%.cpp=%.o}
	LINKER_LIB_FLAGS := $(addprefix -l, ${LIBS})

	ifdef STANDALONE
		DEFINES += GOTTEXT_STANDALONE
		LINKER_LIB_FLAGS := -Wl,-Bstatic ${LINKER_LIB_FLAGS} -Wl,-Bdynamic
	endif

endif

TEST_FILE := test/test.php

######

.PHONY: all
all: ${OBJECTS} ${DIST_DIR}/${EXTENSION}

${DIST_DIR}/${EXTENSION}: ${OBJECTS}
	${MKDIR} ${DIST_DIR}
	${LINKER} -o $@ $^ ${LINKER_FLAGS} ${LINKER_LIB_FLAGS}

${OBJECTS}:
	${COMPILER} ${COMPILER_FLAGS} $(addprefix -D, ${DEFINES}) -o $@ ${@:%.o=${SRC_DIR}/%.cpp}

.PHONY: install
install:
	${MKDIR} ${EXTENSION_DIR}
	${CP} ${DIST_DIR}/${EXTENSION} ${EXTENSION_DIR}/
	if [ "${INI_DIR}" ]; \
	then \
		${MKDIR} ${INI_DIR}; \
		${CP} ${SRC_DIR}/${INI} ${INI_DIR}/; \
	fi

.PHONY: uninstall
uninstall:
	${RM} ${EXTENSION_DIR}/${EXTENSION}
	if [ "${INI_DIR}" ]; \
	then \
		${RM} ${INI_DIR}/${INI}; \
	fi

.PHONY: clean
clean:
	${RM} ${DIST_DIR}/${EXTENSION} ${OBJECTS}
	-${RM_EMPTY_DIR} ${DIST_DIR}

.PHONY: test
test:
	php -dzend.assertions=1 -dextension=./${DIST_DIR}/${EXTENSION} ${TEST_FILE}

.PHONY: test_installed
test_installed:
	php -dzend.assertions=1 ${TEST_FILE}
