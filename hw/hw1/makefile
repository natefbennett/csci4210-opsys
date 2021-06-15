TARGET := hw1
OBJS := hw1-cache.o
SRCS := hw1-cache.c

all: $(TARGET)

$(TARGET): $(OBJS) $(SRCS)
	@echo 'building ' $(TARGET)
	@gcc -o $(TARGET) $(OBJS)
	@echo 'successfully built'

%.o: %.c
	@echo 'building $@ form $<'
	@gcc -o $@ -c $<

clean:
	rm -rf $(OBJS) $(TARGET)