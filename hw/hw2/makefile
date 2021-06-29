TARGET := shell
OBJS := shell.o
SRCS := shell.c

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