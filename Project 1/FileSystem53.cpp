//============================================================================
// Name        : FileSystem53.cpp
// Author      : Pablo Kang
// Version     :
// Copyright   : Your copyright notice
// Description : First Project Lab
//============================================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>


#define FLAG_ERROR	-1
#define FLAG_EMPTY	-1
#define FLAG_SUCCESS 0


using namespace std;

// FILESYSTEM53 FORMATTING PARAMETERS:
int MAX_BLOCK_NO = 64;					// Maximum number of blocks which can be supported by this file system.
int MAX_BLOCK_NO_DIV8 = MAX_BLOCK_NO / 8;		// Maximum nmber of blocks in the file system, divided by 8.
int FILE_SIZE_FIELD = 1;					// Size of file size field in bytes. Maximum file size allowed in this file system is 192.
int FILE_BLOCKS_MAX = 3;					// The maximum amount of blocks a file can be separated into.
int FILE_DESCR_SIZE = FILE_SIZE_FIELD + FILE_BLOCKS_MAX;
int MAX_FILE_NO = 14;					// Maximum number of files which can be stored by this file system.
int MAX_FILE_NAME_LEN = 10;					// Maximum size of file name in byte.
int MAX_OPEN_FILES = 3;					// Maximum number of files to open at the same time.
int FILEIO_BUFFER_SIZE = 64;					// Size of file IO buffer.
int B = 64;					// Size (in bytes) of each block.
int K = 7;					// Number of blocks for desc_table
int F = 4;					// Number of File Descriptors per block.

// Stores data about a single file descriptor.
//	*	bool	inUse		- Warns if index is in use
//	*	int		size		- Size of file
//	*	int*	blks		- Blocks used by file
struct FileDescriptor {
	bool	inUse;			// Warns if OF index is in use.
	int		size;			// File size.
	int*	blks;			// Blocks used by file.

	FileDescriptor()
	{
		inUse = false;
		size = 0;
		blks = new int[FILE_BLOCKS_MAX];
	}
	void Open(int initSize, int initBlk)
	{
		inUse = true;
		size = initSize;
		blks[0] = initBlk;
	}
	void Close()
	{
		inUse = false;
		size = FLAG_ERROR;
	}
};


// Stores data about a single, open file.
//	*	bool	inUse		- Warns if OF index is in use
//	*	char*	buffer		- Read/Write buffer of file
//	*	int		pos			- Current block index of read/write position in file
//	*	int		desc		- File Descriptor index
struct OpenFile {
	bool	inUse;			// Warns if OF index is in use
	char*	buffer;			// Read/Write buffer of file
	int		desc;			// File Descriptor index
	int		cpos;			// Current index of read/write position in file
	int		cblk;			// Current block of the read/write positio in file
	int		size;			// Size of file

	OpenFile()
	{
		inUse = false;
		desc = FLAG_ERROR;
		cpos = FLAG_ERROR;
		cblk = FLAG_ERROR;
		size = FLAG_ERROR;
		buffer = new char[B];
	}
	void Open(int index, int blk, int fSize)
	{
		inUse = true;
		desc = index;
		cpos = 0;
		cblk = blk;
		size = fSize;

	}
	void Close()
	{
		inUse = false;
		desc = FLAG_ERROR;
		cpos = FLAG_ERROR;
		cblk = FLAG_ERROR;
		size = FLAG_ERROR;
	}
}; // END STRUCT

struct Directory{
	bool	inUse;			// Warns if index is in use
	string	fileName;		// Symbolic name of file
	int		desc;			// File Descriptor index

	Directory()
	{
		inUse = false;
	}
	void Open(string name, int index)
	{
		inUse = true;
		fileName = name;
		desc = index;
	}
	void Close()
	{
		inUse = false;
		fileName = "";
		desc = FLAG_ERROR;
	}
};



