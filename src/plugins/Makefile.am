SUBDIRS=parsers imexporters bankinfo backends

sources:
	for d in $(SUBDIRS); do \
	  $(MAKE) -C $$d sources; \
	done


built_sources:
	for d in $(SUBDIRS); do \
	  $(MAKE) -C $$d built_sources; \
	done ;
