LIBS := `root-config --libs`
INCS := `root-config --cflags`
OBJDIR := obj
SRCDIR := src
SRCS := $(SRCDIR)/main.cpp $(SRCDIR)/funzioni.cpp
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

main: $(OBJS)
	g++ -o $@ $^ ${LIBS}

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	g++ -c -o $@ $< ${INCS}

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) main