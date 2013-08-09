// compress.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "zlib.h"

int main(int argc, char* argv[])
{
	if(argc != 3)
		return -1;

	char* file = argv[1];
	char* output = argv[2];
	
	FILE* fr = fopen(file, "rb");
	if(fr == NULL)	return -1;

	long size = fseek(fr, 0, SEEK_END);
	size = ftell(fr); 
	fseek(fr, 0, SEEK_SET);
	printf("Original file size %d\n", size);

	char* buf = new char[size];
	if(buf == NULL)	return -1;

	int nread = fread(buf, 1, size, fr);
	if(nread != size)	return -1;
	
	int zlen = compressBound(size);
	char* zBuf = new char[zlen];
	if(zBuf == NULL)	return -1;

	int once = 1234;
	int offset = 0;
	int totalLen = 0;
	int totalZ = 0;
	int offsetZ = 0;

	//size = once;
	while(offset < size)
	{
		if(offset + once > size)
			once = size - offset;

		zlen = compressBound(once);
		int outLen = compress((Bytef *)zBuf+offsetZ, (uLongf *)&zlen, (Bytef *)buf+offset, once);
		printf("zcompress %d --> %d\n", once, zlen);

		offsetZ += zlen;
		offset += once;
	}

	printf("compress %d ---> %d: %g\n", size, offsetZ, 1.0*offsetZ/size);
	
	return 0;
}

