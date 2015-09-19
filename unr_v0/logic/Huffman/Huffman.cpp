#include "Huffman.h"


Huffman::Huffman() : verbose_mode(true) {

}

//Huffman Decoder
void Huffman::decode_into_file(std::string ifile, std::string ofile, bool verbose)
{
	std::ifstream infile(ifile.c_str(), std::ios::in | std::ios::binary);
	if (!infile)
	{
		std::cerr << ifile << " could not be opened!" << std::endl;
		exit(1);
	}

	if (std::ifstream(ofile.c_str()))
	{
		std::cerr << ofile << " already exists!" << std::endl;
		exit(1);
	}

	//open the output file
	std::ofstream outfile(ofile.c_str(), std::ios::out | std::ios::binary);
	if (!outfile)
	{
		std::cerr << ofile << " could not be opened!" << std::endl;
		exit(1);
	}

	//read frequency table from the input file
	unsigned int f[256];
	char c;
	unsigned char ch;
	unsigned int j = 1;
	for (int i = 0; i<256; ++i)
	{
		//read 4 bytes and combine them into one 32 bit u. int value
		f[i] = 0;
		for (int k = 3; k >= 0; --k)
		{
			infile.get(c);
			ch = c;
			f[i] += ch*(j << (8 * k));
		}
	}

	//re-construct the Huffman tree
	Queue<Tree> q(3); //use a 3-(priority)heap (again)
	Tree* tp;

	for (int i = 0; i<256; ++i)
	{
		if (f[i]>0)
		{
			//send freq-char pairs to the priority heap as Huffman trees
			tp = new Tree;
			(*tp).set_freq(f[i]);
			(*tp).set_char(static_cast<unsigned char>(i));
			q.enq(tp);
		}
	}

	//construct the main Huffman Tree (as in Encoder func.)
	Tree* tp2;
	Tree* tp3;

	do
	{
		tp = q.deq();
		if (!q.empty())
		{
			//get the 2 lowest freq. H. trees and combine them into one
			//and put back into the priority heap
			tp2 = q.deq();
			tp3 = new Tree;
			(*tp3).set_freq((*tp).get_freq() + (*tp2).get_freq());
			(*tp3).set_left((*tp).get_root());
			(*tp3).set_right((*tp2).get_root());
			q.enq(tp3);
		}
	} while (!q.empty()); //until all sub-trees combined into one

	if (verbose)
	{
		std::cout << *tp << std::endl; //output the Huffman tree
		//output the char-H. string list 
		(*tp).huf_list((*tp).get_root(), "");
		std::cout << "frequency table is " << sizeof(unsigned int) * 256 << " bytes" << std::endl;
	}

	//read Huffman strings from the input file
	//find out the chars and write into the output file
	std::string st;
	unsigned char ch2;
	unsigned int total_chars = (*tp).get_freq();
	std::cout << "total chars to decode:" << total_chars << std::endl;
	while (total_chars>0) //continue until no char left to decode 
	{
		st = ""; //current Huffman string
		do
		{
			//read H. strings bit by bit
			ch = huf_read(infile);
			if (ch == 0)
				st = st + '0';
			if (ch == 1)
				st = st + '1';
		} //search the H. tree
		while (!(*tp).get_huf_char(st, ch2)); //continue until a char is found

		//output the char to the output file
		outfile.put(static_cast<char>(ch2));
		--total_chars;
	}

	infile.close();
	outfile.close();

} //end of Huffman decoder

