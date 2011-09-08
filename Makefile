# Nick Amor - 2011

TARGETS = $(patsubst %/,%,$(shell ls -d */))
BINARIES = $(join $(shell ls -d */), $(TARGETS))

.PHONY: all $(TARGETS) clean
all: $(TARGETS)

$(TARGETS):
	@$(MAKE) -C $@ -f ../common.mk all TARGET=$@

clean:
	@$(RM) *.o *~ **/*.o **/*~
	@$(RM) $(BINARIES)
