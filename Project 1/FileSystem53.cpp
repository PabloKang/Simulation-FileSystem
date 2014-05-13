//============================================================================
// Name        : FileSystem53.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : First Project Lab
//============================================================================
#include "FileSystem53.h"
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

FileSystem53::FileSystem53(int l, int b, string storage) {

	// Initialize byte map cache
	byteMap = new char[MAX_BLOCK_NO];

	// Initialize the file descriptor cache
	descTable = new FileDescriptor*[K - 1];
	for (int i = 0; i < K-1; ++i) 
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
	
	OpenFileTable();
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
	
	//strcpy(directory, "directory" + (char)dirSize); /////////////////////////////////////////////////////////////////////////////////
	write_block(K, directory);

	descTable[0][0].Open(dirSize, K);
	descTable[0][0].size = dirSize;
	for (int i = 0; i < FILE_BLOCKS_MAX; i++){
		descTable[0][0].blks[i] = i;
	}
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
		descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE].blks[i] = desc.blks[i];
}


void FileSystem53::update_desc_size(int no, int fSize)
{
	descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE].size = fSize;
}


void FileSystem53::update_desc_block(int no, int blockNum, int value)
{
	descTable[no / FILE_DESCR_SIZE][no % FILE_DESCR_SIZE].blks[blockNum] = value;

	if (DEBUG) {
		cout << "Updated DescNum " << no << " at block " << blockNum << " with value " << value << endl;
	}
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

		if (DEBUG) {
			cout << "Placed char :" << c << " at index: " << index << " in position: " << curr << endl;
			cout << "OFT[index]: " << oft[index].buffer << endl;
		}

		oft[index].cpos++;
		oft[index].size += 1;
		update_desc_size(oft[index].desc, oft[index].size);
		return oft[index].buffer[curr];
	}
	cout << "Error: " << EOF << "EOF" << endl;
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
		if( search_dir(0, symbolic_file_name) != FLAG_ERROR)
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

		int nextBlock = get_desc_block_value(oft[index].desc, ++oft[index].cblk);
		if (nextBlock == NULL) {
			nextBlock = find_empty_block();
			update_desc_block(oft[index].desc, oft[index].cblk, nextBlock);
			byteMap[nextBlock] = (char)1; // Update bitmap
			if (DEBUG) {
				cout << "Bitmap updated at position: " << nextBlock << endl;
			}
		}
	}

	for (int i = 0; i < count; ++i) {
		fputc(value, index);
	}
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
		throw "ERROR >> CLOSE(): Index Out of Bounds.\n";
}


