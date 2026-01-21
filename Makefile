CC := cc
CFLAGS := -Wall -O3
LDFLAGS := -lpng -ljpeg -lgoimg -lluajit-5.1

-include config.mk

CSRC = $(wildcard *.c)
HSRC = $(wildcard *.h)

OBJS = $(CSRC:.c=.o)
LIB = libcdhorizon.a
DRIVER = cdhorizon

$(DRIVER): driver/main.c $(LIB)
	$(CC) $(CFLAGS) -o $(DRIVER) driver/main.c $(LIB) $(LDFLAGS)

$(LIB): $(OBJS)
	ar rcs libcdhorizon.a $(OBJS)
	ranlib libcdhorizon.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(DRIVER) $(LIB) $(OBJS)

.PHONY: clean
