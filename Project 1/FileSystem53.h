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
	int FILE_INDEX_FIELD;	// Size of the file block index field in bytes. 
	int FILE_BLOCKS_MAX;	// The maximum amount of blocks a file can be separated into.
	int FILE_DESCR_SIZE;	// Size of each file descriptor (FILE_SIZE_FIELD + (FILE_INDEX_FIELD * FILE_BLOCKS_MAX))
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

	void	FileSystem53::read_block(int i, char *p);			// COMPLETE
	void	FileSystem53::write_block(int i, char *p);			// COMPLETE
	void	OpenFileTable();
	int		find_oft();
	void	deallocate_oft(int index);
	void	format();											// COMPLETE
	char*	read_descriptor(int no);
	void	clear_descriptor(int no);							
	void	write_descriptor(int no, char* desc);
	int		find_empty_descriptor();
	int		find_empty_block();
	int		fgetc(int index);
	int		fputc(int c, int index);
	bool	feof(int index);
	int		search_dir(int index, string symbolic_file_name);
	void	delete_dir(int index, int start_pos, int len);
	int		create(string symbolic_file_name);					// DUE
	int		open_desc(int desc_no);								// DUE
	int		open(string symbolic_file_name);					// DUE
	int		read(int index, char* mem_area, int count);			// DUE
	int		write(int index, char value, int count);			// DUE
	int		lseek(int index, int pos);							// DUE
	void	close(int index);									// DUE
	int		deleteFile(string fileName);						// DUE
	void	directory();										// DUE
	void	restore();											// COMPLETE
	void	save();												// COMPLETE
	void	diskdump(int start, int size);
};

	