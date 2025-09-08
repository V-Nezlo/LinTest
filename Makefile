all:
	$(CXX) $(CFLAGS) -o testpro src/testpro.cpp
install:
	mkdir -p $(DESTDIR)/usr/bin
	install -m 0755 testpro $(DESTDIR)/usr/bin/
