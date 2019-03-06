/*
 * G2S
 * Copyright (C) 2018, Mathieu Gravey (gravey.mathieu@gmail.com) and UNIL (University of Lausanne)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "jsonData.hpp"
#include "zlib.h"
#include "picosha2.h"
#include <unistd.h>

void createLink(char* outputFullFilename, char* fullFilename){
	symlink(outputFullFilename, fullFilename);
}


char* loadRawData(const char * hash){
	char* data=nullptr;
	char* ptr=nullptr;
	char filename[4096];

	//fprintf(stderr, "look For File %s \n",hash);

	sprintf(filename,"./data/%s.bgrid.gz",hash);
	if(!data &&   g2s::file_exist(filename)){
		gzFile dataFile=gzopen(filename,"rb");
		if(dataFile) {
			size_t fullSize;
			gzread (dataFile, &fullSize, sizeof(fullSize));
			gzrewind (dataFile);
			ptr =(char*) malloc (sizeof(char)*fullSize);
			data=ptr;
			gzread (dataFile, data, fullSize);
			gzclose(dataFile);
		}
	}
	sprintf(filename,"./data/%s.bgrid",hash);
	//fprintf(stderr, "%s\n",filename );
	if(!data &&  g2s::file_exist(filename)){
		FILE* dataFile=fopen(filename,"rb");
		if(dataFile) {
			size_t fullSize;
			fread (&fullSize, 1, sizeof(fullSize), dataFile);
			rewind (dataFile);
			ptr = (char*)malloc (sizeof(char)*fullSize);
			data=ptr;
			fread (data,1,fullSize,dataFile);
			fclose(dataFile);
		}
	}

	if(!data){
		fprintf(stderr, "file not found\n");
	}

	return data;
}

char* writeRawData(char* data, bool compresed){
	size_t fullSize=*((size_t*)data);
	std::vector<unsigned char> hash(32);
	picosha2::hash256((unsigned char*)data, ((unsigned char*)data)+fullSize-1, hash.begin(), hash.end());

	char* hashInHexa=(char*)malloc(65);
	memset(hashInHexa,0,65);
	for (int i = 0; i < 32; ++i)
	{
		sprintf(hashInHexa+2*i,"%02x",hash.data()[i]);
	}
	//fprintf(stderr, "%s\n", hashInHexa);

	char filename[4096];

	if(compresed) {
		sprintf(filename,"./data/%s.bgrid.gz",hashInHexa);

		gzFile dataFile=gzopen(filename,"wb");
		if(dataFile) {
			gzwrite (dataFile, data, fullSize);
			gzclose(dataFile);
		}
	}
	else {
			sprintf(filename,"./data/%s.bgrid",hashInHexa);

		FILE* dataFile=fopen(filename,"wb");
		if(dataFile) {
			fwrite (data, 1, fullSize, dataFile);
			fclose(dataFile);
		}
	}
	return hashInHexa;
}