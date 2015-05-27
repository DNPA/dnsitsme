SERVERCFILES = main.cpp UnPrivileged.cpp MainServer.cpp DnsResponse.cpp
SERVERHFILES = UnPrivileged.hpp MainServer.hpp DnsResponse.hpp cachemap.hpp 
CFILES = $(SERVERCFILES)
HFILES =  $(SERVERHFILES)
LIBFLAGS = -lboost_system -lboost_regex -pthread

all: dnsitsme

dnsitsme: $(CFILES) $(HFILES)
	g++ $(CFILES) $(LIBFLAGS) -o dnsitsme


