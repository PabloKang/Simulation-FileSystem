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

public:
	FileSystem53::FileSystem53(int l, int b, string storage);
	void	FileSystem53::read_block(int i, char *p);							// COMPLETE
	void	FileSystem53::write_block(int i, char *p);							// COMPLETE
	void	FileSystem53::OpenFileTable();
	int		FileSystem53::find_oft();
	void	FileSystem53::deallocate_oft(int index);
	void	FileSystem53::format();												// COMPLETE
	char*	FileSystem53::read_descriptor(int no);
	void	FileSystem53::clear_descriptor(int no);
	void	FileSystem53::write_descriptor(int no, char* desc);
	int		FileSystem53::find_empty_descriptor();
	int		FileSystem53::find_empty_block();
	int		FileSystem53::fgetc(int index);
	int		FileSystem53::fputc(int c, int index);
	bool	FileSystem53::feof(int index);
	int		FileSystem53::search_dir(int index, string symbolic_file_name);
	void	FileSystem53::delete_dir(int index, int start_pos, int len);
	int		FileSystem53::create(string symbolic_file_name);					// DUE
	int		FileSystem53::open_desc(int desc_no);								// DUE
	int		FileSystem53::open(string symbolic_file_name);						// DUE
	int		FileSystem53::read(int index, char* mem_area, int count);			// DUE
	int		FileSystem53::write(int index, char value, int count);				// DUE
	int		FileSystem53::lseek(int index, int pos);							// DUE
	void	FileSystem53::close(int index);										// DUE
	int		FileSystem53::deleteFile(string fileName);							// DUE
	void	FileSystem53::directory();											// DUE
	void	FileSystem53::restore();											// COMPLETE
	void	FileSystem53::save();												// COMPLETE
	void	FileSystem53::diskdump(int start, int size);
};

