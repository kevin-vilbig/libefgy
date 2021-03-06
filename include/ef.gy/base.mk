# vim: set noexpandtab:

DESTDIR:=
PREFIX:=/usr/local
BINDIR:=$(DESTDIR)$(PREFIX)/bin
INCLUDEDIR:=$(DESTDIR)$(PREFIX)/include
MANDIR:=$(DESTDIR)$(PREFIX)/share/man
DOWNLOADS:=./.downloads

# define these in your project!
NAME:=
BASE=$(NAME)
VERSION:=1

STYLE:=llvm

# standard programmes (may or may not be used)
CC:=$(shell which clang false | head -n 1)
CLANG_FORMAT:=$(shell which clang-format clang-format-3.5 clang-format-3.4 false | head -n 1) -style=$(STYLE)
CSSMIN:=$(shell which cssmin false | head -n 1)
CURL:=$(shell which curl false | head -n 1) -s
CXX:=$(shell which clang++ false | head -n 1)
EMXX:=$(shell which em++ false | head -n 1)
GIT:=$(shell which git false | head -n 1)
INSTALL:=$(shell which install false | head -n 1)
PKGCONFIG:=$(shell which pkg-config false | head -n 1)
TIDY:=$(shell which tidy false | head -n 1)
XSLTPROC:=$(shell which xsltproc false | head -n 1)

UNAME:=$(shell uname)
LIBRARIES:=
JSFUNCTIONS:=

DEBUG:=false

CXX_STANDARD:=c++1z
CXX_STDLIB:=-stdlib=libc++
PCCFLAGS=$(shell $(PKGCONFIG) --cflags $(LIBRARIES) 2>/dev/null)
PCLDFLAGS=$(shell $(PKGCONFIG) --libs $(LIBRARIES) 2>/dev/null)
CFLAGS=-O2 $(shell if $(DEBUG); then echo '-g'; fi)
CXXFLAGS:=$(CXX_STDLIB) $(CFLAGS)
EMCFLAGS:=-O2 --llvm-lto 3
EMXXFLAGS=$(EMCFLAGS)
LDFLAGS=

DATABASE:=
BINARIES_SRC_TEST:=$(wildcard src/test-case/*.cpp)
BINARIES_SRC_PROPER:=$(wildcard src/*.cpp)
BINARIES_SRC:=$(BINARIES_SRC_PROPER) $(BINARIES_SRC_TEST)
BINARIES:=$(basename $(notdir $(wildcard src/*.cpp)) $(addprefix test-case-,$(notdir $(wildcard src/test-case/*.cpp))))
JSBINARIES=$(addsuffix .js,$(BINARIES))
TESTBINARIES=$(filter test-case-%,$(BINARIES))

IGNOREBINARIES:=server
IBINARIES:=$(addprefix $(BINDIR)/,$(filter-out $(IGNOREBINARIES) test-case-%,$(BINARIES)))
IINCLUDES:=$(addprefix $(INCLUDEDIR)/$(BASE)/,$(notdir $(wildcard include/$(BASE)/*.h)))
IMANPAGES:=$(addprefix $(MANDIR)/man1/,$(notdir $(wildcard src/*.1)))

DATAHEADERS=$(wildcard include/data/*.h)

# don't delete intermediary files
.SECONDARY:

.PHONY: all clean scrub archive install uninstall

# meta rules
all: $(DATABASES) $(BINARIES)
clean:
	rm -f $(DATABASES) $(BINARIES); true
scrub: clean
	rm -rf dependencies.mk

install: $(IBINARIES) $(IINCLUDES) $(IMANPAGES)
uninstall:
	rm -f $(IBINARIES) $(IINCLUDES) $(IMANPAGES)

archive: ../$(NAME)-$(VERSION).tar.gz

../$(NAME)-$(VERSION).tar.gz:
	$(GIT) archive --format=tar --prefix=$(NAME)-$(VERSION)/ HEAD | gzip -9 >$@

# meta rules for documentation
documentation documentation/xml/combine.xslt documentation/xml/index.xml: doxyfile $(wildcard include/*/* xslt/doxy*)
	doxygen $<

documentation.xml: documentation/xml/combine.xslt documentation/xml/index.xml xslt/doxygen*
	xsltproc documentation/xml/combine.xslt documentation/xml/index.xml | xsltproc xslt/doxygen-post-process.xslt - > $@

