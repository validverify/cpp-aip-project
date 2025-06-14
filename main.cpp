#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>


int METHOD;
std::string SENS_DATA;
std::string IMAGE_PATH;
std::string OUTPUT_PATH;


std::string read_file(const std::string &filename){
	std::string res;
	std::string buff;
	std::ifstream f(filename);

	if (f.is_open()){
		while (std::getline(f, buff)){
			res += buff + '\n';
		}
	} else{
		std::cerr << "Error: CAN NOT OPEN FILE. Name: " << filename;
	}

	f.close();

	return res;
}



int main(int argc, char const *argv[]){
	// Hello banner
	std::cout << "This is AIP project that can help to hide your data with stegonography algorithms.\n\nUsage: main.exe METHOD \"IMAGE_TO_HIDE.png\" \"OUTPUT_FILE.png\" \"Some sensetive data to hide\".\n\nOr just use flag -F to work with file where placed sensetive data.\n";

	if (argc <= 1){
		std::cerr << "Error: Incorrect argumets." << std::endl;
		return 1;
	}

	// Putting arguments
	METHOD = atoi(argv[1]);
	IMAGE_PATH = argv[2];
	OUTPUT_PATH = argv[3];

	if (std::string(argv[4]) == "-F"){
		SENS_DATA = read_file(argv[5]);
	} else{
		SENS_DATA = argv[4];
	}

	return 0;
}
