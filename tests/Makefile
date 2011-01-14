TESTS := CRC

test: $(TESTS)
	for t in $(TESTS); do ./$$t; done

clean:
	-rm -f *.o
	-rm -f $(TESTS)

CRC: CRC.o ../CRC32.o
	$(CXX) $(CXXFLAGS) -o $@ $+
