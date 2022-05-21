TARGET	= chipical

CC		= gcc
CFLAGS	= -Wall -O0 -lncurses
LINKER	= gcc
LFLAGS	= -Wall

SRCDIR	= src
OBJDIR	= obj
BINDIR	= bin

SOURCES	:= $(wildcard $(SRCDIR)/*.c)
INCLUDE := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): $(OBJECTS) $(BINDIR)
	$(LINKER) $(OBJECTS) $(LFLAGS) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

.PHONY: remove
remove: clean
	rm -rf $(BINDIR)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)