/**********************************************************************************************************
- CLASS: FileSystem53
-	A virtual simulation of a hard disk memory storage and controllers.
-	The hard disk is represented by the 2D char array, lDisk.
***********************************************************************************************************/
class FileSystem53 {
	//	This is a 2D Array simulation of a file system (hard disk).
	//	ldisk format:
	//	ldisk { byteMap[], fDesc_0[], fDesc_1[], ... , fDesc_N[], fData_0[], fData_1[], ... , fData_L[] }
	//	*	byteMap		- Each bit represent a block in a disk. MAX_BLOCK_NO/8 bytes
	//	*	fDesc_0		- Root File Directory's descriptor.
	//	*	fDesc_N		- N'th descriptor. Each descriptor is FILE_SIZE_FIELD + FILE_BLOCKS_MAX bytes long.
	//	*	fData_0		- First data storage block.
	//	*	fData_L		- Last data storage block, where L is MAX_BLOCK_NO - 1.
	char** lDisk;

	char*				byteMap;			// Cache of byte map.
	FileDescriptor**	descTable;			// Cache of file descriptors.
	OpenFile*			oft;				// Open File Table - Stores info of up to 3 open files.
	Directory*			dirFile;			// Directory File Map - <File Name, File Descriptor Index>.


public:
	string				restoreFile = "";	// File name of disk image text file.


	FileSystem53::FileSystem53() {};
	FileSystem53::FileSystem53(string resFile) {
		// Initialize byte map cache
		byteMap = new char[MAX_BLOCK_NO];

		// Initialize the file descriptor cache
		descTable = new FileDescriptor*[K - 1];
		for (int i = 0; i < K - 1; ++i)
			descTable[i] = new FileDescriptor[F];

		// Initialize the directory file cache
		dirFile = new Directory[MAX_FILE_NO];

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
		restoreFile = resFile;
		OpenFileTable();
		format();
	}


	void FileSystem53::format() {
		// initialize byte map
		for (int i = 0; i < MAX_BLOCK_NO; ++i) {
			if (i < K + FILE_BLOCKS_MAX)
				byteMap[i] = (char)1;
			else
				byteMap[i] = (char)0;

		}
		write_block(0, byteMap);

		// create directory file
		int dirSize = MAX_FILE_NO * (MAX_FILE_NAME_LEN + 1);
		char* directory = new char[B];

		char dir[11] = "directory ";
		dir[10] = (char)dirSize;

		for (int i = 0; i < 11; i++){
			directory[i] = dir[i];
		}
		write_block(K, directory);

		descTable[0][0].Open(dirSize, K);
		descTable[0][0].size = dirSize;
		for (int i = 0; i < FILE_BLOCKS_MAX; i++){
			descTable[0][0].blks[i] = i;
		}
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
		else throw "FS53> ERROR >> READ_BLOCK(): Exceeded Disk Boundary.\n";
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
		else throw "FS53> ERROR >> WRITE_BLOCK(): Exceeded Disk Boundary.\n";
	}


	void FileSystem53::OpenFileTable() {
		oft = new OpenFile[MAX_OPEN_FILES];
	}


	int FileSystem53::find_oft() {
		for (int i = 0; i < MAX_OPEN_FILES; i++) {
			if (oft[i].inUse == false)
				return i;
		}
		return FLAG_ERROR;
	}


	void FileSystem53::deallocate_oft(int index) {
		for (int i = 0; i < MAX_OPEN_FILES; i++)
			oft[i].Close();
		delete[] oft;
	}


