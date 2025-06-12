#include <opencv2/opencv.hpp>
#include <string>
#include <bitset>
#include <vector>
#include <cstdlib>

void qim_embed(const std::string& original, const std::string& stego, 
           const std::string& msg, int q) {
    cv::Mat img = cv::imread(original);
    if(img.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return;
    }

    std::string message_binary;
    for(char c : msg) {
        message_binary += std::bitset<8>(c).to_string();
    }
    message_binary += "00000000";

    size_t index = 0;
    const size_t msg_length = message_binary.length();

    for(int h = 0; h < img.rows; ++h) {
        for(int w = 0; w < img.cols; ++w) {
            for(int ch = 0; ch < 3; ++ch) {
                if (index >= msg_length){
                    break;
                }
                else {
                    uchar& pixel = img.at<cv::Vec3b>(h, w)[ch];
                    int v1 = static_cast<int>(pixel);
                    
                    int v2 = q * (v1 / q) + (q / 2) * (message_binary[index] - '0');
                    pixel = static_cast<uchar>(v2);
                    ++index;
                }
                
            }
        }
    }

    if(!cv::imwrite(stego, img)) {
        std::cerr << "Error: Could not save image!" << std::endl;
    }
}

std::string qim_extract(const std::string& stego, int q)
{
    cv::Mat img = cv::imread(stego);
    std::string message_binary = "";
    std::string extracted_message = "";

    for(int h = 0; h < img.rows; ++h) {
        for(int w = 0; w < img.cols; ++w) {
            for(int ch = 0; ch < 3; ++ch) {
                uchar& pixel = img.at<cv::Vec3b>(h, w)[ch];
                int v1 = static_cast<int>(pixel);
                int v2 = q * (v1 / q);
                int v3 = q * (v1 / q) + (q / 2);
                if (abs(v1 - v2) < abs(v1 - v3)) {
                    message_binary += '0';
                }
                else {
                    message_binary += '1';
                }
                if (message_binary.length() >= 8) {
                    std::string byte = message_binary.substr(0, 8);
                    message_binary = message_binary.substr(8);

                    if (byte == "00000000") {
                        return extracted_message;
                    }
                    
                    extracted_message += static_cast<char>(std::stoi(byte, nullptr, 2));
                }
            }
        }
    }
    return extracted_message;
}

void lsb_embed(const std::string& original, const std::string& stego, 
               const std::string& msg) {
    
    cv::Mat img = cv::imread(original);
    if(img.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return;
    }

    std::string message_binary;
    for(char c : msg) {
        message_binary += std::bitset<8>(c).to_string();
    }
    message_binary += "00000000";

    size_t index = 0;
    const size_t msg_length = message_binary.length();

    for(int h = 0; h < img.rows; ++h) {
        for(int w = 0; w < img.cols; ++w) {
            for(int ch = 0; ch < 3; ++ch) {
                if (index >= msg_length){
                    break;
                }
                else {
                    uchar& pixel = img.at<cv::Vec3b>(h, w)[ch];
                    int v1 = static_cast<int>(pixel);
                    if (v1 % 2 == 0) {
                        v1 += static_cast<int>(message_binary[index]);
                    }
                    else {
                        if (message_binary[index] == '0') {
                            v1 -= 1;
                        }
                    }

                    pixel = static_cast<uchar>(v1);
                    ++index;
                }
            }
        }
    }

    if(!cv::imwrite(stego, img)) {
        std::cerr << "Error: Could not save image!" << std::endl;
    }
}

std::string lsb_extract(const std::string& stego) {
    cv::Mat img = cv::imread(stego);
    std::string message_binary = "";
    std::string extracted_message = "";

    for(int h = 0; h < img.rows; ++h) {
        for(int w = 0; w < img.cols; ++w) {
            for(int ch = 0; ch < 3; ++ch) {
                uchar& pixel = img.at<cv::Vec3b>(h, w)[ch];
                int v1 = static_cast<int>(pixel);
                if (v1 % 2 == 0) {
                    message_binary += '0';
                }
                else {
                    message_binary += '1';
                }
                if (message_binary.length() >= 8) {
                    std::string byte = message_binary.substr(0, 8);
                    message_binary = message_binary.substr(8);

                    if (byte == "00000000") {
                        return extracted_message;
                    }
                    
                    extracted_message += static_cast<char>(std::stoi(byte, nullptr, 2));
                }
            }
        }
    }

    return extracted_message;
}

int main() {
    try {
        const std::string original = "1.png";
        const std::string stego = "2.png";
        const std::string message = "abcdefghigk";

        lsb_embed(original, stego, message);
        std::cout << "Message embedded successfully." << std::endl;

        std::cout << "Extracted message: " << lsb_extract(stego) << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}