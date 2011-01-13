all: eit xml_chop
clean:
	-rm -f *.o
	-rm -f eit xml_chop

test:
	$(MAKE) -C tests test

DESCRIPTORS := Descriptor Unknown ShortEvent
eit: main.o section_buffer.o Buffer.o eit_processor.o CRC32.o Event.o util.o $(foreach d,$(DESCRIPTORS),Descriptor/$d.o)
	g++ -o $@ -liconv $+

xml_chop: xml_chop.o
	g++ -o $@ -lexpat $+