	FileDescriptor FileSystem53::read_descriptor(int no) {
		return descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE];
	}


	void FileSystem53::clear_descriptor(int no) {
		descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE].Close();
	}


	void FileSystem53::write_descriptor(int no, FileDescriptor desc) {
		descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE].inUse = desc.inUse;
		descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE].size = desc.size;
		for (int i = 0; i < FILE_BLOCKS_MAX; i++)
				descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE].blks[i] = desc.blks[i];;
	}


	void FileSystem53::update_desc_size(int no, int fSize)
	{
		descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE].size = fSize;
	}


	void FileSystem53::update_desc_block(int no, int blockNum, int value)
	{
		descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE].blks[blockNum] = value;
	}


	int FileSystem53::get_desc_block_value(int no, int blockNum)
	{
		return descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE].blks[blockNum];
	}


	int FileSystem53::get_desc_size(int no)
	{
		return descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE].size;
	}


	int FileSystem53::find_empty_descriptor() {
		for (int no = 0; no <= MAX_FILE_NO; no++) {
			if (!descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE].inUse)
				return no;
		}
		return FLAG_ERROR;
	}


	int FileSystem53::find_empty_block() {
		for (int i = 0; i < MAX_BLOCK_NO; i++) {
			if (byteMap[i] == (char)0)
				return i;
		}
		return FLAG_ERROR;
	}


	int FileSystem53::fgetc(int index) {
		int curr = oft[index].cpos - B;
		if (curr < B) {
			oft[index].cpos++;
			return oft[index].buffer[curr];
		}
		return EOF;
	}


	int FileSystem53::fputc(int c, int index) {
		int curr = oft[index].cpos - B;

		if (curr < B) {
			oft[index].buffer[curr] = c;
			oft[index].cpos++;
			oft[index].size += 1;
			update_desc_size(oft[index].desc, oft[index].size);
			return oft[index].buffer[curr];
		}
		cout << "FS53> Error >> fputc(): " << EOF << "EOF" << endl;
		return EOF;
	}


	bool FileSystem53::feof(int index) {

		return false;
	}


	int FileSystem53::search_dir(int index, string symbolic_file_name) {
		for (int i = 0; i < MAX_FILE_NO; i++) {
			if (dirFile[i].fileName == symbolic_file_name)
				return dirFile[i].desc;
		}
		return FLAG_ERROR;
	}


	void FileSystem53::delete_dir(int index, int start_pos, int len) {

	}


	int FileSystem53::create(string symbolic_file_name) {
		int desc_no = find_empty_descriptor();
		int blck_no = find_empty_block();

		if (desc_no != FLAG_ERROR && blck_no != FLAG_ERROR) {
			if (search_dir(0, symbolic_file_name) != FLAG_ERROR)
				return -2;

			for (int i = 0; i < MAX_FILE_NO; i++) {
				if (!dirFile[i].inUse) {
					FileDescriptor desc;
					desc.Open(0, blck_no);
					write_descriptor(desc_no, desc);
					dirFile[i].fileName = symbolic_file_name;
					dirFile[i].desc = desc_no;
					return 0;
				}
			}
		}
		return FLAG_ERROR; // error: no space in disk
	}


	int FileSystem53::open_desc(int desc_no) {
		for (int i = 0; i < MAX_OPEN_FILES; i++) {
			if (oft[i].inUse == false) {
				oft[i].Open(desc_no, 0, get_desc_size(desc_no));
				read_block(get_desc_block_value(desc_no, oft[i].cblk), oft[i].buffer);
				return i;
			}
		}
		return FLAG_ERROR;
	}


	int FileSystem53::open(string symbolic_file_name) {
		int desc_no = -1;	// Descriptor index
		int oftIndex = -2;	// OFT entry index

		desc_no = search_dir(0, symbolic_file_name);
		if (desc_no == FLAG_ERROR)
			return -2;

		for (int i = 0; i < MAX_OPEN_FILES; i++) {
			if (oft[i].inUse == false) {
				oft[i].Open(desc_no, 0, get_desc_size(desc_no));
				read_block(get_desc_block_value(desc_no, oft[i].cblk), oft[i].buffer);
				return i;
			}
		}
		return oftIndex;
	}


	int FileSystem53::read(int index, char* mem_area, int count) {
		if (!oft[index].inUse)
			return FLAG_ERROR;

		int bytes_read = 0;
		for (int i = 0; i < count; ++i) {
			if (oft[index].cpos >= B * oft[index].cblk++)
				read_block(get_desc_block_value(oft[index].desc, oft[index].cblk), oft[index].buffer);
			mem_area[i] = fgetc(index);
			bytes_read++;
		}
		return bytes_read;
	}


	int FileSystem53::write(int index, char value, int count) {
		if (!oft[index].inUse)
			return FLAG_ERROR;

		if (oft[index].cpos >= B * oft[index].cblk) {
			if (oft[index].cblk > MAX_OPEN_FILES) return FLAG_ERROR;

			write_block(get_desc_block_value(oft[index].desc, oft[index].cblk++), oft[index].buffer);

			int nextBlock = get_desc_block_value(oft[index].desc, oft[index].cblk);
			if (nextBlock != NULL) {
				nextBlock = find_empty_block();
				update_desc_block(oft[index].desc, oft[index].cblk, nextBlock);
				byteMap[nextBlock] = (char)1; // Update bitmap
			}
		}

		for (int i = 0; i < count; ++i) {
			fputc(value, index);
		}
		return 0;
	}


	int FileSystem53::lseek(int index, int pos) {
		if (!oft[index].inUse)
			return FLAG_ERROR;

		if (pos > oft[index].size) {
			pos = oft[index].size;
		}

		oft[index].cpos = pos;
		return pos;
	}


	void FileSystem53::close(int index) {
		if (index < MAX_OPEN_FILES) {
			// set the cursor back to start position
			lseek(index, 0);
			// create write buffer and start writing to IO layer
			write_block(get_desc_block_value(oft[index].desc, oft[index].cblk), oft[index].buffer);
			oft[index].Close();
		}
		else
			throw "FS53> ERROR >> CLOSE(): Index Out of Bounds.\n";
	}


	int FileSystem53::deleteFile(string fileName) {
		int fileDesc;

		fileDesc = search_dir(0, fileName);

		if (fileDesc != FLAG_ERROR) {
			// clear descriptor
			clear_descriptor(fileDesc);

			// update bitmap
			for (int i = 0; i < FILE_BLOCKS_MAX; ++i) {
				int bitmapIndex = get_desc_block_value(fileDesc, i);
				if (bitmapIndex > K) {
					byteMap[bitmapIndex] = (char)0;
				}
			}

			// delete in oft
			for (int i = 1; i < MAX_OPEN_FILES + 1; i++)
			{
				if (oft[i].inUse && oft[i].desc == fileDesc)
				{
					close(i);
				}
			}
			return FLAG_SUCCESS;
		}
		return FLAG_ERROR;
	}


	void FileSystem53::directory() {
		cout << "FS53>\n";
		for (int i = 0; i < MAX_FILE_NO; i++)
		{
			cout << "   File Name:" << dirFile[i].fileName << " File Length (bytes):" << get_desc_size(dirFile[i].desc) << endl;
		}
	}


	void FileSystem53::restore() {
		ifstream inFile(restoreFile.c_str());
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
				inFile >> nextChar;

				// If there is a character left in the file line, copy it to 'p'.
				if (inFile && nextChar != '\n') {
					if (i < 7)
						p[j] = (char)nextChar;
					else
						p[j] = nextChar;
				}
				else if (nextChar == '\n')
					break;
				else if (nextChar == EOF)
					throw "FS53> ERROR >> RESTORE(): File data stream was interrupted.\n";
			}
			write_block(i, p);
			delete[] p;
		}
	}


	void FileSystem53::save() {
		diskdump();

		ofstream outFile(restoreFile.c_str());

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
				if (nextChar != NULL) {
					if (i < 7)
						outFile << (int)nextChar << ' ';
					else
						outFile << nextChar << ' ';
				}
			}
			outFile << '\n';
		}
	}


	void FileSystem53::diskdump() 
	{
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
				if (i == 0){
					p[j] = byteMap[j];
				}
				else if (i > 0 && i < K) {
					p[j] = descTable[i-1][j].size; j++;
					for (int b = 0; b < FILE_BLOCKS_MAX; b++) {
						p[j + b] = descTable[i-1][j / FILE_DESCR_SIZE].blks[b]; j++;
					}
					j--;
				}
				else if (i >= K){
					//if (i < FILE_BLOCKS_MAX + K) {
					//	p[i][j] = dirFile[j].;
					//}
				}
			}
			write_block(i, p);
		}
	}

};



