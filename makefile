# This defines the compiler
CXX = g++

# -Wall is Warning all which just enables all compilation warnings
# -std=c++11 has the compiler use c++11
CXXFLAGS = -Wall -std=c++11

# now we can do make all or just make to run these 4 commands
all: d4client d4server d3client server

d4client: d4client.cpp
	$(CXX) $(CXXFLAGS) -o d4client d4client.cpp

d4server: d4server.cpp
	$(CXX) $(CXXFLAGS) -o d4server d4server.cpp

d3client: d3client.cpp
	$(CXX) $(CXXFLAGS) -o d3client d3client.cpp

server: server.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp

# These files will be zipped
ZIP_SOURCES = d4client.cpp d4server.cpp d3client.cpp server.cpp readme.txt makefile images

zip:
	rm -f assignment1.zip
	zip -r assignment1.zip $(ZIP_SOURCES) -x "*.DS_Store"

# so we can do make clean
clean:
	rm -f d4client d4server d3client server

# Add .PHONY to mark targets that don't represent files
.PHONY: all clean zip