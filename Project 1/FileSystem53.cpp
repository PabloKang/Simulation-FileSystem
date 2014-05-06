//============================================================================
// Name        : FileSystem53.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : First Project Lab
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "FileSystem53.h"
using namespace std;


// Filesystem format parameters:
int FILE_SIZE_FIELD = 1;		// Size of file size field in bytes. Maximum file size allowed in this file system is 192.
int FILE_BLOCKS_MAX = 3;		// The maximum amount of blocks a file can be separated into.
int FILE_DESCR_SIZE = FILE_SIZE_FIELD + FILE_BLOCKS_MAX;
int MAX_FILE_NO = 14;			// Maximum number of files which can be stored by this file system.
int MAX_BLOCK_NO = 64;			// Maximum number of blocks which can be supported by this file system.
int MAX_BLOCK_NO_DIV8 = MAX_BLOCK_NO / 8;
int MAX_FILE_NAME_LEN = 32;		// Maximum size of file name in byte.
int MAX_OPEN_FILE = 3;			// Maximum number of files to open at the same time.
int FILEIO_BUFFER_SIZE = 64;	// Size of file io bufer
int _EOF = -1;					// End-of-File

int B = 64;						// Size (bytes) of each block
int K = 7;						// Number of blocks for desc_table
int F = 4;						// Number of File Descriptors per block

// STRUCT: OpenFile - Information of a single, open file:
struct OpenFile {
	bool inUse;					// Warns if OF index is in use
	char* buffer;				// Read/Write buffer of file
	int currPos;				// Current block index of read/write position in file
	int fDescIndex;				// File Descriptor index

	OpenFile()
	{
		inUse = false;
		buffer = new char[B];
		currPos = 0;
		fDescIndex = -1;
	}

	void Open(int fdIndex)
	{
		inUse = true;
		fDescIndex = fdIndex;
	}

	void Close()
	{
		delete[] buffer;
		currPos = 0;
		fDescIndex = -1;
	}
}; // END OpenFile

OpenFile* oft = new OpenFile[MAX_OPEN_FILE];	// Open File Table - Stores info of up to 3 open files
map<string, int>  dirFileMap;							// Directory File Map - <File Name, File Descriptor Index>


/**********************************
* Constructor of this File syste	m.
*   1. Initialize IO system.
*   2. Format it if not done.
*/
FileSystem53::FileSystem53(int l, int b, string storage) {

	// Initialize the description table cache
	desc_table = new char*[K];
	for (int i = 0; i < K; ++i) 
	{
		// Initalize Bytemap
		if (i == 0) 
			desc_table[i] = new char[MAX_BLOCK_NO];
		// Initialize File Descriptors
		else 
			desc_table[i] = new char[FILE_DESCR_SIZE * F];
	}
		

	// Initialize the ldisk
	ldisk = new char*[MAX_BLOCK_NO];
	for (int i = 0; i < MAX_BLOCK_NO; ++i)
	{
		// Initalize Bytemap
		if (i == 0)
			ldisk[i] = new char[MAX_BLOCK_NO];
		// Initialize File Descriptors
		else if (i > 0 && i < K)
			ldisk[i] = new char[FILE_DESCR_SIZE * F];
		// Initialize Data Blocks
		else
			ldisk[i] = new char[B];
	}
	
	format();
}



/******************************
* read_block : Read data to pointer in main memory (p) from the hard disk location on ldisk at index 'i'.
*/
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
			p[j] = ldisk[i][j];
	}
	else throw "ERROR >> READ_BLOCK(): Exceeded Disk Boundary.\n";
}



/******************************
* write_block : Write data from pointer in main memory (p) to the hard disk location on ldisk at index 'i'.
*/
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
			ldisk[i][j] = p[j];
	}
	else throw "ERROR >> WRITE_BLOCK(): Exceeded Disk Boundary.\n";
}



// Open File Table(OFT).
void FileSystem53::OpenFileTable() {

}



// Allocate open file table
int FileSystem53::find_oft() {

	return -1;
}



//Deallocate
void FileSystem53::deallocate_oft(int index) {

}



/* Format file system.
 *   1. Initialize the first K blocks with zeros.
 *   2. Create root directory descriptor for directory file.
 * Parameter(s):
 *   none
 * Return:
 *   none
 */
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
			desc_table[i][j] = '0';
		}
		
		// Write to ldisk
		write_block(i, b);
		delete[] b;
	}

	// 2. Create root directory descriptor for directory file.
	char* d = new char[FILE_DESCR_SIZE * F];
	read_block(1, d);


}



/* Read descriptor
 * Parameter(s):
 *    no: Descriptor number to read
 * Return:
 *    Return char[4] of descriptor
 */
char* FileSystem53::read_descriptor(int no) {
	int i = no / 4;
	int j = no % 4;

	char* desc = new char[FILE_DESCR_SIZE];

	for (int k = 0; k < FILE_DESCR_SIZE; k++)
		desc[k] = desc_table[i][j + k];

	return desc;
}



/* Clear descriptor
 *   1. Clear descriptor entry
 *   2. Clear bitmap
 *   3. Write back to disk
 * Parameter(s):
 *    no: Descriptor number to clear
 * Return:
 *    none
 */