// COMMAND SHELL FUNCTIONS ///////////////////////////////////////////////////////////////////////


// SHOW COMMAND LIST
void shellCommandList() {

	cout << "|===========================================================================|\n"
		<< "|                       FILE SYSTEM COMMAND INTERFACE                       |\n"
		<< "|===========================================================================|\n"
		<< "| in <restoreFile>    | INITIALIZE  - Start the file system.                |\n"
		<< "| sv <restoreFile>    | SAVE   DISK - Save changes to the restore file.     |\n"
		<< "| cr <name>           | CREATE FILE - Create a new file with a file name.   |\n"
		<< "| de <name>           | DELETE FILE - Delete a file with matching name.     |\n"
		<< "| op <name>           | OPEN   FILE - Open a file with matching name.       |\n"
		<< "| cl <index>          | CLOSE  FILE - Close a file matching OFT index.      |\n"
		<< "| rd <OFI> <N>        | READ   CHAR - From OFT index, read char N-times.    |\n"
		<< "| wr <OFI> <char> <N> | WRITE  CHAR - From OFT index, write a char N-times. |\n"
		<< "| sk <OFI> <pos>      | SKIP   TO   - From OFT index, skip to new position. |\n"
		<< "| dr                  | DIRECTORY   - Display directory contents.           |\n"
		<< "| hp                  | HELP        - Displays this command list.           |\n"
		<< "| sd                  | SHUT DOWN   - Shuts down the file system.           |\n"
		<< "|===========================================================================|\n\n";
}


