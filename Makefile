CC := g++
SRCDIR := src
BUILDDIR := build
TARGETDIR := bin
TARGET := cppps1
ABSTARGET := $(realpath $(TARGETDIR)/$(TARGET))

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall -Werror -std=c++11
INC := -I include
INSTALLDIR := /usr/local/bin
INSTALLTARGET := cppps1

$(TARGET): $(OBJECTS)
	@echo "Building..."
	@echo " $(CC) $^ -o $(TARGETDIR)/$(TARGET)"; $(CC) $^ -o $(TARGETDIR)/$(TARGET)

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
	@echo " $(RM) $(INSTALLDIR)/$(INSTALLTARGET)"; $(RM) $(INSTALLDIR)/$(INSTALLTARGET)