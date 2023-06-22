# Project name
NAME=       uniforms

# Include directory
INC_DIR=    ../../include/

# Compiler
CXX=	g++

# Source files
SRC_DIR=    # in case your cpp files are in a folder like src/

SRC_FILES=  uniforms.cpp      \
	    ../glad.c \

# Obj files
OBJ=	$($(addprefix $(SRC_DIR), $(SRC_FILES)):.c=.o)
OBJ= uniforms.o ../glad.o
# that rule is composed of two steps
#  addprefix, which add the content of SRC_DIR in front of every
#  word of SRC_FILES
#  And a second rule which change every ".c" extension into ".o"

LIBS=       glfw \
	    GL  \
	    X11 \
	    pthread    \
	    Xrandr  \
	    Xi \
	    dl

# Compilation flags
CXXFLAGS=   -Wall

CXXFLAGS+=  $(addprefix -I, $(INC_DIR))

LDFLAGS=    $(addprefix -L, $(LIB_DIR)) \
	    $(addprefix -l, $(LIBS))

# Rules

# this rule is only linking, no CFLAGS required
$(NAME):    $(OBJ) # this force the Makefile to create the .o files
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS)


All:    $(NAME)

# Remove all obj files
clean:
	rm -f $(OBJ)

# Remove all obj files and the binary
fclean: clean
	rm -f $(NAME)

# Remove all and recompile
re: fclean all

# Rule to compile every .c file into .o
%.o:    %.c
	$(CXX) -o $@ -c $< $(CFLAGS)

# Describe all the rules who do not directly create a file
.PHONY: All clean fclean re
