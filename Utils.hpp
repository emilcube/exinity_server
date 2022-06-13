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

inline std::string encodeStrComma(const std::vector<std::string>& vData)
{
	std::stringstream s;
	bool first = true;
	for (const auto& p : vData)
	{
		if (first) first = false;
		else s << ",";
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


