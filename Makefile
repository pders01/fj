# Name of your executable 
EXECUTABLE = fj 

# C compiler and flags
CC = gcc
CFLAGS = -Wall 

# Source files (adjust if you have multiple source files)
SRCS = $(wildcard *.c)

# Object files
OBJS = $(SRCS:.c=.o)

.PHONY: all clean install 

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ 

# Object file dependencies 
%.o : %.c uthash.h 
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJS)

SHELL := $(shell basename $$SHELL)

install: $(EXECUTABLE)
	@echo "Installing $(EXECUTABLE) to /usr/local/bin/..."
	@sudo cp $(EXECUTABLE) /usr/local/bin/
	@echo "Installation successful."
	@echo ""
	@echo "Appending fj function to your shell configuration..."

ifeq ($(SHELL), bash)
	@echo "" >> ~/.bashrc
	@cat fj_function.sh >> ~/.bashrc
	@echo "fj function appended to ~/.bashrc."
	@echo "Please restart your shell or run 'source ~/.bashrc' to apply changes."
else ifeq ($(SHELL), zsh)
	@echo "" >> ~/.zshenv
	@cat fj_function.sh >> ~/.zshenv
	@cat fj_function.sh
	@echo "fj function appended to ~/.zshenv."
	@echo "Please restart your shell or run 'source ~/.zshenv' to apply changes."
else
	@echo "Unsupported shell: $(SHELL). Please manually append the fj function to your shell configuration."
endif

