# Code prefix
NOME_PROGRAMMA=*

# Compilers and flags
COMPILER=mpic++
#CFLAGS=-Wall --pedantic -DSHOW_RESULTS # Uncomment to make the program print its results

# Source files
SRCS=$(NOME_PROGRAMMA).cpp

OUTPUT_PROGRAMMA = sorting.out

ALLEGRO_FLAGS=-lallegro -lallegro_main -lallegro_primitives -lallegro_image

$(OUTPUT_PROGRAMMA).out: $(SRCS)
	$(COMPILER) $(SRCS) -o $(OUTPUT_PROGRAMMA) $(CFLAGS) $(ALLEGRO_FLAGS)
