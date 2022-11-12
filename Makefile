ifeq (${shell uname},Darwin)
	CC=g++ -g -Wall -fno-builtin -std=c++17 -D_XOPEN_SOURCE
	LIBVM_APP=libvm_app_macos.o
	LIBVM_PAGER=libvm_pager_macos.o
else
	CC=g++ -g -Wall -fno-builtin -std=c++17
	LIBVM_APP=libvm_app.o
	LIBVM_PAGER=libvm_pager.o
endif

# List of source files for your pager
PAGER_SOURCES=clock.cpp shared.cpp vm_pager.cpp

# Generate the names of the pager's object files
PAGER_OBJS=${PAGER_SOURCES:.cpp=.o}

all: pager app

# Compile the pager and tag this compilation
pager: ${PAGER_OBJS} ${LIBVM_PAGER}
	./autotag.sh
	${CC} -o $@ $^

# Compile an application program
app: test1.4.cpp ${LIBVM_APP}
	${CC} -o $@ $^ -ldl

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${PAGER_OBJS} pager app

debug%: test%.cpp ${LIBVM_APP}
	${CC} -o $@ $^ -g

test%: test%.cpp ${LIBVM_APP}
	${CC} -o $@ $^
