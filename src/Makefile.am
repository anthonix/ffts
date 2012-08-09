
OBJLIBS = libffts.a
HDRS = cp_sse.h patterns.h macros.h
FILES = cp_sse.c patterns.c 
OBJS = cp_sse.o patterns.o 

all: $(OBJLIBS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $< -I../include
	$(CC) $(CFLAGS) -S $< -I../include
	
$(OBJLIBS): $(OBJS) 
	$(AR) rcs libffts.a $(OBJS)

clean:
	$(RM) -f *.o $(OBJLIBS)
