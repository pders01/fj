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

install: $(EXECUTABLE)
	@echo "Installation instructions:"
	@echo "1. Copy '$(EXECUTABLE)' to a directory in your PATH (e.g., /usr/local/bin)"
	@echo "2. Add the following shell function to your shell configuration:"
	@echo "   function fj() {"
	@echo "       if [ $$# -eq 1 ]; then"
	@echo "           new_dir=$$(./fj $$1)"
	@echo "           if [ $$? -eq 0 ]; then"
	@echo "               cd \"$$new_dir\""
	@echo "           fi"
	@echo "       else"
	@echo "           echo \"Usage: fj <ALIAS>\""
	@echo "       fi"
	@echo "   }"

_fj_complete() {
   local cur prev expanded_path

   cur="${words[CURRENT]}"  
   prev="${words[CURRENT-1]}"

   if [[ $prev == 'fj' && $CURRENT -eq 2 ]]; then
      expanded_path=$(./fj -complete "$cur") 
      if [[ -d $expanded_path ]]; then # Safety check
         # Trigger standard Zsh directory completion
         COMPREPLY=(${(f)"$(cd $expanded_path; pwd)"})
      fi 
   fi
}

compctl -K _fj_complete fj

function fj() {
  if [ -d $1 ]; then
    cd "$new_dir"
  elif [ $# -eq 1 ]; then
    new_dir=$(./fj $1)
    cd "$new_dir"   
  else
    echo ./fj $1
  fi
}
