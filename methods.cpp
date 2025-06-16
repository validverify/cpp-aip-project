#include <iostream>
#include <vector>
#include <bitset>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



/**
 * @brief BasicImage is class that realise basic work with image throw stb_image module.
 *
 * @throw CAN_NOT_LOAD_IMAGE_FILE(LOAD_IMAGE_PIXELS:CAN_NOT_LOAD_IMAGE_FILE) - When image loader in stb_image can not open image file;
 * @throw CAN_NOT_SAVE_IMAGE - When function save_result can not save image with redacted data throw stb_image module;
 *
 * @return In constructor function class returns own object with accessible(public) functions: get_image_loader, get_image_params, get_pixels_range, save_result, free_space
*/
class BasicImage {
public:
	/**
	 * @brief Constructor for class BasicImage, that helps work with images.
	 *
	 * @param img_path Constant that contains path to image file.
	 * @return BasicImage class.
	*/
	BasicImage(const std::string &img_path){
		loaded_image = stbi_load(img_path.c_str(), &w, &h, &ch, 3);

		if (!loaded_image) {
			std::cerr << "Error: CAN NOT LOAD IMAGE FILE." << std::endl;
			throw std::runtime_error("LOAD_IMAGE_PIXELS:CAN_NOT_LOAD_IMAGE_FILE.");
		}

		pixels.assign(loaded_image, loaded_image + w * h * 3);
	}

	/**
	 * @brief Just return loader object of image
	 *
	 * @return unsigned char image.
	*/
	unsigned char* get_image_loader(){
		return loaded_image;
	}

	/**
	 * @brief Function to get image basic params such a Width, Height, Channels.
	 *
	 * @return Integer vector with 3 params.
	*/
	std::vector<int> get_image_params(){
		return {w, h, ch};
	}

	/**
	 * @brief Just return result of image assigned params.
	 *
	 * @return Unsigned char vector with pixels dataset.
	*/
	std::vector<unsigned char> get_pixels_range(){
		return pixels;
	}

	/**
	 * @brief Function that saves result of working with image and free space.
	 *
	 * @param output_path Constant value of output image path.
	*/
	void save_result(const std::string &output_path){
		if (!stbi_write_png(output_path.c_str(), w, h, 3, pixels.data(), w * 3)) {
			std::cerr << "Error: CAN NOT SAVE IMAGE." << std::endl;
		}

		stbi_image_free(loaded_image);
	}

	/**
	 * @brief Function to free space that contains image data
	*/
	void free_space(){
		stbi_image_free(loaded_image);
	}

private:
	int w, h, ch;
	unsigned char *loaded_image;
	std::vector<unsigned char> pixels;
};



/**
 * @brief ChannelSwapping is class that realise encode and decode functionality of Channel Swapping encrypt algotithm.
 *
 * @throw TOO_MANY_SENSETIVE_DATA_TO_ENCODE - When encode function got too big sensetive data message even image can hold this;
 * @throw IMAGE_CAN_NOT_BE_OPEN_TO_DECODE - When image loader on stb_image module can not open image file;
*/
class ChannelSwapping {
public:
	/**
	 * @brief Encode function with Channel Swapping algorithm.
	 *
	 * @param img_path Constant that contains path to input image file.
	 * @param sens_data Sensetive data to encode.
	 * @param output_path Constant that contains path to output image file, save file.
	*/
	void encode(const std::string &img_path, const std::string &sens_data, const std::string &output_path) {
		BasicImage image(img_path);

		std::vector<int> _params = image.get_image_params();
		int w = _params[0], h = _params[1], ch = _params[2];

		std::vector<unsigned char> pixels = image.get_pixels_range();

		size_t bit_pos = 0;
		const size_t total_bits = sens_data.size() * 8;

		if (total_bits > (w * h)){
			std::cerr << "Error: TOO MANY SENSETIVE DATA TO ENCODE. R: Sensitive data to encode is too large for this image file. Data: " << total_bits << " - sens data size, " <<  pixels.size() << " - image pixels total size." << std::endl;
			return;
		}

		for (size_t i = 0; i < pixels.size() && bit_pos < total_bits; i += 3) {
			if (bit_pos >= total_bits) break;
			
			char current_char = sens_data[bit_pos / 8];
			bool bit = (current_char >> (7 - (bit_pos % 8))) & 1;
			
			if (bit && pixels[i] < pixels[i+1]) {
				std::swap(pixels[i], pixels[i+1]);
			}
			else if (!bit && pixels[i] > pixels[i+1]) {
				std::swap(pixels[i], pixels[i+1]);
			}
			
			bit_pos++;
		}

		image.save_result(output_path);
		sens_size = sens_data.size();
	}

