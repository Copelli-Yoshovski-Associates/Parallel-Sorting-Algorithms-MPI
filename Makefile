
COMPILER=mpic++
CFLAGS=-O3 -Ofast
#CFLAGS+=-Wall --pedantic -DSHOW_RESULTS

SRCS=*.cpp
OUTPUT_PROGRAMMA = sorting.out
ALLEGRO_FLAGS=-lallegro -lallegro_main -lallegro_primitives -lallegro_image

$(OUTPUT_PROGRAMMA).out: $(SRCS)
	$(COMPILER) $(SRCS) -o $(OUTPUT_PROGRAMMA) $(CFLAGS) $(ALLEGRO_FLAGS)
