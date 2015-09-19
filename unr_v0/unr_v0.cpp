#include "defines.h"

void usage(char *name);

int main(int argc, char* argv[])
{
	/* getopt */
	extern char *optarg;
	register int opt;
	int rezult = 0;

	/* Get options */ 
	while((opt = getopt(argc, argv, "hs:r:t:k:")) != EOF) {
		switch(opt) {     

			/* show help menu */    	
		case 'h' :
			usage(argv[0]);
			return rezult;

		case 'r': {
			if (strlen(optarg) <= 255) {
				CRC32bCalculate *crc32b_calculator = new CRC32bCalculate();
				if (crc32b_calculator) {
					uint8_t buffer[] = "SGMBzCsT3,Z@Ufb";
					uint_least32_t crc = 0x00;
					
					try {
						if (crc32b_calculator->get_crc(buffer, sizeof(buffer), crc)) {
							std::cout << "CRC32: 0x" << std::uppercase << std::hex << crc << std::endl;
							getchar();
							rezult = 0;
						}
					}
					catch (...) {
						rezult = -1;
					}
					delete crc32b_calculator;
					crc32b_calculator = NULL;
				}
				return rezult;
			}
			else {
				Logger::instance().log("Catch error in \"main()\". Too long path to raw file.\n", Logger::kLogLevelError);
				return -1;
			}
		}

		case 't': {
			if (strlen(optarg) <= 255) {
				SHA1_Calculate *sha1_calculator = new SHA1_Calculate();
				if (sha1_calculator) {
					uint8_t buffer[] = "SGMBzCsT3,Z@Ufb", hash[20];
					if (sha1_calculator)
						sha1_calculator->get_sha1_for_winrar(buffer, sizeof(buffer), hash);

					delete sha1_calculator;
					sha1_calculator = NULL;
				}
				return rezult;
			}
			else {
				Logger::instance().log("Catch error in \"main()\". Too long path to raw file.\n", Logger::kLogLevelError);
				return -1;
			}
		}

		case 's': {
			if (strlen(optarg) <= 255) {
				HeaderParser* header_parser = new HeaderParser();
				if (header_parser) {
					if (header_parser->set_path_to_archive(optarg)) {
						try {
							header_parser->show_archive_headers();

							rezult = 0;
						}
						catch (...) {
							rezult = -1;
						}
					}

					delete header_parser;
					header_parser = NULL;
				}
				return rezult;

			}
			else {
				Logger::instance().log("Catch error in \"main()\". Too long path to WinRAR archive.\n", Logger::kLogLevelError);
				return -1;
			}
		}

		case 'k' : {
			if (strlen(optarg) <= 255) {
				Huffman* haffman = new Huffman();
				if (haffman) {

					haffman->encode_into_file(optarg, "D:\\Projects\\ÍÈÐ\\Workspace\\tests\\encode_huffman_into_file.txt", false);
					getchar();

					/*
					if (haffman->set_path_to_file(optarg)) {
						try {
							uint16_t size = 65535;
							size_t pos = 0;
							uint8_t data[65535];
							memset(data, 0, sizeof(data));

							if (haffman->encode_from_file(data, &pos)) {
								for (uint16_t i = 0; i < pos; i++)
									printf("%c", data[i]);
								getchar();
							}

							rezult = 0;
						} catch(...) {
							rezult = -1;
						}
					}
					*/

					delete haffman;
					haffman = NULL;
				}
				return rezult;

			} else {
				Logger::instance().log("Catch error in \"main()\". Too long path to WinRAR archive.\n", Logger::kLogLevelError);
				return -1;
			}
		} 

		default:
			Logger::instance().log("Catch error in \"main()\". Unknown startup option.\n", Logger::kLogLevelError);
			return -1;
		}
	}

	return 0;
}

void usage(char *name) {
	printf("\n>> Bruteforce attack on WinRAR 3.0 by Allow (allowl@mail.ru) <<\n\n");

	//printf("\nusage: %s [options]\n\n", name);	    
	printf("Show startup options:\n");
	printf("  -s [full path to WinRAR archive]              show struct of headers\n");  
	printf("  -a [full path to WinRAR archive]              run bruteforce attack\n");
	printf("  -c [full path to WinRAR archive] [password]   check archive password\n");
	printf("  -r [full path to file with row text]          calculate CRC32b\n");
	printf("  -t [full path to file with row text]          calculate SHA1\n");
	printf("  -k [full path to file with row text]          calculate Haffman code (encode)\n");
	printf("  -h                                            help\n");  
	getchar();
}
