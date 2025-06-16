#include <iostream>
#include <vector>
#include <bitset>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



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


int main() {
	// ChannelSwapping csw; // DONE

	// csw.encode("original.png", sens_data, "output.png");
	// std::cout << csw.decode("output.png", csw.get_sens_data_size()) << std::endl;

	// MidBitChange MBC; // DONE

	// MBC.encode("original.png", sens_data, "output.png");
	// std::cout << MBC.decode("output.png", sens_data.size()) << std::endl;
	
	return 0;
}