bool Huffman::encode_from_file(uint8_t *data, size_t *size) {
	if (inFile.is_open()) {
		try {
			size_t pos = 0;

			//array to hold frequency table for all ASCII characters in the file
			unsigned int f[256];
			for (int i = 0; i<256; ++i)
				f[i] = 0;

			//read the whole file and count the ASCII char table frequencies
			char c;
			unsigned char ch;
			while (inFile.get(c))
			{
				ch = c;
				++f[ch];
			}

			inFile.clear(); //clear EOF flag
			inFile.seekg(0); //reset get() pointer to beginning

			Queue<Tree> q(3); //use a 3-(priority)heap
			Tree* tp;

			for (int i = 0; i<256; ++i)
			{
				//output char freq table to the output file
				//divide 32 bit u. int values into 4 bytes
				/*
				raw_buffer[pos++] = (static_cast<unsigned char>(f[i] >> 24));
				raw_buffer[pos++] = (static_cast<unsigned char>((f[i] >> 16) % 256));
				raw_buffer[pos++] = (static_cast<unsigned char>((f[i] >> 8) % 256));
				raw_buffer[pos++] = (static_cast<unsigned char>(f[i] % 256));
				*/

				if (f[i]>0)
				{
					//send freq-char pairs to the priority heap as Huffman trees
					tp = new Tree;
					(*tp).set_freq(f[i]);
					(*tp).set_char(static_cast<unsigned char>(i));
					q.enq(tp);
				}
			}

			//construct the main Huffman Tree
			Tree* tp2;
			Tree* tp3;

			do
			{
				tp = q.deq();
				if (!q.empty())
				{
					//get the 2 lowest freq. H. trees and combine them into one
					//and put back into the priority heap
					tp2 = q.deq();
					tp3 = new Tree;
					(*tp3).set_freq((*tp).get_freq() + (*tp2).get_freq());
					(*tp3).set_left((*tp).get_root());
					(*tp3).set_right((*tp2).get_root());
					q.enq(tp3);
				}
			} while (!q.empty()); //until all sub-trees combined into one

			//find H. strings of all chars in the H. tree and put into a string table
			std::string H_table[256];
			unsigned char uc;
			for (unsigned short us = 0; us<256; ++us)
			{
				H_table[us] = "";
				uc = static_cast<unsigned char>(us);
				(*tp).huf((*tp).get_root(), uc, "", H_table[us]);
			}

			if (verbose_mode) {
				std::cout << *tp << std::endl; //output the Huffman tree
				//output the char-H. string list 
				(*tp).huf_list((*tp).get_root(), "");
				std::cout << "frequency table is " << sizeof(unsigned int) * 256 << " bytes" << std::endl;
			}

			unsigned int total_chars = (*tp).get_freq();
			std::cout << "total chars to encode:" << total_chars << std::endl;

			//output Huffman coded chars into the output file
			unsigned char ch2;
			while (inFile.get(c))
			{
				ch = c;
				//send the Huffman string to output file bit by bit
				for (unsigned int i = 0; i<H_table[ch].size(); ++i)
				{
					if (H_table[ch].at(i) == '0')
						ch2 = 0;
					if (H_table[ch].at(i) == '1')
						ch2 = 1;
					pos += huf_write(ch2, &raw_buffer[pos], max_size_of_part - pos);
				}
			}
			ch2 = 2; // send EOF
			pos += huf_write(ch2, &raw_buffer[pos], max_size_of_part - pos);

			memcpy(data, raw_buffer, pos);
			*size = pos;
			return true;
		}
		catch (...) {
			throw("Catch error in CRC32Calculate::get_crc(). Calculate crc32 failed with error.");
		}
	}
	else
		throw("Catch error in CRC32Calculate::get_crc(). File with raw text not opened.");

	return false;
}

//Huffman Encoder
void Huffman::encode_into_file(std::string ifile, std::string ofile, bool verbose)
{
	std::ifstream infile(ifile.c_str(), std::ios::in | std::ios::binary);
	if (!infile)
	{
		std::cerr << ifile << " could not be opened!" << std::endl;
		exit(1);
	}

	if (std::ifstream(ofile.c_str()))
	{
		std::cerr << ofile << " already exists!" << std::endl;
		exit(1);
	}

	//open the output file
	std::ofstream outfile(ofile.c_str(), std::ios::out | std::ios::binary);
	if (!outfile)
	{
		std::cerr << ofile << " could not be opened!" << std::endl;
		exit(1);
	}

	//array to hold frequency table for all ASCII characters in the file
	unsigned int f[256];
	for (int i = 0; i<256; ++i)
		f[i] = 0;

	//read the whole file and count the ASCII char table frequencies
	char c;
	unsigned char ch;
	while (infile.get(c))
	{
		ch = c;
		++f[ch];
	}

	infile.clear(); //clear EOF flag
	infile.seekg(0); //reset get() pointer to beginning

	Queue<Tree> q(3); //use a 3-(priority)heap
	Tree* tp;

	for (int i = 0; i<256; ++i)
	{
		//output char freq table to the output file
		//divide 32 bit u. int values into 4 bytes
		outfile.put(static_cast<unsigned char>(f[i] >> 24));
		outfile.put(static_cast<unsigned char>((f[i] >> 16) % 256));
		outfile.put(static_cast<unsigned char>((f[i] >> 8) % 256));
		outfile.put(static_cast<unsigned char>(f[i] % 256));

		if (f[i]>0)
		{
			//send freq-char pairs to the priority heap as Huffman trees
			tp = new Tree;
			(*tp).set_freq(f[i]);
			(*tp).set_char(static_cast<unsigned char>(i));
			q.enq(tp);
		}
	}

	//construct the main Huffman Tree
	Tree* tp2;
	Tree* tp3;

	do
	{
		tp = q.deq();
		if (!q.empty())
		{
			//get the 2 lowest freq. H. trees and combine them into one
			//and put back into the priority heap
			tp2 = q.deq();
			tp3 = new Tree;
			(*tp3).set_freq((*tp).get_freq() + (*tp2).get_freq());
			(*tp3).set_left((*tp).get_root());
			(*tp3).set_right((*tp2).get_root());
			q.enq(tp3);
		}
	} while (!q.empty()); //until all sub-trees combined into one

	//find H. strings of all chars in the H. tree and put into a string table
	std::string H_table[256];
	unsigned char uc;
	for (unsigned short us = 0; us<256; ++us)
	{
		H_table[us] = "";
		uc = static_cast<unsigned char>(us);
		(*tp).huf((*tp).get_root(), uc, "", H_table[us]);
	}

	if (verbose)
	{
		std::cout << *tp << std::endl; //output the Huffman tree
		//output the char-H. string list 
		(*tp).huf_list((*tp).get_root(), "");
		std::cout << "frequency table is " << sizeof(unsigned int) * 256 << " bytes" << std::endl;
	}

	unsigned int total_chars = (*tp).get_freq();
	std::cout << "total chars to encode:" << total_chars << std::endl;

	//output Huffman coded chars into the output file
	unsigned char ch2;
	while (infile.get(c))
	{
		ch = c;
		//send the Huffman string to output file bit by bit
		for (unsigned int i = 0; i<H_table[ch].size(); ++i)
		{
			if (H_table[ch].at(i) == '0')
				ch2 = 0;
			if (H_table[ch].at(i) == '1')
				ch2 = 1;
			huf_write(ch2, outfile);
		}
	}
	ch2 = 2; // send EOF
	huf_write(ch2, outfile);

	infile.close();
	outfile.close();

} //end of Huffman encoder

