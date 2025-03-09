CC = g++
FLAGS = -Wall -g -O2\

library      = text_files/library.txt

#---- write a name of input file ----
sourse_file  = text_files/text_5.txt
encoded_file = text_files/file_encoded.txt
decoded_file = text_files/file_decoded.txt

compress: data_compression
	data_compression.exe $(library) $(sourse_file) $(encoded_file)

data_compression.o: src/data_compression.cpp
	$(CC) -c src/data_compression.cpp

data_compression: data_compression.o
	$(CC) -o data_compression data_compression.o

decoder: data_decoder
	data_decoder.exe $(library) $(encoded_file) $(decoded_file)

decoder.o: src/decoder.cpp
	$(CC) -c src/decoder.cpp

data_decoder: decoder.o
	$(CC) -o data_decoder decoder.o