// INITIALIZE
void shellIn(FileSystem53& fs, string restoreFile){
	ifstream testFile(restoreFile.c_str());
	fs = FileSystem53(restoreFile);

	if (testFile) {
		fs.restore();
		cout << "FS53> Disk restored.\n";
	}
	else
		cout << "FS53> Disk initialized.\n";
}


// SAVE DISK
void shellSv(FileSystem53& fs){
	fs.save();
	cout << "FS53> Disk saved.\n";
}


// CREATE FILE
void shellCr(FileSystem53& fs, string fileName){
	int result = fs.create(fileName);

	if (result == FLAG_SUCCESS)
		cout << "FS53> File '" << fileName << "' created.\n";
	else
		cout << "FS53> Error!\n";
}


// DELETE FILE
void shellDe(FileSystem53& fs, string fileName){
	string fname = fileName;
	int result = fs.deleteFile(fname);

	if (result == FLAG_SUCCESS)
		cout << "FS53> File '" << fname << "' destroyed.\n";
	else
		cout << "FS53> Error!\n";
}


// OPEN FILE
void shellOp(FileSystem53& fs, string fileName){
	string fname = fileName;
	int result = fs.open(fname);

	if (result >= 0)
		cout << "FS53> File '" << fname << "' opened; index = " << result << ".\n";
	else
		cout << "FS53> Error!\n";
}


// CLOSE FILE
void shellCl(FileSystem53& fs, string ofIndex){
	int index = atoi(ofIndex.c_str());

	fs.close(index);
	cout << "FS53> Open File " << index << " closed.\n";
}


// READ CHAR
void shellRd(FileSystem53& fs, string ofIndex, string countN){
	int index = atoi(ofIndex.c_str());
	int count = atoi(countN.c_str());
	char* ma = new char[count];
	int result = fs.read(index, ma, count);

	if (result >= 0) {
		cout << "FS53> " << count << " bytes read from Open File " << index << " : {";
		for (int i = 0; i < count; i++) { cout << ma[i]; }
		cout << "}.\n";
	}
	else
		cout << "FS53> Error!\n";
}


// WRITE CHAR
void shellWr(FileSystem53& fs, string ofIndex, string inputChar, string countN){
	int index = atoi(ofIndex.c_str());
	char c = inputChar[0];
	int count = atoi(countN.c_str());
	int result = fs.write(index, c, count);

	if (result >= 0)
		cout << "FS53> " << count << " bytes written to Open File " << index << ".\n";
	else
		cout << "FS53> Error!\n";
}


// SKIP TO
void shellSk(FileSystem53& fs, string ofIndex, string pos){
	int index = atoi(ofIndex.c_str());
	int position = atoi(pos.c_str());
	int result = fs.lseek(index, position);

	if (result != FLAG_ERROR)
		cout << "FS53> Current position in Open File " << index << " is " << position << ".\n";
	else
		cout << "FS53> Error!\n";
}


// DISPLAY DIRECTORY
void shellDr(FileSystem53& fs){
	fs.directory();
}