//input : a input file stream to read bits
//return: unsigned char (:0 or 1 as bit read or 2 as EOF) 
unsigned char Huffman::huf_read(std::ifstream & infile)
{
	static int bit_pos = 0; //0 to 7 (left to right) on the byte block
	static unsigned char c = infile.get();

	unsigned char i;

	i = (c >> (7 - bit_pos)) % 2; //get the bit from the byte
	++bit_pos;
	bit_pos %= 8;
	if (bit_pos == 0)
		if (!infile.eof())
		{
		c = infile.get();
		}
		else
			i = 2;

	return i;
}


//the given bit will be written to the output file stream
//input : unsigned char i(:0 or 1 : bit to write ; 2:EOF) 
void Huffman::huf_write(unsigned char i, std::ofstream & outfile)
{
	static int bit_pos = 0; //0 to 7 (left to right) on the byte block
	static unsigned char c = '\0'; //byte block to write

	if (i<2) //if not EOF
	{
		if (i == 1)
			c = c | (i << (7 - bit_pos)); //add a 1 to the byte
		else //i==0
			c = c & static_cast<unsigned char>(255 - (1 << (7 - bit_pos))); //add a 0
		++bit_pos;
		bit_pos %= 8;
		if (bit_pos == 0)
		{
			outfile.put(c);
			c = '\0';
		}
	}
	else
	{
		outfile.put(c);
	}
}

size_t Huffman::huf_write(uint8_t i, uint8_t *buffer, size_t size)
{
	static int bit_pos = 0; //0 to 7 (left to right) on the byte block
	static unsigned char c = '\0'; //byte block to write
	size_t pos = 0;

	if (i<2) //if not EOF
	{
		if (i == 1)
			c = c | (i << (7 - bit_pos)); //add a 1 to the byte
		else //i==0
			c = c & static_cast<unsigned char>(255 - (1 << (7 - bit_pos))); //add a 0
		++bit_pos;
		bit_pos %= 8;
		if (bit_pos == 0)
		{
			if (size > 0) {
				*(buffer + pos++) = c;
				c = '\0';
			}
			else
				throw "Catch exception in Huffman::huf_write(). Size of part has left.";
			
		}
	}
	else
	{
		if (size > 0) {
			*(buffer + pos++) = c;
			c = '\0';
		}
		else
			throw "Catch exception in Huffman::huf_write(). Size of part has left.";
	}

	return pos;
}

bool Huffman::set_path_to_file(const char *path_to_file) {
	std::ios_base::iostate exceptionMask = inFile.exceptions() | std::ios::failbit;
	try {
		inFile.open(path_to_file, std::ios_base::ate | std::ios_base::_Nocreate | std::ios_base::binary);
		if (inFile.is_open() && inFile.good()) {
			std::streampos length = inFile.tellg();
			inFileSize = length;
			inFile.seekg(0, std::ios::beg);
			if (length > 0 && length < UINT64_MAX)
				return true;
			else {
				inFile.close();
				inFile.clear();
			}
		}
		else if (inFile.fail() || inFile.bad()) {
			Logger::instance().log("Catch error in CRC32Calculate::set_path_to_file()", Logger::kLogLevelError);
			Logger::instance().log(strerror(errno), Logger::kLogLevelError);
		}
	}
	catch (std::ios_base::failure& e) {
		Logger::instance().log("Catch error in CRC32Calculate::set_path_to_file()", Logger::kLogLevelError);
		Logger::instance().log(e.what(), Logger::kLogLevelError);
	}

	return false;
}

Huffman::~Huffman() {
}
