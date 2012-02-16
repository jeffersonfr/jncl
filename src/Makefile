include ../Makefile.defs

MODULES = \
		  tuner\
		  demux\
		  media\
		  formatter\

all:
	@for i in $(MODULES); do \
		$(ECHO) "Compiling module $$i " && (cd $$i && make -s) && $(ECHO) "Module $$i $(OK)" && continue || $(ECHO) "Module $$i $(FAILED)" && exit; \
	done

tests:
	@for i in $(MODULES); do \
		$(ECHO) "Compiling $$i/tests " && (cd $$i/tests && make -s) && $(ECHO) "$$i $(OK)" && continue || $(ECHO) "$$i $(FAILED)" && exit; \
	done

install:
	@mkdir -p $(PREFIX)/jginga
	@for i in $(MODULES); do \
		$(ECHO) -n "Installing $$i " && (cd $$i && make -s install) && $(ECHO) "$$i installed $(OK)" && continue || $(ECHO) "$$i installed $(FAILED)" && exit; \
	done

clean:
	@for i in $(MODULES); do \
		$(ECHO) "Cleaning module $$i " && (cd $$i && make -s clean) && (cd $$i/tests && make -s clean) && $(ECHO) "$$i cleaned $(OK)" && continue || $(ECHO) "$$i cleaned $(FAILED)" && exit; \
	done

ultraclean:
	@for i in $(MODULES); do \
		$(ECHO) "Cleaning module $$i " && (cd $$i && make -s ultraclean) && (cd $$i/tests && make -s ultraclean) && $(ECHO) "$$i ultra-cleaned $(OK)" && continue || $(ECHO) "$$i ultra-cleaned $(FAILED)" && exit; \
	done