# meta rules to reformat sources
format:
	$(CLANG_FORMAT) -i $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard include/$(BASE)/*.h)

# meta rules for javascript
js: $(JSBINARIES)

#run unit tests
test: $(addprefix run-,$(TESTBINARIES))

run-test-case-%: test-case-%
	@echo TEST BATCH: $*
	@./$^
	@echo PASSED

# pattern rules to install things
$(BINDIR)/%: %
	$(INSTALL) -D $< $@

$(INCLUDEDIR)/%: include/%
	$(INSTALL) -D $< $@

$(MANDIR)/man1/%.1: src/%.1
	$(INSTALL) -D $< $@

# pattern rules for databases
%.sqlite3: src/%.sql
	rm -f $@ && $(SQLITE3) $@ < $<

# pattern rules for C++ code
%: src/%.cpp
	$(CXX) -std=$(CXX_STANDARD) -Iinclude/ $(CXXFLAGS) $(PCCFLAGS) $< $(LDFLAGS) $(PCLDFLAGS) -o $@ && ($(DEBUG) || strip -x $@)

test-case-%: src/test-case/%.cpp
	$(CXX) -std=$(CXX_STANDARD) -Iinclude/ -DRUN_TEST_CASES $(CXXFLAGS) $(PCCFLAGS) $< $(LDFLAGS) $(PCLDFLAGS) -o $@

%.js: src/%.cpp include/*/*.h
	$(EMXX) -std=$(CXX_STANDARD) -Iinclude/ -D NOLIBRARIES $(EMXXFLAGS) -s EXPORTED_FUNCTIONS="$(JSFUNCTIONS)" $< $(LDFLAGS) -o $@

# dependency calculations
dependencies.mk: $(BINARIES_SRC) include/*/*.h $(DATAHEADERS) include/ef.gy/base.mk makefile
	($(CXX) -std=$(CXX_STANDARD) -Iinclude/ $(PCCFLAGS) -MM -MG $(BINARIES_SRC_PROPER) | sed -E 's/(.*).o: /\1: /' || true) > $@
	($(CXX) -std=$(CXX_STANDARD) -Iinclude/ $(PCCFLAGS) -MM -MG $(BINARIES_SRC_TEST) | sed -E 's/(.*).o: /test-case-\1: /' || true) >> $@

# downloads
$(DOWNLOADS)/.volatile:
	mkdir -p $(DOWNLOADS) || true
	touch $@

doxyfile: makefile
	echo 'DOXYFILE_ENCODING = UTF-8' > $@
	echo 'PROJECT_NAME = "$(NAME)"' >> $@
	echo 'PROJECT_NUMBER = "$(VERSION)"' >> $@
	echo 'OUTPUT_DIRECTORY = documentation' >> $@
	echo 'ALLOW_UNICODE_NAMES = YES' >> $@
	echo 'INLINE_INHERITED_MEMB = YES' >> $@
	echo 'STRIP_FROM_PATH = .' >> $@
	echo 'STRIP_FROM_INC_PATH = include/' >> $@
	echo 'TAB_SIZE = 2' >> $@
	echo 'BUILTIN_STL_SUPPORT = YES' >> $@
	echo 'EXTRACT_STATIC = YES' >> $@
	echo 'QUIET = YES' >> $@
	echo 'INPUT = README.md include/$(BASE) src src/test-case' >> $@
	echo 'IMAGE_PATH = documentation/' >> $@
	echo 'SOURCE_BROWSER = YES' >> $@
	echo 'COLS_IN_ALPHA_INDEX = 3' >> $@
	echo 'HTML_FILE_EXTENSION = .xhtml' >> $@
	echo 'SEARCHENGINE = NO' >> $@
	echo 'GENERATE_XML = YES' >> $@
	echo 'GENERATE_LATEX = NO' >> $@
	echo 'GENERATE_TAGFILE = $(NAME).tag' >> $@
	echo 'DOT_IMAGE_FORMAT = svg' >> $@
	echo 'INTERACTIVE_SVG = YES' >> $@
	[ -e "documentation/doxygen-layout.xml" ] && echo 'LAYOUT_FILE = documentation/doxygen-layout.xml' >> $@ || true
	[ -e "documentation/extra.css" ] && echo 'HTML_EXTRA_STYLESHEET  = documentation/extra.css' >> $@ || true
	[ -e "doxyfile-extra" ] && echo '@INCLUDE = doxyfile-extra' >> $@ || true

doxygen:: doxyfile
	doxygen $<

# just in case we want to compress something
%.gz: %
	gzip -kf9n $<

# provide a wrapper for experimental/optional to plain optional
optional: include/optional

include/optional:
	echo '#include <experimental/optional>' > $@
	echo 'namespace std { using experimental::optional; }' >> $@

include dependencies.mk
