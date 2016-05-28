# build test hg
# to build on a PC run "make PCRUN=-DPCRUN" 
#

TARGET=test_hg

CC=gcc
OPTIMIZE = -O0
prod: OPTIMIZE = -O3
PROFILE =
prof: PROFILE = -pg # for gprof profiling
pcrun: PCRUN=-DPCRUN
CFLAGS = $(PROFILE) -std=c11 -g -Wall -Werror $(OPTIMIZE)
CPPFLAGS = $(PROFILE) $(PCRUN) -std=c++14 -g -Wall -Werror $(OPTIMIZE) 
OBJECTS = Geom.o
LDFLAGS =  $(PROFILE)  
LDLIBS = 


all: pcrun

pcrun: $(TARGET)

prod: pcrun

prof: cleand pcrun 
	./$(TARGET) d
	gprof ./$(TARGET)  > test_hg.profile
	cat ./test_hg.profile | less

$(TARGET): $(OBJECTS)

clean:
	@rm -f ./$(TARGET)
	@rm -f ./*.o


cleand: clean
	@rm -f ./gmon.out
	@rm -f ./*.profile

memchk: all
	valgrind -v --dsymutil=yes --track-origins=yes \
                 --leak-check=full --show-leak-kinds=all ./$(TARGET) d

display: clean pcrun
	./$(TARGET) display

