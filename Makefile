# Makefile
#

CC = g++
CFLAGS = -c -Wall -Iinc -std=c++11 -g
LDFLAGS = 

SRCDIR = ./src
OBJDIR = ./obj
INCDIR = ./inc

VPATH = src

# List of sources
SOURCES = $(wildcard $(SRCDIR)/*.cc)
HEADERS = $(wildcard $(INCDIR)/*.h)
OBJECTS = $(patsubst $(SRCDIR)/%.cc,$(OBJDIR)/%.o,$(SOURCES))

# executable target
EXECUTABLE = game

LIBS = allegro-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 allegro_font-5 allegro_ttf-5 allegro_physfs-5
CFLAGS += $(shell pkg-config --cflags $(LIBS))
LDFLAGS += $(shell pkg-config --libs $(LIBS)) -lpugixml

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@$(CC) $^ -o $@ $(LDFLAGS)
	@echo "$(OBJECTS) -> $@"

$(OBJDIR)/%.o: %.cc $(HEADERS)
	@$(CC) $(CFLAGS) $< -c -o $@
	@echo "$@"

.PHONY: clean

clean:
	@rm -f $(OBJECTS) $(EXECUTABLE)
