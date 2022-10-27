BCFLAGS= -W -Wall -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -fstack-protector-all -Wfloat-equal -Wpointer-arith -Wwrite-strings -Wcast-align -Wno-format -Wno-long-long -std=gnu99 -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wbad-function-cast -Wnested-externs #-Wdeclaration-after-statement
BXXFLAGS= -g -std=c++1z -W -Wall -Wextra -Wcast-align -Wfloat-equal -Wpointer-arith -Wunused -Wwrite-strings -Wformat-nonliteral -Wformat-security -Wmissing-format-attribute -Wvla -Wformat=1 -Werror -Wno-maybe-uninitialized -Wnon-virtual-dtor -Wuninitialized -Wlogical-op -Wsuggest-override -Wno-unused-local-typedefs -Wshadow
CC= gcc -O3 $(BCFLAGS)


all: convert

convert: src/convert.c
	$(CC) -o convert src/convert.c

clean:
	rm -f convert
	find * -name \*~ -delete

