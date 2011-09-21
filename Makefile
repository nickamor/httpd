# Nick Amor - 2011

# Unless it's a non-target, each directory makes up a target of the form %/%,
# where % is the directory name
# All .c files in a directory are compiled to make the target binary
# All .c files in the root directory are compiled and included in all targets

TARGETS = $(filter-out $(NONTARGETS), $(patsubst %/,%,$(shell ls -d */)))
BINARIES = $(addprefix **/, $(TARGETS))

NONTARGETS = httpdoc

.PHONY: all $(TARGETS) clean
all: $(TARGETS)

OSTYPE = $(shell uname)

ifeq "$(OSTYPE)" "SunOS"
	TARGETS = server-single
	MAKE = gmake
endif

$(TARGETS):
	@$(MAKE) -C $@ -f ../common.mk all TARGET=$@

clean:
	@$(RM) *.o *~ **/*.o **/*~
	@$(RM) $(BINARIES)

help:
	@echo Targets: $(TARGETS)
	@echo Binaries: $(BINARIES)
	@echo Make: $(MAKE)
