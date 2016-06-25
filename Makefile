include src/semver.mk

######

NAME := gottext
TITLE := GotText

CP := cp -R
RM := rm -f
MKDIR := mkdir -p
ECHO := echo

EXTENSION := ${NAME}.so

ifeq ($(filter test test_installed doc, ${MAKECMDGOALS}),)

	VER_PAT := $(shell git rev-list HEAD ^$(shell git log --pretty=format:%H -1 src/semver.mk) --count src/*.h src/*.cpp)

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
		ifeq (${PHP_VER_MAJ}, 5)
			PHPENMOD := php5enmod
			MODS_SUBDIR := php5
		else
			$(error Unsupported PHP version: ${PHP_VER_MAJ}.${PHP_VER_MIN})
		endif
	endif

	SRC_DIR := src

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

	COMPILER_FLAGS := -c -fPIC -std=c++0x -Wall
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

endif

DOC_DIR := doc
DOC_DIR_SOURCE := ${DOC_DIR}/source
DOC_DIR_API := ${DOC_DIR}/api
TEST_FILE := test/test.php

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
	if [ "${INI_DIR}" ]; \
	then \
		${RM} ${INI_DIR}/${INI}; \
	fi

.PHONY: clean
clean:
	${RM} ${EXTENSION} ${OBJECTS}

.PHONY: test
test:
	php -dzend.assertions=1 -dextension=./${EXTENSION} ${TEST_FILE}

.PHONY: test_installed
test_installed:
	php -dzend.assertions=1 ${TEST_FILE}

.PHONY: doc
doc: ${DOC_DIR_API}/index.html
${DOC_DIR_API}/index.html: ${DOC_DIR_SOURCE}/${NAME}.php
	${ECHO} | apigen generate \
		--source ${DOC_DIR_SOURCE} \
		--destination ${DOC_DIR_API} \
		--title="${TITLE} Documentation" \
		--debug
