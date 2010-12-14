all: eit
clean:
	-rm *.o
	-rm eit

test:
	$(MAKE) -C tests test

DESCRIPTORS := Descriptor Unknown ShortEvent
eit: main.o section_buffer.o Buffer.o eit_processor.o CRC32.o Event.o $(foreach d,$(DESCRIPTORS),Descriptor/$d.o)
	g++ -o $@ $+