/*****************************************************************************************************
-	MAIN BODY FUNCTION
******************************************************************************************************/
int main() {
	FileSystem53 fileSystem;			// File system object.
	bool shuttingDown = false;					// Current command.
	
	shellCommandList();

	while (!shuttingDown){

		// Command line prompt & input.
		cout << "FS53> ";	
		string inputLine;
		getline(cin, inputLine);
		
		// Handles empty input.
		while (inputLine.size() == 0) {		
			cout << "FS53> ";
			getline(cin, inputLine);
		}

		// Adds each element of the user's input, separated by spaces, to the vector.
		stringstream streamIn(inputLine);	// sstream token handler.
		vector<string> userCommands;		// Stack of user commands.
		string buffer;						// Buffer for input tokens.
		int numberOfArgs = 0;				// Number of arguments passed with current command.
		
		while (streamIn >> buffer) {		
			userCommands.push_back(buffer);
			++numberOfArgs;
		}

		// INITIALIZE
		if (userCommands.at(0) == "in") {
			if (numberOfArgs != 2) {	 cout << "FS53> ERROR: Invalid number of arguments. Entered: " << numberOfArgs << ". Needed: 2.\n"; }
			else { shellIn(fileSystem, userCommands.at(1)); }
		}
		else if (fileSystem.restoreFile != "") {
			// SAVE DISK
			if (userCommands.at(0) == "sv") {
				if (numberOfArgs != 1) { cout << "FS53> ERROR: Invalid number of arguments. Entered: " << numberOfArgs << ". Needed: 1.\n"; }
				else { shellSv(fileSystem); }
			}

			// CREATE FILE
			if (userCommands.at(0) == "cr") {
				if (numberOfArgs != 2) { cout << "FS53> ERROR: Invalid number of arguments. Entered: " << numberOfArgs << ". Needed: 2.\n"; }
				else { shellCr(fileSystem, userCommands.at(1)); }
			}

			// DELETE FILE
			else if (userCommands.at(0) == "de") {
				if (numberOfArgs != 2) { cout << "FS53> ERROR: Invalid number of arguments. Entered: " << numberOfArgs << ". Needed: 2.\n"; }
				else { shellDe(fileSystem, userCommands.at(1)); }
			}

			// OPEN FILE
			else if (userCommands.at(0) == "op") {
				if (numberOfArgs != 2) { cout << "FS53> ERROR: Invalid number of arguments. Entered: " << numberOfArgs << ". Needed: 2.\n"; }
				else { shellOp(fileSystem, userCommands.at(1)); }
			}

			// CLOSE FILE
			else if (userCommands.at(0) == "cl") {
				if (numberOfArgs != 2) { cout << "FS53> ERROR: Invalid number of arguments. Entered: " << numberOfArgs << ". Needed: 2.\n"; }
				else { shellCl(fileSystem, userCommands.at(1)); }
			}

			// READ CHAR
			else if (userCommands.at(0) == "rd") {
				if (numberOfArgs != 3) { cout << "FS53> ERROR: Invalid number of arguments. Entered: " << numberOfArgs << ". Needed: 3.\n"; }
				else { shellRd(fileSystem, userCommands.at(1), userCommands.at(2)); }
			}

			// WRITE CHAR
			else if (userCommands.at(0) == "wr") {
				if (numberOfArgs != 4) { cout << "FS53> ERROR: Invalid number of arguments. Entered: " << numberOfArgs << ". Needed: 4.\n"; }
				else { shellWr(fileSystem, userCommands.at(1), userCommands.at(2), userCommands.at(3)); }
			}

			// SKIP TO
			else if (userCommands.at(0) == "sk") {
				if (numberOfArgs != 3) { cout << "FS53> ERROR: Invalid number of arguments. Entered: " << numberOfArgs << ". Needed: 3.\n"; }
				else { shellSk(fileSystem, userCommands.at(1), userCommands.at(2)); }
			}

			// DISPLAY DIRECTORY
			else if (userCommands.at(0) == "dr") {
				if (numberOfArgs != 1) { cout << "FS53> ERROR: Invalid number of arguments. Entered: " << numberOfArgs << ". Needed: 1.\n"; }
				else { shellDr(fileSystem); }
			}

			// Exits the file system terminal
			else if (userCommands.at(0) == "sd") {
				shuttingDown = true;
				cout << "FS53> Good bye!\n";
			}

			// Displays the list of commands available
			else if (userCommands.at(0) == "hp") {
				shellCommandList();
			}

			// In case users enters unknown or erroneous command
			else
				cout << "FS53> ERROR: Invalid command. Enter 'hp' for command help.\n";
		}
		else 
			cout << "FS53> ERROR: File System not yet initialized. Enter 'hp' for command help.\n";
	}

	system("pause");
	return 0;
}