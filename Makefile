CXX = clang
LD = clang

target = ow
depends = main.o

CXXFLAGS = -Wall -g
LIBS = -lboost_program_options-mt -lc++

$(target):$(depends)
	@echo [LD] $(target)
	@$(LD) -o $(target) $(depends) $(LIBS)

.cpp.o:
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) -c $<

clean:
	@echo Cleaning up...
	@rm -f $(target) $(depends)
	@echo done
