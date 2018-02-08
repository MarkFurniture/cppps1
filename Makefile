CC := g++
SRCDIR := src
BUILDDIR := build
INCLUDEDIR := include
TARGETDIR := bin
TARGET := cppps1
LIBS := config++
ABSTARGET := $(realpath $(TARGETDIR)/$(TARGET))

SRCEXT := cpp
HEXT := h
DEPS := $(shell find $(INCLUDEDIR) -type f -name *.$(HEXT))
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
# OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(INCLUDEDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall -Werror -std=c++11
INC := -I $(INCLUDEDIR)
INSTALLDIR := /usr/local/bin
INSTALLTARGET := cppps1

$(TARGET): $(OBJECTS)
	@echo "Building..."
	@echo " $(CC) $^ -o $(TARGETDIR)/$(TARGET)"; $(CC) $^ -o $(TARGETDIR)/$(TARGET) -l $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

install:
	@echo " ln -s $(ABSTARGET) $(INSTALLDIR)/$(INSTALLTARGET)"
	@ln -s $(ABSTARGET) $(INSTALLDIR)/$(INSTALLTARGET)

uninstall:
	@echo " rm $(INSTALLDIR)/$(INSTALLTARGET)"
	@rm $(INSTALLDIR)/$(INSTALLTARGET)

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGETDIR)/$(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)