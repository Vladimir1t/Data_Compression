CC = g++
FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code\
	-Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default\
	-Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual\
	-Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2\
	-Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers\
	-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo\
	-Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits\
	-Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

library      = text_files\library.txt

#    write a name of input file
sourse_file  = text_files\text_5.txt
encoded_file = text_files\file_encoded.txt
decoded_file = text_files\file_decoded.txt

compress: data_compression
	data_compression.exe $(library) $(sourse_file) $(encoded_file)

data_compression.o: src\data_compression.cpp
	$(CC) -c src\data_compression.cpp

data_compression: data_compression.o
	$(CC) -o data_compression data_compression.o


decoder: data_decoder
	data_decoder.exe $(library) $(encoded_file) $(decoded_file)

decoder.o: src\decoder.cpp
	$(CC) -c src\decoder.cpp

data_decoder: decoder.o
	$(CC) -o data_decoder decoder.o
