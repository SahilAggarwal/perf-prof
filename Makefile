INCDIR = -Iinclude
SRCDIR = src
OBJDIR = obj
BINDIR = bin

CC = gcc
LINKER = gcc -o
TARGET = perf-prof

rm = rm -f

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJDIR)/cpu_map.o $(OBJDIR)/perf_prof.o $(OBJDIR)/perf_event.o $(OBJDIR)/mmap_page.o $(OBJDIR)/output.o
	$(CC) $(INCDIR) $^ -lpthread -g -o $@

$(OBJDIR)/cpu_map.o: $(SRCDIR)/cpu_map.c
	$(CC) $(INCDIR) -c $(SRCDIR)/cpu_map.c -g -o $@

$(OBJDIR)/perf_prof.o: $(SRCDIR)/perf_prof.c
	$(CC) $(INCDIR) -c $(SRCDIR)/perf_prof.c -g -o $@

$(OBJDIR)/perf_event.o: $(SRCDIR)/perf_event.c
	$(CC) $(INCDIR) -c $(SRCDIR)/perf_event.c -g -o $@

$(OBJDIR)/mmap_page.o: $(SRCDIR)/mmap_page.c
	$(CC) $(INCDIR) -c $(SRCDIR)/mmap_page.c -g -o $@

$(OBJDIR)/output.o: $(SRCDIR)/output.c
	$(CC) $(INCDIR) -c $(SRCDIR)/output.c -g -o $@

clean:
	rm $(OBJDIR)/*
