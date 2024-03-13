CXX = g++
CXXFLAGS = -g
PROJECT = fleet
PROJECTNAME = proj5

mytest.exe: $(PROJECT).o mytest.cpp
	$(CXX) $(CXXFLAGS) $(PROJECT).o mytest.cpp -o mytest.exe

$(PROJECT).o: $(PROJECT).h $(PROJECT).cpp
	$(CXX) $(CXXFLAGS) -c $(PROJECT).cpp

clean:
	rm *.o*
	rm *.exe
	rm *~
	rm \#*\#

run:
	./mytest.exe

val:
	valgrind ./mytest.exe

driver:
	make $(PROJECT).o
	$(CXX) $(CXXFLAGS) $(PROJECT).o driver.cpp -o driver.exe

test:
	./driver.exe

testval:
	valgrind ./driver.exe

submit:
	cp $(PROJECT).h $(PROJECT).cpp mytest.cpp ~/341/cs341proj/$(PROJECTNAME)
