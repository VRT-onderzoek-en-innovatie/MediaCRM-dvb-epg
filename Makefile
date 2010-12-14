all: eit
clean:
	-rm *.o
	-rm eit

test:
	$(MAKE) -C tests test

eit: main.o section_buffer.o Buffer.o eit_processor.o CRC32.o Event.o Descriptor.o Descriptor-Unknown.o
	g++ -o $@ $+
