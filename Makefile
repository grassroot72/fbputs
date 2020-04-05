# fbputs makefile

CC = gcc
CFLAGS = -O2 -Wall `pkg-config --cflags freetype2` -D DEMO
LDFLAGS = `pkg-config --libs freetype2`
OBJS = apex_memmove.o \
       fb.o \
       font.o \
       sb.o \
       unicode.o \
       fbputs.o
EXES = fbputs

all: ${EXES}

${EXES}: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	${CC} -o $@ -c $< $(CFLAGS)

clean:
	$(RM) *.o $(EXES)
