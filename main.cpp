#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>


int METHOD;
std::string IMAGE_PATH;
std::string SENS_DATA;


std::string read_file(const std::string &filename){
	std::string res;
	std::string buff;
	std::ifstream f(filename);

	if (f.is_open()){
		while (std::getline(f, buff)){
			res += buff + '\n';
		}
	} else{
		std::cout << "Error: CAN NOT OPEN FILE. Name: " << filename;
	}

	f.close();

	return res;
}



int main(int argc, char const *argv[]){
	// Hello banner
	std::cout << "This is AIP project that can help to hide your data with stegonography algorithms.\n\nUsage: main.exe METHOD \"IMAGE_TO_HIDE.png\" \"Some sensetive data to hide\".\n\nOr just use flag -F to work with file where placed sensetive data.\n";

	if (argc <= 1){
		std::cout << "Error: Incorrect argumets." << std::endl;
		return 1;
	}

	// Putting arguments
	METHOD = atoi(argv[1]);
	IMAGE_PATH = argv[2];

	if (std::string(argv[3]) == "-F"){
		SENS_DATA = read_file(argv[4]);
	} else{
		SENS_DATA = argv[3];
	}

	return 0;
}
