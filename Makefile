CFLAGS := -W -Wall \
	  -Wformat=2 \
	  -Wunused \
	  -Werror \
	  -Wcast-align \
	  -pedantic-errors \
	  -fstack-protector-all \
	  -Wwrite-strings \
	  -Wcast-align \
	  -Wno-format -Wno-long-long \
	  -std=gnu11 \
	  -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations \
	  -Wold-style-definition \
	  -Wbad-function-cast \
	  -Wnested-externs \
	  -Wfloat-equal \
	  -Wpointer-arith \
#	  -Wdeclaration-after-statement \
#	  -pedantic \

BXXFLAGS := -g -std=c++1z \
	    -W -Wall -Wextra \
	    -Wcast-align -Wfloat-equal \
	    -Wpointer-arith -Wunused \
	    -Wwrite-strings -Wformat-nonliteral \
	    -Wformat-security \
	    -Wmissing-format-attribute -Wvla \
	    -Wformat=1 -Werror \
	    -Wno-maybe-uninitialized \
	    -Wnon-virtual-dtor -Wuninitialized \
	    -Wlogical-op -Wsuggest-override \
	    -Wno-unused-local-typedefs -Wshadow \

CC := gcc $(CFLAGS)
CPP := g++ $(BXXFLAGS)


all: renumerate edgelist2mtx convert graphstat test


renumerate: src/renumerate.cpp
	$(CPP) -O3 -o renumerate src/renumerate.cpp


edgelist2mtx: src/edgelist2mtx.c
	$(CC) -O3 -o edgelist2mtx src/edgelist2mtx.c

convert: src/convert.c
	$(CC) -O3 -o convert src/convert.c

graphstat: src/graphstat.cpp
	$(CPP) -O3 -o graphstat src/graphstat.cpp

test: renumerate input/test.txt output/test.renumerated.valid output/test.new2old.valid output/test.old2new.valid
	./renumerate input/test.txt output/test
	diff output/test.new2old output/test.new2old.valid
	diff output/test.old2new output/test.old2new.valid
	diff output/test.renumerated output/test.renumerated.valid

clean:
	rm -f renumerate edgelist2mtx convert graphstat output/test.renumerated output/test.new2old output/test.old2new
	find * -name \*~ -delete

