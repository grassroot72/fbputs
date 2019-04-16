# fbputs makefile

CC = gcc
CFLAGS = -O2 -Wall `pkg-config --cflags freetype2` -D DEMO -D UNICODE_INFO
LDFLAGS = `pkg-config --libs freetype2`
OBJS = apex_memmove.o \
       fb.o \
       font.o \
       sb.o \
       unicode.o \
       fbputs.o
EXE = fbputs


all: $(EXE)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(EXE)
