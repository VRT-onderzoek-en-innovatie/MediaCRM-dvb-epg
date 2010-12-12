TESTS := CRC

test: $(TESTS)
	for t in $(TESTS); do ./$$t; done

CRC: CRC.o ../CRC32.o
	$(CXX) $(CXXFLAGS) -o $@ $+
