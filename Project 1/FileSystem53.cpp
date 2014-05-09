//============================================================================
// Name        : FileSystem53.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : First Project Lab
//============================================================================

#include "FileSystem53.h"
using namespace std;


FileSystem53::FileSystem53(int l, int b, string storage) {

	// Initialize the description table cache
	descTable = new char*[K];
	for (int i = 0; i < K; ++i) 
	{
		// Initalize Bytemap
		if (i == 0) 
			descTable[i] = new char[MAX_BLOCK_NO];
		// Initialize File Descriptors
		else 
			descTable[i] = new char[FILE_DESCR_SIZE * F];
	}
		
	// Initialize the lDisk
	lDisk = new char*[MAX_BLOCK_NO];
	for (int i = 0; i < MAX_BLOCK_NO; ++i)
	{
		// Initalize Bytemap
		if (i == 0)
			lDisk[i] = new char[MAX_BLOCK_NO];
		// Initialize File Descriptors
		else if (i > 0 && i < K)
			lDisk[i] = new char[FILE_DESCR_SIZE * F];
		// Initialize Data Blocks
		else
			lDisk[i] = new char[B];
	}
	
	format();
}


void FileSystem53::read_block(int i, char *p) {
	if (i < MAX_BLOCK_NO)
	{
		int arrayLimit = 0;		// Array size for block to be read.
		
		// Set arrayLimit to match for the block area (bytemap, FD's, data blocks) being read.
		if (i == 0)
			arrayLimit = MAX_BLOCK_NO;
		else if (i > 0 && i < K)
			arrayLimit = FILE_DESCR_SIZE * F;
		else if (i >= K && i < MAX_BLOCK_NO)
			arrayLimit = B;
		
		// Copy all entries in the block at position 'i' to the memory array 'p'
		for (int j = 0; j < arrayLimit; j++)
			p[j] = lDisk[i][j];
	}
	else throw "ERROR >> READ_BLOCK(): Exceeded Disk Boundary.\n";
}


void FileSystem53::write_block(int i, char *p) {
	if (i < MAX_BLOCK_NO)
	{
		int arrayLimit = 0;		// Array size for block to be written to.

		// Set arrayLimit to match for the block area (bytemap, FD's, data blocks) being written.
		if (i == 0)
			arrayLimit = MAX_BLOCK_NO;
		else if (i > 0 && i < K)
			arrayLimit = FILE_DESCR_SIZE * F;
		else if (i >= K && i < MAX_BLOCK_NO)
			arrayLimit = B;

		// Copy all entries in the memory array 'p' to the block at position 'i' to
		for (int j = 0; j < arrayLimit; j++)
			lDisk[i][j] = p[j];
	}
	else throw "ERROR >> WRITE_BLOCK(): Exceeded Disk Boundary.\n";
}


void FileSystem53::OpenFileTable() {

}


int FileSystem53::find_oft() {

	return -1;
}


void FileSystem53::deallocate_oft(int index) {

}


void FileSystem53::format() {
	int arraySize = 0;	// Size of b block.

	// 1. Initialize the first K blocks with zeros.
	for (int i = 0; i < K; i++) {
		// Determine size of temporary block
		if (i == 0)
			arraySize = MAX_BLOCK_NO;
		else if (i > 0 && i < K)
			arraySize = FILE_DESCR_SIZE * F;

		char* b = new char[arraySize];

		// Fill temporary block
		for (int j = 0; j < arraySize; j++) {
			b[j] = '0';
			descTable[i][j] = '0';
		}
		
		// Write to lDisk
		write_block(i, b);
		delete[] b;
	}

	// 2. Create root directory descriptor for directory file.
	char* d = new char[FILE_DESCR_SIZE * F];
	read_block(1, d);


}


char* FileSystem53::read_descriptor(int no) {
	int i = no / 4;
	int j = no % 4;

	char* desc = new char[FILE_DESCR_SIZE];

	for (int k = 0; k < FILE_DESCR_SIZE; k++)
		desc[k] = descTable[i][j + k];

	return desc;
}


void FileSystem53::clear_descriptor(int no) {
	int i = no / 4;
	int j = no % 4;

	for (int k = 0; k < FILE_DESCR_SIZE; k++)
		descTable[i][j + k] = '0';
}


void FileSystem53::write_descriptor(int no, char* desc) {


}


int FileSystem53::find_empty_descriptor() {
	int index = -1;

	for (int i = 1; i <= MAX_FILE_NO / 4; i++)
	{
		for (int j = 0; j < FILE_DESCR_SIZE * F; j++)
		{
			if (j % 4 == 0 && descTable[i][j] == '0')
			{
				index = (i * 4) + (j / 4);
				
				if (i >= 3 && j >= 2 )
					index = -1;

				break;
			}
		}
	}
	return index; 
}


int FileSystem53::find_empty_block() {

	return -1;
}


int FileSystem53::fgetc(int index) {

	return -1;
}


int FileSystem53::fputc(int c, int index) {

	return -1;
}


bool FileSystem53::feof(int index) {

	return false;
}


int FileSystem53::search_dir(int index, string symbolic_file_name) {

	return -1;

}


void FileSystem53::delete_dir(int index, int start_pos, int len) {


}


