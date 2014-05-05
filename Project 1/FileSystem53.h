//============================================================================
// Name        : FileSystem53.h
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : First Project Lab
//============================================================================

#include <iostream>
#include <string>
using namespace std;

class FileSystem53 {

	int B;				// Block length
	int K;				// Number of blocks for descriptor table
	int F;				// Number of File Descriptors per block
	
	char** desc_table;  // Descriptor table (in memory).
		// This is a cache of the descriptor portion of ldisk. 
		// It's contents should be maintained to be same as first K blocks in disk.
		// Descriptor table format:
		//            +---------------------------------------------+
		// desc_table[| byteMap[] | dsc_0[] | dsc_1[] | ... | dsc_N |]
		//            +---------------------------------------------+
		//   bitmap: Each bit represent a block in a disk. MAX_BLOCK_NO/8 bytes
		//   dsc_0 : Root directory descriptor
		//   dsc_i : i'th descriptor. Each descriptor is FILE_SIZE_FIELD+ARRAY_SIZE bytes long.
	
	char** ldisk;		// 2D Array simulation of a file system (hard disk).
		// ldisk format: 
		//		 +-----------------------------------------------------------------------------------------+
		// ldisk[| byteMap[] | dsc_0[] | dsc_1[] | ... | dsc_N[] | fData_0[] | fData_1[] | ... | fData_L[] |]
		//		 +-----------------------------------------------------------------------------------------+
	
	// Filesystem format parameters:
	int FILE_SIZE_FIELD;    // Size of file size field in bytes. Maximum file size allowed in this file system is 192.
	int FILE_BLOCKS_FIELD;  // The maximum amount of blocks a file can be separated into.
	int FILE_DESCR_SIZE;	// Size of each file descriptor (FILE_SIZE_FIELD + ARRAY_SIZE)
	int MAX_FILE_NO;		// Maximum number of files which can be stored by this file system.
	int MAX_BLOCK_NO;       // Maximum number of blocks which can be supported by this file system.
	int MAX_BLOCK_NO_DIV8;
	int MAX_FILE_NAME_LEN;  // Maximum size of file name in byte.
	int MAX_OPEN_FILE;      // Maximum number of files to open at the same time.
	int FILEIO_BUFFER_SIZE; // Size of file io bufer
	int _EOF;				// End-of-File

    public:

	/******************************
	*	Constructor of this File system.
	*   1. Initialize IO system.
	*   2. Format it if not done.
	*/
	FileSystem53(int l, int b, string storage);

	/******************************
	* read_block : Read data to pointer in main memory (p) from the hard disk location on ldisk at index 'i'.
	*/
	void FileSystem53::read_block(int i, char *p);

	/******************************
	* write_block : Write data from pointer in main memory (p) to the hard disk location on ldisk at index 'i'.
	*/
	void FileSystem53::write_block(int i, char *p);

	// Open File Table(OFT).
	void OpenFileTable();

	// Allocate open file table
	int find_oft();
	
	//Deallocate
	void deallocate_oft(int index);
	
	/* Format file system.
	 *   1. Initialize the first K blocks with zeros.
	 *   2. Create root directory descriptor for directory file.
	 * Parameter(s):
	 *   none
	 * Return:
	 *   none
	 */
	void format();

	/* Read descriptor
	 * Parameter(s):
	 *    no: Descriptor number to read
	 * Return:
	 *    Return char[4] of descriptor
	 */
	char* read_descriptor(int no);

	/* Clear descriptor
	 *   1. Clear descriptor entry
	 *   2. Clear bitmap
	 *   3. Write back to disk
	 * Parameter(s):
	 *    no: Descriptor number to clear
	 * Return:
	 *    none
	 */
	void clear_descriptor(int no);

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
	void write_descriptor(int no, char* desc);

	/* Search for an unoccupied descriptor.
	 * If ARRAY[0] is zero, this descriptor is not occupied.
	 * Then it returns descriptor number.
	 */
	int find_empty_descriptor();

	/* Search for an unoccupied block.
	 *   This returns the first unoccupied block in bitmap field.
	 *   Return value -1 means all blocks are occupied.
	 * Parameter(s):
	 *    none
	 * Return:
	 *    Returns the block number
	 *    -1 if not found
	 */
	int find_empty_block();

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
	int fgetc(int index);

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
	int fputc(int c, int index);

	/* Check for the end of file.
	 * Parameter(s):
	 *    index (descriptor number of the file to be added.)
	 * Return:
	 *    Return true if end-of-file reached.
	 */
	bool feof(int index);

	/* Search for a file
	 * Parameter(s):
	 *    index: index of open file table
	 *    st: The name of file to search.
	 * Return:
	 *    index: An integer number position of found file entry.
	 *    Return -1 if not found.
	 */
	int search_dir(int index, string symbolic_file_name);

	/* Clear a file entry from directory file
	 *
	 * Parameter(s):
	 *    index: open file table index
	 *    start_pos:
	 *    length:
	 * Return:
	 *    none
	 */
	void delete_dir(int index, int start_pos, int len);

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
	int create(string symbolic_file_name);

	/* Open file with descriptor number function:
	 * Parameter(s):
	 *    desc_no: descriptor number
	 * Return:
	 *    index: index if open file table if successfully allocated.
	 *    Return -1 for error.
	 */
	int open_desc(int desc_no);

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
	int open(string symbolic_file_name);

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
	int read(int index, char* mem_area, int count);

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
	int write(int index, char value, int count);

	/* Setting new read/write position function:
	 * Parameter(s):
	 *    index: File index which indicates the file to be read.
	 *    pos: New position in the file. If pos is bigger than file size, set pos to file size.
	 * Return:
	 *    0 for successful write
	 *    -1 value for error case "File hasn't been open"
	 */
	int lseek(int index, int pos);

	/* Close file function:
	 * Parameter(s):
	 *    index: The index of open file table
	 * Return:
	 *    none
	 */
	void close(int index);

	/* Delete file function:
	 *    Delete a file
	 * Parameter(s):
	 *    symbolic_file_name: a file name to be deleted.
	 * Return:
	 *    Return 0 with success
	 *    Return -1 with error (ie. No such file).
	 */
	int deleteFile(string fileName);

	/* Directory listing function:
	 *    List the name and size of files in the directory. (We have only one directory in this project.)
	 *    Example of format:
	 *       abc 66 bytes, xyz 22 bytes
	 * Parameter(s):
	 *    None
	 * Return:
	 *    None
	 */
	void directory();
	
	/*------------------------------------------------------------------
	  Disk management functions.
	  These functions are not really a part of file system.
	  They are provided for convenience in this emulated file system.
	  ------------------------------------------------------------------
	 Restores the saved disk image in a file to the array.
	 */
	void restore();

	// Saves the array to a file as a disk image.
	void save();

	// Disk dump, from block 'start' to 'start+size-1'.
	void diskdump(int start, int size);
};

	