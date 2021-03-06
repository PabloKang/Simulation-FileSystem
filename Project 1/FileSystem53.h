//============================================================================
// Name        : FileSystem53.h
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : First Project Lab
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace std;


#define DEBUG		 0	// Debug mode flag
#define FLAG_ERROR	-1	// Empty flag for array representation
#define FLAG_SUCCESS 1


// FILESYSTEM53 FORMATTING PARAMETERS:
extern int MAX_BLOCK_NO;					// Maximum number of blocks which can be supported by this file system.
extern int MAX_BLOCK_NO_DIV8;		// Maximum nmber of blocks in the file system, divided by 8.
extern int FILE_SIZE_FIELD;					// Size of file size field in bytes. Maximum file size allowed in this file system is 192.
extern int FILE_BLOCKS_MAX;					// The maximum amount of blocks a file can be separated into.
extern int FILE_DESCR_SIZE;
extern int MAX_FILE_NO;					// Maximum number of files which can be stored by this file system.
extern int MAX_FILE_NAME_LEN;					// Maximum size of file name in byte.
extern int MAX_OPEN_FILES;					// Maximum number of files to open at the same time.
extern int FILEIO_BUFFER_SIZE;					// Size of file IO buffer.
extern int B;					// Size (in bytes) of each block.
extern int K;					// Number of blocks for desc_table
extern int F;					// Number of File Descriptors per block.


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
	}
	void Open(int initSize, int initBlk)
	{
		inUse = true;
		size = initSize;
		blks = new int[FILE_BLOCKS_MAX];
		blks[0] = initBlk;
	}
	void Close()
	{
		inUse = false;
		size = FLAG_ERROR;
		delete[] blks;
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
	}
	void Open(int index, int blk, int fSize)
	{
		inUse = true;
		buffer = new char[B];
		desc = index;
		cpos = 0;
		cblk = blk;
		size = fSize;
	}
	void Close()
	{
		inUse = false;
		delete[] buffer;
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


	char* byteMap;						// Cache of byte map

	FileDescriptor** descTable;			// Cache of file descriptors

	OpenFile* oft;						// Open File Table - Stores info of up to 3 open files.

	Directory*  dirFile;		// Directory File Map - <File Name, File Descriptor Index>


public:


	//	Constructor of file system.
	//	1.	Initialize IO system.
	//	2.	Format it if not done.
	FileSystem53::FileSystem53();


	//	Constructor of file system.
	//	Parameters:
	//	-	(l) : Number of blocks (l)
	//	-	(b) : Size of each block
	//	-	(storage) : Name of text file to save/restore.
	FileSystem53::FileSystem53(int l, int b, string storage);
	

	//	read_block() : Read data to pointer in main memory (p) from the hard disk location on lDisk at index (i).
	void	FileSystem53::read_block(int i, char *p);
	

	//	write_block() : Write data from pointer in main memory (p) to the hard disk location on lDisk at index (i).
	void	FileSystem53::write_block(int i, char *p);
	

	//	Constructor of Open File Table(OFT). 
	void	FileSystem53::OpenFileTable();
	

	//	find_oft() : Finds empty spot in OFT and allocates a new open file.
	int		FileSystem53::find_oft();
	

	//	deallocate_oft() : Removes an open file from OFT.
	void	FileSystem53::deallocate_oft(int index);
	

	//	Format file system.
	//	1. Initialize the first K blocks with zeros.
	//	2. Create root directory descriptor for directory file.
	//	Parameter(s):	none
	//	Return:			none 
	void	FileSystem53::format();
	

	//	Read descriptor
	//	Parameter(s):
	//	-	no: Descriptor number to read
	//	Return:
	//	-	Return char[4] of descriptor */
	FileDescriptor	FileSystem53::read_descriptor(int no);
	

	//	Clear descriptor
	//	1. Clear descriptor entry
	//	2. Clear bitmap
	//	3. Write back to disk
	//	Parameter(s):
	//	-	no: Descriptor number to clear
	//	Return:
	//	-	none
	void	FileSystem53::clear_descriptor(int no);
	

	//	Write descriptor
	//	1. Update descriptor entry
	//	2. Mark bitmap
	//	3. Write back to disk
	//	Parameter(s):
	//	-	no: Descriptor number to write
	//	-	desc: descriptor to write
	//	Return:
	//	-	none
	void	FileSystem53::write_descriptor(int no, FileDescriptor fd);
	

	void	FileSystem53::update_desc_size(int descNum, int value);


	void	FileSystem53::update_desc_block(int descNum, int blockNum, int value);


	int		FileSystem53::get_desc_block_value(int descNum, int blockNum);


	int		FileSystem53::get_desc_size(int descNum);


	//	Search for an unoccupied descriptor.
	//	If ARRAY[0] is zero, this descriptor is not occupied.
	//	Then it returns descriptor number.
	int		FileSystem53::find_empty_descriptor();
	

	/* Search for an unoccupied block.
	*   This returns the first unoccupied block in bitmap field.
	*   Return value -1 means all blocks are occupied.
	* Parameter(s):
	*    none
	* Return:
	*    Returns the block number
	*    -1 if not found
	*/
	int		FileSystem53::find_empty_block();
	

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
	int		FileSystem53::fgetc(int index);


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
	int		FileSystem53::fputc(int c, int index);


	/* Check for the end of file.
	* Parameter(s):
	*    index (descriptor number of the file to be added.)
	* Return:
	*    Return true if end-of-file reached.
	*/
	bool	FileSystem53::feof(int index);


	/* Search for a file
	* Parameter(s):
	*    index: index of open file table
	*    st: The name of file to search.
	* Return:
	*    index: An integer number position of found file entry.
	*    Return -1 if not found.
	*/
	int		FileSystem53::search_dir(int index, string symbolic_file_name);


	/* Clear a file entry from directory file
	*
	* Parameter(s):
	*    index: open file table index
	*    start_pos:
	*    length:
	* Return:
	*    none
	*/
	void	FileSystem53::delete_dir(int index, int start_pos, int len);


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
	int		FileSystem53::create(string symbolic_file_name);


	/* Open file with descriptor number function:
	* Parameter(s):
	*    desc_no: descriptor number
	* Return:
	*    index: index if open file table if successfully allocated.
	*    Return -1 for error.
	*/
	int		FileSystem53::open_desc(int desc_no);	


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
	int		FileSystem53::open(string symbolic_file_name);						// COMPLETE


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
	int		FileSystem53::read(int index, char* mem_area, int count);			// COMPLETE


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
	int		FileSystem53::write(int index, char value, int count);				// DUE


	/* Setting new read/write position function:
	* Parameter(s):
	*    index: File index which indicates the file to be read.
	*    pos: New position in the file. If pos is bigger than file size, set pos to file size.
	* Return:
	*    0 for successful write
	*    -1 value for error case "File hasn't been open"
	*/
	int		FileSystem53::lseek(int index, int pos);							// DUE


	/* Close file function:
	* Parameter(s):
	*    index: The index of open file table
	* Return:
	*    none
	*/
	void	FileSystem53::close(int index);										// COMPLETE


	/* Delete file function:
	*    Delete a file
	* Parameter(s):
	*    symbolic_file_name: a file name to be deleted.
	* Return:
	*    Return 0 with success
	*    Return -1 with error (ie. No such file).
	*/
	int		FileSystem53::deleteFile(string fileName);							// DUE


	//	Directory listing function:
	//	List the name and size of files in the directory. 
	//	We have only one directory in this project.
	//	Example of format:
	//		abc 66 bytes, xyz 22 bytes
	//	Parameter(s):
	//	-	None
	//	Return:
	//	-	None
	void	FileSystem53::directory();


	//	The cr command available to the user in terminal
	//	For use with the shell
	void FileSystem53::shellCr(std::string name);

	//	The de command available to the user in terminal
	//	For use with the shell
	void FileSystem53::shellDe(std::string name);

	//	The op command available to the user in terminal
	//	For use with the shell
	void FileSystem53::shellOp(std::string name);

	//	The cl command available to the user in terminal
	//	For use with the shell
	void FileSystem53::shellCl(std::string name);

	//	The rd command available to the user in terminal
	//	For use with the shell
	void FileSystem53::shellRd(std::string name, std::string number);

	//	The wr command available to the user in terminal
	//	For use with the shell
	void FileSystem53::shellWr(std::string name, std::string something2, std::string something3);

	//	The sk command available to the user in terminal
	//	For use with the shell
	void FileSystem53::shellSk(std::string name, std::string pos);

	//	The dr command available to the user in terminal
	//	For use with the shell
	void FileSystem53::shellDr();

	//	The in command available to the user in terminal
	//	For use with the shell
	void FileSystem53::shellIn(std::string disk_cont);

	//	The sv command available to the user in terminal
	//	For use with the shell
	void FileSystem53::shellSv();


	//	Lists commands available to the user in terminal
	//	For use with the shell
	void FileSystem53::shellCommandList();


	/*------------------------------------------------------------------
	Disk management functions.
	These functions are not really a part of file system.
	They are provided for convenience in this emulated file system.
	------------------------------------------------------------------
	Restores the saved disk image in a file to the array.
	*/
	void	FileSystem53::restore();


	// Saves the array to a file as a disk image.
	void	FileSystem53::save();


	// Disk dump, from block 'start' to 'start+size-1'.
	void	FileSystem53::diskdump(int start, int size);


	string	FileSystem53::toString();

};


// OTHER FUNCTIONS
template <typename T>
void	numberToCharArray(const T& Number, char* cstring);


void	numberToCharArray(int& number, char* cstring, int size);


template <typename T>
T		charArrayToNumber(const string& Text);