int FileSystem53::deleteFile(string fileName) {
	int fileDesc;

	fileDesc = search_dir(0, fileName);

	if (fileDesc != FLAG_ERROR) {
		if (DEBUG)
			cout << "File found. Now deleting at:" << fileDesc << endl;

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
	for (int i = 0; i < MAX_FILE_NO; i++)
	{
		cout << "File Name:" << dirFile[i].fileName << " File Length (bytes):" << get_desc_size(dirFile[i].desc) << endl;
	}
}



void FileSystem53::shellCr(std::string name){

	string fname = name;
	cin >> fname;
	int result = create(fname);
	if (result == 0)
		cout << "file " << fname << " created" << endl;
	else
		cout << "error" << endl;
}


void FileSystem53::shellDe(std::string name){

	string fname = name;
	int result = deleteFile(fname);
	if (result == 0)
		cout << "file " << fname << " destroyed" << endl;
	else
		cout << "error" << endl;
}


void FileSystem53::shellOp(std::string name){
	string fname = name;
	int result = open(fname);
	if (result != -1 || result != -2)
		cout << "file " << fname << " opened, index = " << result << endl;
	else
		cout << "error" << endl;
}


void FileSystem53::shellCl(std::string name){
	int index = atoi(name.c_str());
	close(index);
	cout << "file with index " << index << " closed" << endl;
}


void FileSystem53::shellRd(std::string name, std::string number){
	int index = atoi(name.c_str());
	int count = atoi(number.c_str());

	char* ma = new char[count];
	int result = read(index, ma, count);
	if (result != -1 || result != -2)
	{
		cout << count << " bytes read: ";
		for (int i = 0; i < count; i++)
		{
			std::cout << ma[i];
		}
	}
	else
		cout << "error";
}


void FileSystem53::shellWr(std::string name, std::string something2, std::string something3){
	int index = atoi(name.c_str());
	char c = something2[0];
	int count = atoi(something3.c_str());

	int result = write(index, c, count);
	if (result != -1 || result != -2)
		cout << count << " bytes written" << endl;
	else
		cout << "error" << endl;
}


void FileSystem53::shellSk(std::string name, std::string pos){
	int index = atoi(name.c_str());
	int position = atoi(pos.c_str());

	int result = lseek(index, position);
	if (result == 0)
		cout << "current position is " << position << endl;
	else
		cout << "error" << endl;
}


void FileSystem53::shellDr(){
	directory();
}


void FileSystem53::shellIn(std::string disk_cont){

	if (std::ifstream("lDiskISO.txt")) // file exists
	{
		restore();
		cout << "disk restored" << endl;
	}
	else
	{
		FileSystem53 newFS(0, 0, "lDiskISO.txt");
		cout << "disk initialized" << endl;
	}
}


void FileSystem53::shellSv(){
		save();
		cout << "disk saved" << endl;
}


void FileSystem53::shellCommandList() {

	std::cout << "\nThis is the list of commands available...\n"
		<< "\ncr <name>\n   Output: file <name> created\n"
		<< "\nde <name>\n   Output: file <name> destroyed\n"
		<< "\nop <name>\n   Output: file <name> opened, index=<index>\n"
		<< "\ncl <index>\n   Output: file <name> closed\n"
		<< "\nrd <index> <count>\n   Output: <count> bytes read: <xx...x>\n"
		<< "\nwr <index> <char> <count>\n   Output: <count> bytes written\n"
		<< "\nsk <index> <pos>\n   Output: current position is <pos>\n";
	system("pause");
	std::cout << "\ndr\n   Output: file0 <len0>,..., fileN <lenN>\n"
		<< "\nin <disk_cont>\n   -disk_cont is a text file; it holds copy of ldisk"
		<< "\n   -If file does not exist, output: disk initialized"
		<< "\n   -If file does exist, output: disk restored\n"
		<< "\nsv <disk_cont>\n   Output: disk saved\n"
		<< "\nsd\n   Shuts down the system\n"
		<< "\nhp\n   Displays this command list\n"
		<< "\nIf any command fails, output: error\n\n";
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
			cin >> nextChar;

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


//void FileSystem53::diskdump(int start, int size) {
//	for (int i = start; i < size; ++i) {
//		write_block(i, descTable[i]);
//	}
//	save();
//}


//// Prints entire filesystem to console
//string FileSystem53::toString()
//{
//	stringstream ss;
//	for (int i = 0; i < K; ++i) {
//		for (int j = 0; j < MAX_BLOCK_NO; ++j) {
//			ss << descTable[i][j];
//		}
//		ss << '\n';
//	}
//	return ss.str();
//}


// OTHER FUNCTIONS OUTSIDE OF CLASS

template <typename T>
void numberToCharArray(const T& Number, char* cstring)
{
	ostringstream ss;
	ss << Number;
	strcpy(cstring, ss.str().c_str());
}


void numberToCharArray(int& number, char* cstring, int size)
{
	for (int i = 0; i < size; ++i) {
		cstring[i] = '0';
	}
	while (number > 0) {
		cstring[--size] = (char)(((int)'0') + (number % 10));
		number /= 10;
	}
}


template <typename T>
T charArrayToNumber(const string& Text)
{
	istringstream ss(Text);
	T result;
	return ss >> result ? result : 0;
}