void FileSystem53::clear_descriptor(int no) {
	int i = no / 4;
	int j = no % 4;

	for (int k = 0; k < FILE_DESCR_SIZE; k++)
		desc_table[i][j + k] = '0';
}



/* Write descriptor
 *   1. Update descriptor entry
 *   2. Mark bitmap
 *   3. Write back to disk
 * Parameter(s):
 *    no: Descriptor number to write
 *    desc: descriptor to write
 * Return:
 *    none
 */
void FileSystem53::write_descriptor(int no, char* desc) {


}



/* Search for an unoccupied descriptor.
 * If ARRAY[0] is zero, this descriptor is not occupied.
 * Then it returns descriptor number.
 */
int FileSystem53::find_empty_descriptor() {
	int index = -1;

	for (int i = 1; i <= MAX_FILE_NO / 4; i++)
	{
		for (int j = 0; j < FILE_DESCR_SIZE * F; j++)
		{
			if (j % 4 == 0 && desc_table[i][j] == '0')
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



/* Search for an unoccupied block.
 *   This returns the first unoccupied block in bitmap field.
 *   Return value -1 means all blocks are occupied.
 * Parameter(s):
 *    none
 * Return:
 *    Returns the block number
 *    -1 if not found
 */
int FileSystem53::find_empty_block() {

	return -1;
}



/* Get one character.
 *    Returns the character currently pointed by the internal file position
 *    indicator of the specified stream. The internal file position indicator
 *    is then advanced to the next character.
 * Parameter(s):
 *    index (descriptor number of the file to be added.)
 * Return:
 *    On success, the character is returned.
 *    If a writing error occurs, EOF is returned.
 */
int FileSystem53::fgetc(int index) {

	return -1;
}




/* Put one character.
 *    Writes a character to the stream and advances the position indicator.
 *    The character is written at the position indicated by the internal position
 *    indicator of the file, which is then automatically advanced by one.
 * Parameter(s):
 *    c: character to write
 *    index (descriptor number of the file to be added.)
 * Return:
 *    On success, the character written is returned.
 *    If a writing error occurs, -2 is returned.
 */
int FileSystem53::fputc(int c, int index) {

	return -1;
}




/* Check for the end of file.
 * Parameter(s):
 *    index (descriptor number of the file to be added.)
 * Return:
 *    Return true if end-of-file reached.
 */
bool FileSystem53::feof(int index) {

	return false;
}


/* Search for a file
 * Parameter(s):
 *    index: index of open file table
 *    st: The name of file to search.
 * Return:
 *    index: An integer number position of found file entry.
 *    Return -1 if not found.
 */
int FileSystem53::search_dir(int index, string symbolic_file_name) {

	return -1;

}



/* Clear a file entry from directory file
 *
 * Parameter(s):
 *    index: open file table index
 *    start_pos:
 *    length:
 * Return:
 *    none
 */
void FileSystem53::delete_dir(int index, int start_pos, int len) {


}




/* File creation function:
 *    1. creates empty file with file size zero.
 *    2. makes/allocates descriptor.
 *    3. updates directory file.
 * Parameter(s):
 *    symbolic_file_name: The name of file to create.
 * Return:
 *    Return 0 for successful creation.
 *    Return -1 for error (no space in disk)
 *    Return -2 for error (for duplication)
 */
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



/* Open file with descriptor number function:
 * Parameter(s):
 *    desc_no: descriptor number
 * Return:
 *    index: index if open file table if successfully allocated.
 *    Return -1 for error.
 */
int FileSystem53::open_desc(int desc_no) {
	int oftIndex = -1;	// OFT entry index

	for (int i = 0; i < MAX_OPEN_FILE; i++) {
		if (oft[i].inUse == false) {
			oft[i].inUse = true;
			oft[i].fDescIndex = desc_no;
			oftIndex = i;
			break;
		}
	}
	return oftIndex;
}



/* Open file with file name function:
 * Parameter(s):
 *    symbolic_file_name: The name of file to open.
 * Return:
 *    index: An integer number, which is a index number of open file table.
 *    Return -1 or -2 if it cannot be open.
 */
// TODOs:
// 1. Open directory file
// 2. Search for a file with given name
//    Return -1 if not found.
// 3. Get descriptor number of the found file
// 4. Looking for unoccupied entry in open file table.
//    Return -2 if all entry are occupied.
// 5. Initialize the entry (descriptor number, current position, etc.)
// 6. Return entry number
int FileSystem53::open(string symbolic_file_name) {
	int desc_no = -1;	// Descriptor index
	int oftIndex = -2;	// OFT entry index

	try { desc_no = dirFileMap.at(symbolic_file_name); }
	catch (exception& e) { return -1; }
	
	for (int i = 0; i < MAX_OPEN_FILE; i++) {
		if (oft[i].inUse == false) {
			oft[i].inUse = true;
			oft[i].fDescIndex = desc_no;
			oftIndex = i;
			break;
		}
	}
	return oftIndex;
}




/* File Read function:
 *    This reads a number of bytes from the the file indicated by index.
 *    Reading should start from the point pointed by current position of the file.
 *    Current position should be updated accordingly after read.
 * Parameter(s):
 *    index: File index which indicates the file to be read.
 *    mem_area: buffer to be returned
 *    count: number of byte(s) to read
 * Return:
 *    Actual number of bytes returned in mem_area[].
 *    -1 value for error case "File hasn't been open"
 *    -2 value for error case "End-of-file"
 TODOs:
 1. Read the open file table using index.
 1.1 Get the file descriptor number and the current position.
 1.2 Can't get proper file descriptor, return -1.
 2. Read the file descriptor
 2.1 Get file size and block array.
 3. Read 'count' byte(s) from file and store in mem_area[].
 3.1 If current position crosses block boundary, call read_block() to read the next block.
 3.2 If current position==file size, stop reading and return.
 3.3 If this is called when current position==file size, return -2.
 3.4 If count > mem_area size, only size of mem_area should be read.
 3.5 Returns actual number of bytes read from file.
 3.6 Update current position so that next read() can be done from the first byte haven't-been-read.
 */
int FileSystem53::read(int index, char* mem_area, int count) {
	OpenFile * of = &oft[index];
	int desc_no = of->fDescIndex;

	int i = desc_no / FILE_DESCR_SIZE;
	int j = desc_no - (i * FILE_DESCR_SIZE);

	if (desc_table[i][j] > 0){
		int fSize = desc_table[i][j];
		if (fSize = of->currPos) { return -2; }
		
		int* blocks = new int[FILE_BLOCKS_MAX];
		// Find block list for file being read
		for (int b = 1; b <= FILE_BLOCKS_MAX; b++)
			blocks[b] = desc_table[i][j + b];
		
		int posMod = 0;
		// Read file into mem_area
		for (; posMod < count; posMod++) {
			of->currPos += posMod;

			if (of->currPos >= B) {
				int nextBlock = of->currPos / 64;
				read_block(nextBlock, of->buffer);
			}

			mem_area[posMod] = of->buffer[of->currPos];
		}
		return posMod;
	}
	return -1;
}




/* File Write function:
 *    This writes 'count' number of 'value'(s) to the file indicated by index.
 *    Writing should start from the point pointed by current position of the file.
 *    Current position should be updated accordingly.
 * Parameter(s):
 *    index: File index which indicates the file to be read.
 *    value: a character to be written.
 *    count: Number of repetition.
 * Return:
 *    >0 for successful write
 *    -1 value for error case "File hasn't been open"
 *    -2 for error case "Maximum file size reached" (not implemented.)
 */
int FileSystem53::write(int index, char value, int count) {
	OpenFile * of = &oft[index];
	if (of->inUse == false)
		return -1;

	return -2;
}




/* Setting new read/write position function:
 * Parameter(s):
 *    index: File index which indicates the file to be read.
 *    pos: New position in the file. If pos is bigger than file size, set pos to file size.
 * Return:
 *    0 for successful write
 *    -1 value for error case "File hasn't been open"
 */
int FileSystem53::lseek(int index, int pos) {

	return -1;
}




/* Close file function:
 * Parameter(s):
 *    index: The index of open file table
 * Return:
 *    none
 */
void FileSystem53::close(int index) {
	if (index < MAX_OPEN_FILE) {
		oft[index].Close();
	}
	else 
		throw "ERROR >> CLOSE(): Index Out of Bounds.\n";
}


/* Delete file function:
 *    Delete a file
 * Parameter(s):
 *    symbolic_file_name: a file name to be deleted.
 * Return:
 *    Return 0 with success
 *    Return -1 with error (ie. No such file).
 */
int FileSystem53::deleteFile(string fileName) {

	return -1;
}




/* Directory listing function:
 *    List the name and size of files in the directory. (We have only one directory in this project.)
 *    Example of format:
 *       abc 66 bytes, xyz 22 bytes
 * Parameter(s):
 *    None
 * Return:
 *    None
 */
void FileSystem53::directory() {
	for (map<string, int>::iterator iter = dirFileMap.begin(); iter != dirFileMap.end(); iter++)
	{
		int desc_no = iter->second;
		int i = desc_no / FILE_DESCR_SIZE;
		int j = desc_no - (i * FILE_DESCR_SIZE);
		cout << "File Name:" << iter->first << " File Length (bytes):" << desc_table[i][j] << endl;
	}
}




/*------------------------------------------------------------------
  Disk management functions.
  These functions are not really a part of file system.
  They are provided for convenience in this emulated file system.
  ------------------------------------------------------------------
  Restores the saved disk image in a file to the array.
  */
void FileSystem53::restore() {
	std::ifstream inFile("ldiskISO.txt");
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



// Saves the array to a file as a disk image.
void FileSystem53::save() {
	std::ofstream outFile("ldiskISO.txt");

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
			nextChar = ldisk[i][j];
			if (nextChar != NULL)
				outFile << nextChar;
		}
		outFile << '\n';
	}
}



// Disk dump, from block 'start' to 'start+size-1'.
void FileSystem53::diskdump(int start, int size) {


}

