INSTALL=$(PWD)/install
BUILD=$(PWD)/build
SENTINELS=$(PWD)/sentinels
SOURCE=$(PWD)/source
PATCH=$(PWD)/patches
JOBS=1

DEPENDENCIES = rose or-tools isl loopchainir boost

.PHONY: all clean clean-build clean-install clean-sentinal printvars $(DEPENDENCIES)

all: $(DEPENDENCIES)

printvars:
	@echo INSTALL=$(INSTALL)
	@echo BUILD=$(BUILD)
	@echo SENTINELS=$(SENTINELS)
	@echo PATCH=$(PATCH)
	@echo JOBS=$(JOBS)
	@echo DEPENDENCIES = $(DEPENDENCIES)

$(DEPENDENCIES): % : $(SENTINELS)/%

$(SENTINELS)/loopchainir: $(addprefix $(SENTINELS)/,or-tools isl rose)
	- /bin/rm -rf $(BUILD)/loopchainir
	cp -r $(SOURCE)/loopchainir $(BUILD)/loopchainir
	cd $(BUILD)/loopchainir && \
	./configure --prefix=$(INSTALL) && \
	make -j$(JOBS) && \
	make test && \
	make install
	touch $@
	ls $(SENTINELS)
	- /bin/rm -rf $(BUILD)/loopchainir

$(SENTINELS)/rose: $(addprefix $(SENTINELS)/,boost)
	- /bin/rm -rf $(BUILD)/rose $(BUILD)/rose_build
	cp -r $(SOURCE)/rose $(BUILD)/rose
	cd $(BUILD) && \
	mkdir rose_build && \
	cd rose && \
	patch ./config/support-java.m4 $(PATCH)/support-java.m4.openjdk_fix.patch && \
	./build && \
	cd ../rose_build && \
	../rose/configure --without-java --without-doxygen --without-gfortran --without-php --with-boost=$(INSTALL) --prefix=$(INSTALL) && \
	make -j$(JOBS) && \
	make install && \
	cp -rf $(INSTALL)/include/rose/* $(INSTALL)/include/. && \
	/bin/rm -rf $(INSTALL)/include/rose/
	touch $@
	ls $(SENTINELS)
	- /bin/rm -rf $(BUILD)/rose $(BUILD)/rose_build

$(SENTINELS)/boost:
	- /bin/rm -rf $(BUILD)/boost
	cp -r $(SOURCE)/boost $(BUILD)/boost
	cd $(BUILD)/boost && \
	./bootstrap.sh --prefix=$(INSTALL) --with-libraries=chrono,date_time,filesystem,iostreams,program_options,random,regex,serialization,signals,system,thread,wave && \
	./b2 -j$(JOBS) && \
	./b2 install
	touch $@
	ls $(SENTINELS)
	- /bin/rm -rf $(BUILD)/boost

$(SENTINELS)/or-tools:
	- /bin/rm -rf $(BUILD)/or-tools
	cp -r $(SOURCE)/or-tools $(BUILD)/or-tools
	cd $(BUILD)/or-tools && \
	make -j$(JOBS) third_party && \
	make -j$(JOBS) cc && \
	make -j$(JOBS) test_cc && \
	find -regex '\.\/ortools\/.*\.h\(pp\)?' -exec cp --parent {} $(INSTALL)/include \; && \
	cp -r dependencies/install/* $(INSTALL)/. && \
	cp -r lib/* $(INSTALL)/lib/. && \
	cd ortools/gen && \
	find -regex '\.\/ortools\/.*\.h\(pp\)?' -exec cp --parent {} $(INSTALL)/include \; && \
	cd ../.. && \
	/bin/rm -r $(INSTALL)/include/ortools/gen
	touch $@
	ls $(SENTINELS)
	- /bin/rm -rf $(BUILD)/or-tools

$(SENTINELS)/isl:
	- /bin/rm -rf $(BUILD)/isl
	cp -r $(SOURCE)/isl $(BUILD)/isl
	cd $(BUILD)/isl && \
	./configure --prefix=$(INSTALL) && \
	make -j$(JOBS) && \
	make install
	touch $@
	ls $(SENTINELS)
	- /bin/rm -rf $(BUILD)/isl

clean-build:
	- /bin/rm -rf $(BUILD)/*

clean-install:
	- /bin/rm -rf $(INSTALL)/*
	mkdir $(addprefix $(INSTALL)/, lib bin include)

clean-sentinal:
	- /bin/rm $(SENTINELS)/*

clean: clean-build clean-install clean-sentinal
