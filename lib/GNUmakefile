COHORT_HOME = ../

include $(COHORT_HOME)/mk/Make.defs

lib_targets := $(shell ls src | \tr ' ' '\n' | $(SED) '/GNUmakefile/d' | sort | uniq)

$(lib_targets):
	@echo "Checking $@ library..."
	@$(MAKE) --no-print-directory --directory=src/$@ lib

printvars:
	@echo "lib_targets         : $(lib_targets)"

clean:
	@for x in $(lib_targets); do \
          $(MAKE) --no-print-directory --directory=src/$$x clean; \
          done;
	-$(RM) $(addprefix lib,$(addsuffix .$(_lib_ext),$(lib_targets)))
