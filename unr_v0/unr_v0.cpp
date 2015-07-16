#include "defines.h"

void usage(char *name);

int main(int argc, char* argv[])
{
	/* getopt */
	extern char *optarg;
	register int opt;
	int rezult = 0;

	/* Get options */ 
	while((opt = getopt(argc, argv, "hs:")) != EOF) {
		switch(opt) {     

			/* show help menu */    	
		case 'h' :
			usage(argv[0]);
			return rezult;

		case 's' : {
			if (strlen(optarg) <= 255) {
				HeaderParser* header_parser = new HeaderParser();
				if (header_parser) {
					if (header_parser->set_path_to_archive(optarg)) {
						try {
							header_parser->show_archive_headers();

							rezult = 0;
						} catch(...) {
							rezult = -1;
						}
					}

					delete header_parser;
					header_parser = NULL;
				}
				return rezult;

			} else {
				Logger::instance().log("Catch error \"main()\". Too long path to WinRAR archive.\n", Logger::kLogLevelError);
				return -1;
			}
		} 

		default:
			Logger::instance().log("Catch error \"main()\". Unknown startup option.\n", Logger::kLogLevelError);
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
	printf("  -c [full path to WinRAR archive] [password]   check archive password\n");
	printf("  -a [full path to WinRAR archive]              run bruteforce attack\n");    
	printf("  -h                                            help\n");  
	getchar();
}