	/**
	 * @brief Decode function with Channel Swapping algorithm.
	 *
	 * @param img_path Constant that contains path to input image file.
	 * @param sens_data_lenght Lenght of data, that was encoded to image.
	 * @return String value that is result of decoding proccess.
	*/
	std::string decode(const std::string &img_path, long long int sens_data_length) {
		int w, h, ch;
		unsigned char *image = stbi_load(img_path.c_str(), &w, &h, &ch, 3);
		
		if (!image) {
			std::cerr << "Error: IMAGE CAN NOT BE OPENED TO DECODE." << std::endl;
			return "";
		}

		std::string bit_string;
		const size_t total_bits = sens_data_length * 8;

		for (size_t i = 0; i < w * h * 3 && bit_string.size() < total_bits; i += 3) {
			bit_string += (image[i] > image[i+1]) ? '1' : '0';
		}

		stbi_image_free(image);

		std::string result;
		for (size_t i = 0; i + 8 <= bit_string.size(); i += 8) {
			result += static_cast<char>(
				std::bitset<8>(bit_string.substr(i, 8)).to_ulong()
			);
		}

		return result;
	}

	/**
	 * @brief Just set sensetive data lenght to private function.
	 *
	 * @param size Size of sensetive data.
	*/
	void set_sens_data_size(long long int size) {
		sens_size = size;
	}

	/**
	 * @brief Just get sensetive data lenght from private function.
	 *
	 * @return Int variable of sensitive data lenght.
	*/
	long long int get_sens_data_size() {
		return sens_size;
	}

private:

	long long int sens_size = 0;

};



/**
 * @brief MidBitChange is class that realise encode and decode functionality of Mid Bit Changing encrypt algotithm.
 *
 * @throw MESSAGE_TO_LARGE_FOR_IMAGE - When encode function got too big sensetive data message even image can hold this;
 * @throw FAILED_TO_SAVE_IMAGE - When image loader on stb_image module can not save redacted image in encode function;
*/
class MidBitChange {
public:
	/**
	 * @brief Encode function with Mid Bit Changing algorithm.
	 *
	 * @param img_path Constant that contains path to input image file.
	 * @param sens_data Sensetive data to encode.
	 * @param output_path Constant that contains path to output image file, save file.
	*/
	void encode(const std::string &img_path, const std::string &sens_data, const std::string &output_path) {
		BasicImage image(img_path);
		std::vector<unsigned char> pixels = image.get_pixels_range();
		const size_t total_bits = sens_data.size() * 8;

		if (total_bits > pixels.size()) {
			std::cerr << "Error: Message too large for the image." << std::endl;
			return;
		}

		size_t bit_pos = 0;
		for (size_t i = 0; i < pixels.size() && bit_pos < total_bits; ++i) {
			if (i % 3 == 2) continue;

			char current_char = sens_data[bit_pos / 8];
			bool bit = (current_char >> (7 - (bit_pos % 8))) & 1;

			// Changing 4-th bit(stats with zero)
			pixels[i] = (pixels[i] & 0xEF) | (bit << 4);
			bit_pos++;
		}

		if (!stbi_write_png(output_path.c_str(), image.get_image_params()[0], image.get_image_params()[1], 
							image.get_image_params()[2], pixels.data(), image.get_image_params()[0] * 3)) {
			std::cerr << "Error: Failed to save image." << std::endl;
		}
	}

