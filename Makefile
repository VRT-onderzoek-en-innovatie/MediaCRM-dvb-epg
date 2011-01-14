all: eit xml_chop
clean:
	-rm -f Makefile.includes
	-rm -f *.o
	-rm -f eit xml_chop

test:
	$(MAKE) -C tests test

Makefile.includes:
	-rm -f $@
	find . -name '*.cpp' | while read s; do echo "$${s%%cpp}o: $${s}" >> $@; done
	find . -name '*.c' | while read s; do echo "$${s%%c}o: $${s}" >> $@; done
	find . -name '*.cpp' -o -name '*.c' -o -name '*.hpp' -o -name '*.h' | while read f; do \
		/bin/echo -n "$$f: " >> $@; \
		D="`dirname "$$f"`/"; \
		grep '^#include "' $$f | sed 's@^#include "\(.*\)"@'$$D'\1@' | tr '\n' ' ' >> $@; \
		echo "" >> $@; \
		echo "	touch \$$@" >> $@; \
	done
.PHONY: Makefile.includes
include Makefile.includes

DESCRIPTORS := Descriptor Unknown ShortEvent
eit: main.o section_buffer.o Buffer.o eit_processor.o CRC32.o Event.o util.o $(foreach d,$(DESCRIPTORS),Descriptor/$d.o)
	g++ -o $@ -liconv $+

xml_chop: xml_chop.o
	g++ -o $@ -lexpat $+
