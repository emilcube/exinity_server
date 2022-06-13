#pragma once 
#include <iostream>
#include <vector>
#include <sstream>


std::string encodeStr(const std::vector<std::string>& vData, const std::string& separator);

bool read_file(std::string& file_data, const std::string& file_name, const std::ios_base::openmode& mode = std::ios_base::in);

bool write_to_file(const std::string& file_data, const std::string& file_name, const std::ios_base::openmode& mode = std::ios_base::out);


inline std::string encodeStr(const std::vector<std::string>& vData, const std::string& separator)
{
	std::stringstream s;
	bool first = true;
	for (const auto& p : vData)
	{
		if (first) first = false;
		else s << separator;
		s << p;
	}
	return s.str();
}




inline bool read_file(std::string& file_data, const std::string& file_name,
	const std::ios_base::openmode& mode)
{
	std::ifstream file;
	file.open(file_name, mode);

	if (file.is_open())
	{
		file_data.clear();
		file_data.assign(std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>());
		return true;
	}
	return false;

}

inline bool write_to_file(const std::string& file_data, const std::string& file_name,
	const std::ios_base::openmode& mode)
{
	std::ofstream file;
	file.open(file_name, mode);

	if (file.is_open())
	{
		file << file_data;
		file.close();
		return true;
	}
	else
	{
		//std::cout << "Filed to open/create file: " << file_name << std::endl;
		return false;
	}


}

inline std::string encodeQueryData(const std::map<std::string, std::string>& data)
{
	std::stringstream s;
	bool first = true;
	for (const auto& pair : data)
	{
		if (!first) s << "&"; else first = !first;
		s << pair.first << "=" << pair.second;
	}
	return s.str();
}

inline bool responseChecker(const std::string& data, boost::json::array& resp)
{
	bool status = true;
	try
	{
		resp = boost::json::parse(data).as_array();
	}
	catch (const boost::exception& e)
	{
		status = false;
		std::cout << "responseChecker array parce error " << boost::diagnostic_information_what(e) << std::endl;
	}
	return status;
}

inline bool responseChecker(const std::string& data, boost::json::object& resp)
{
	bool status = true;
	try
	{
		resp = boost::json::parse(data).as_object();
	}
	catch (const boost::exception& e)
	{
		status = false;
		std::cout << "responseChecker object parce error " << boost::diagnostic_information_what(e) << std::endl;
	}
	return status;
}

extern const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;

}
std::string base64_decode(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}

std::string replaceUrlChars(std::string msg)
{
	//const std::string URLSpChars = "!#$%&'()*+,/:;=?@[]";
	const std::string URLSpChars = ":+=";
	std::stringstream adaptedEntry;
	for (auto& ch : msg)
	{
		if (URLSpChars.find(ch) < URLSpChars.length())
		{
			adaptedEntry << '%' << std::hex << std::uppercase << (int)ch;
		}
		else
		{
			adaptedEntry << ch;
		}
	}
	return adaptedEntry.str();

}

int rand_range(int min_n, int max_n)
{
	int res = 0;
	while (res == 0)
	{
		res = rand() % (max_n - min_n + 1) + min_n;
	}
	return res;
}