	/**
	 * @brief Decode function with Mid Bit Changing algorithm.
	 *
	 * @param img_path Constant that contains path to input image file.
	 * @param sens_data_size Lenght of data, that was encoded to image.
	 *
	 * @return String value that is result of decoding proccess.
	*/
	std::string decode(const std::string &img_path, const size_t sens_data_size) {
		BasicImage image(img_path);
		std::vector<unsigned char> pixels = image.get_pixels_range();
		std::string binary_str;

		for (size_t i = 0; i < pixels.size() && binary_str.size() < sens_data_size * 8; ++i) {
			if (i % 3 == 2) continue; // Skipping blue channel

			bool bit = (pixels[i] >> 4) & 1;
			binary_str += (bit ? '1' : '0');
		}

		// Bits to string
		std::string result;
		for (size_t i = 0; i + 8 <= binary_str.size(); i += 8) {
			result += static_cast<char>(std::bitset<8>(binary_str.substr(i, 8)).to_ulong());
		}

		return result;
	}
};



/**
 * @brief EOFHiding is class that realise encode and decode functionality of End Of File hidding algotithm.
 *
 * @throw FILE_CAN_NOT_BE_OPEN - Universal error that appears when image file can not be open.
 * @throw FILE_IS_EMPTY - universal Error that appears when image file is empty.
 * @throw SENS_DATA_SIZE_IS_INCCORRECT - When sensetive data size is negative or equals zero.
*/
class EOFHiding {
public:
	/**
	 * @brief Hidding(encoding) sensetive data in the end of file (EOF Steganography).
	 * 
	 * @param img_path Path to original(container) image file.
	 * @param sens_data Sensetive data to hide.
	 * @param output_path Output file path and name.
	*/
	void encode(const std::string &img_path, const std::string &sens_data, const std::string &output_path) {
		std::ifstream in(img_path, std::ios::binary);

		if (!in.is_open()) {
			throw std::runtime_error("Error FILE_CAN_NOT_BE_OPEN: Failed to open input file '" + img_path + "'");
		}

		in.seekg(0, std::ios::end);

		if (in.tellg() == 0) {
			throw std::runtime_error("Error FILE_IS_EMPTY: Input file '" + img_path + "' is empty");
		}

		in.seekg(0, std::ios::beg);

		std::ofstream out(output_path, std::ios::binary);

		if (!out.is_open()) {
			throw std::runtime_error("Error FILE_CAN_NOT_BE_OPEN: Failed to open input file '" + output_path + "'");
		}

		out << in.rdbuf(); // Just copy

		out.write(sens_data.c_str(), sens_data.size()); // Adding sens data
	}

	/**
	 * @brief Taking(decoding) sensetive data from th end of file.
	 * 
	 * @param img_path Path to container image file.
	 * @param sens_data_size Lenght of hidden sensetive data.
	 *
	 * @return String data that is result of decoding.
	*/
	std::string decode(const std::string &img_path, long long int sens_data_size) {
		if (sens_data_size <= 0) {
			throw std::runtime_error("Error SENS_DATA_SIZE_IS_INCCORRECT: Invalid data size (must be positive)");
		}

		std::ifstream in(img_path, std::ios::binary | std::ios::ate);

		if (!in.is_open()) {
			throw std::runtime_error("Error FILE_CAN_NOT_BE_OPEN: Failed to open input file '" + img_path + "'");
		}

		long long int fileSize = in.tellg();
		in.seekg(fileSize - sens_data_size);

		std::vector<char> hidden_data(sens_data_size);
		in.read(hidden_data.data(), sens_data_size);

		std::string res (hidden_data.begin(), hidden_data.end());

		return res;
	}
};




int main() {
	std::string sens_data {"This is a sensetive data!"};

	// ChannelSwapping CSW; // DONE

	// CSW.encode("original.png", sens_data, "output.png");
	// std::cout << CSW.decode("output.png", CSW.get_sens_data_size()) << std::endl;

	// MidBitChange MBC; // DONE

	// MBC.encode("original.png", sens_data, "output.png");
	// std::cout << MBC.decode("output.png", sens_data.size()) << std::endl;

	EOFHiding EOFH; // DONE

	EOFH.encode("original.png", sens_data, "output.png");
	std::cout << EOFH.decode("output.png", sens_data.size()) << std::endl;
	
	return 0;
}