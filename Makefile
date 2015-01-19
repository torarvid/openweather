CXX = g++

target = ow
depends = main.o

CXXFLAGS = -Wall -g
LIBS = -lpthread

$(target):$(depends)
	@echo [LD] $(target)
	@$(CXX) -o $(target) $(depends) $(LIBS)

.cpp.o:
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) -c $<

clean:
	@echo Cleaning up...
	@rm -f $(target) $(depends)
	@echo done
