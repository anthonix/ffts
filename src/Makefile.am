
OBJLIBS = libffts.a
HDRS = cp_sse.h patterns.h macros.h neon_float.h codegen.h neon.h
FILES = cp_sse.c patterns.c codegen.c neon.s
OBJS = cp_sse.o patterns.o codegen.o neon.o

all: $(OBJLIBS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $< -I../include -g
	$(CC) $(CFLAGS) -S $< -I../include

%.o: %.s 
	$(CC) $(CFLAGS) -c -o $@ $< -I../include -g
	
$(OBJLIBS): $(OBJS) 
	$(AR) rcs libffts.a $(OBJS)

clean:
	$(RM) -f *.o $(OBJLIBS)
