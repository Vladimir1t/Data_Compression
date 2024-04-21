CC = g++
FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code\
	-Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default\
	-Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual\
	-Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2\
	-Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers\
	-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo\
	-Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits\
	-Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

all: data_compression
	data_compression.exe text_files\library.txt text_files\file_sourse.txt text_files\file_result.txt

data_compression.o: src\data_compression.cpp
	$(CC) -c src\data_compression.cpp $(FLAGS)

data_compression: data_compression.o
	$(CC) -o data_compression data_compression.o

decoder: data_decoder
	data_decoder.exe text_files\library.txt text_files\file_result.txt text_files\file_decoded.txt

decoder.o: src\decoder.cpp
	$(CC) -c src\decoder.cpp

data_decoder: decoder.o
	$(CC) -o data_decoder decoder.o