int FileSystem53::create(string symbolic_file_name) {
	int desc_no = find_empty_descriptor();
	
	if (desc_no != -1) {
		try { dirFileMap.at(symbolic_file_name); }
		catch (exception& e) { return -2; }

		dirFileMap.insert(std::pair<string, int>(symbolic_file_name, desc_no));
		return 0;
	}
	else return -1; // error: no space in disk
}


int FileSystem53::open_desc(int desc_no) {
	int oftIndex = -1;	// OFT entry index

	for (int i = 0; i < MAX_OPEN_FILE; i++) {
		if (oft[i].inUse == false) {
			oft[i].inUse = true;
			oft[i].desc = desc_no;
			oftIndex = i;
			break;
		}
	}
	return oftIndex;
}


int FileSystem53::open(string symbolic_file_name) {
	int desc_no = -1;	// Descriptor index
	int oftIndex = -2;	// OFT entry index

	try { desc_no = dirFileMap.at(symbolic_file_name); }
	catch (exception& e) { return -1; }
	
	for (int i = 0; i < MAX_OPEN_FILE; i++) {
		if (oft[i].inUse == false) {
			oft[i].inUse = true;
			oft[i].desc = desc_no;
			oftIndex = i;
			break;
		}
	}
	return oftIndex;
}


int FileSystem53::read(int index, char* mem_area, int count) {
	OpenFile * of = &oft[index];
	int desc_no = of->desc;

	int i = desc_no / FILE_DESCR_SIZE;
	int j = desc_no - (i * FILE_DESCR_SIZE);

	if (descTable[i][j] > 0){
		int fSize = descTable[i][j];
		if (fSize = of->pos) { return -2; }
		
		int* blocks = new int[FILE_BLOCKS_MAX];
		// Find block list for file being read
		for (int b = 1; b <= FILE_BLOCKS_MAX; b++)
			blocks[b] = descTable[i][j + b];
		
		int posMod = 0;
		// Read file into mem_area
		for (; posMod < count; posMod++) {
			of->pos += posMod;

			if (of->pos >= B) {
				int nextBlock = of->pos / 64;
				read_block(nextBlock, of->buffer);
			}

			mem_area[posMod] = of->buffer[of->pos];
		}
		return posMod;
	}
	return -1;
}


int FileSystem53::write(int index, char value, int count) {
	OpenFile * of = &oft[index];
	if (of->inUse == false)
		return -1;

	return -2;
}


int FileSystem53::lseek(int index, int pos) {

	return -1;
}


void FileSystem53::close(int index) {
	if (index < MAX_OPEN_FILE) {
		deallocate_oft(index);
	}
	else 
		throw "ERROR >> CLOSE(): Index Out of Bounds.\n";
}


int FileSystem53::deleteFile(string fileName) {

	return -1;
}


void FileSystem53::directory() {
	for (map<string, int>::iterator iter = dirFileMap.begin(); iter != dirFileMap.end(); iter++)
	{
		int desc_no = iter->second;
		int i = desc_no / FILE_DESCR_SIZE;
		int j = desc_no - (i * FILE_DESCR_SIZE);
		cout << "File Name:" << iter->first << " File Length (bytes):" << descTable[i][j] << endl;
	}
}


void FileSystem53::restore() {
	std::ifstream inFile("lDiskISO.txt");
	char nextChar = ' ';

	// For all blocks...
	for (int i = 0; i < MAX_BLOCK_NO; i++)
	{	
		int arraySize = 0;		// Array size for block to be read.
			
		// Set arraySize to match for the block area (bytemap, FD's, data blocks).
		if (i == 0)
			arraySize = MAX_BLOCK_NO;
		else if (i > 0 && i < K)
			arraySize = FILE_DESCR_SIZE * F;
		else if (i >= K)
			arraySize = B;

		// Initialize a temporary array, 'p', to store the block read from file.
		char* p = new char[arraySize];

		// For the size of the current block...
		for (int j = 0; j < arraySize; j++)
		{
			nextChar = inFile.get();

			// If there is a character left in the file line, copy it to 'p'.
			if (nextChar != '\n' && nextChar != EOF)
				p[j] = nextChar;
			else if (nextChar == '\n')
				break;
			else if (nextChar == EOF)
				throw "ERROR >> RESTORE(): File data stream was interrupted.\n";
		}
		write_block(i, p);
		delete[] p;
	}
}


void FileSystem53::save() {
	std::ofstream outFile("lDiskISO.txt");

	char nextChar;	// Character to write to file.

	// For all blocks...
	for (int i = 0; i < MAX_BLOCK_NO; i++)
	{
		int arraySize = 0;		// Array size for block to be read. 
		
		// Set arraySize to match for the block area (bytemap, FD's, data blocks).
		if (i == 0)
			arraySize = MAX_BLOCK_NO;
		else if (i > 0 && i < K)
			arraySize = FILE_DESCR_SIZE * F;
		else if (i >= K)
			arraySize = B;

		// For every character in the block...
		for (int j = 0; j < arraySize; j++)
		{
			// Copy the character to nextChar, then write it to the file
			nextChar = lDisk[i][j];
			if (nextChar != NULL)
				outFile << nextChar;
		}
		outFile << '\n';
	}
}


void FileSystem53::diskdump(int start, int size) {


}

