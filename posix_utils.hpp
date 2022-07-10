#ifndef  _POSIX_UTILS_
#define _POSIX_UTILS_

#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <tuple>

namespace posix { 
class Utils
{
public:
	Utils() {};
	~Utils() {};
public:
	static bool WriteFile(const std::string& filename, const std::string& content)
	{
		std::ofstream ofs(filename, std::ios::out | std::ios::binary);

		if (!ofs.is_open()) { return false; }

		ofs.write(content.data(), content.size());
		ofs.flush();

		return true;
	}

	static bool WriteFileLines(const std::string& filename, const std::vector<std::string>& lines)
	{
		std::ofstream ofs(filename, std::ios::out | std::ios::binary);

		if (!ofs.is_open()) { return false; }

		for (const auto& it : lines)
		{
			ofs << it << std::endl;
		}

		ofs.flush();

		return true;
	}


	static bool ReadFile(const std::string& filename, std::string& content)
	{
		std::ifstream ifs(filename, std::ios::in | std::ios::binary);

		if (!ifs.is_open()) { return false; }

		ifs.seekg(0, std::ios::end);
		content.reserve(ifs.tellg());
		ifs.seekg(0, std::ios::beg);

		content.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

		return true;
	}

	static bool ReadFileLines(const std::string filename, std::vector<std::string>& lines)
	{
		std::ifstream ifs(filename, std::ios::in | std::ios::binary);

		if (!ifs.is_open()) { return false; }
		
		for (std::string line; std::getline(ifs, line);)
		{
			lines.push_back(line);
		}

		return true;
	}

	static bool Split(const std::string& content, std::vector<std::string>& results, const std::string& separator)
	{
		std::string::size_type pos1 = 0;
		std::string::size_type pos2 = content.find(separator);

		for (std::string value; std::string::npos != pos2; )
		{
			value = content.substr(pos1, pos2 - pos1);
			results.push_back(value);

			pos1 = pos2 + separator.size();
			pos2 = content.find(separator, pos1);
		}
		if (!content.substr(pos1).empty()) { results.push_back(content.substr(pos1)); }

		return true;
	}

	static bool Replace(std::string& content, const std::string& orign, const std::string& dst)
	{
		if (content.empty()) { return false; }

		for (std::string::size_type pos = 0, orign_len = orign.size(), dst_len = dst.size(); (pos = content.find(orign, pos)) != std::string::npos; )
		{
			content.replace(pos, orign_len, dst);
			pos += dst_len;
		}

		return true;
	}

	// Regex(R"([{]\d+[}])", context, results);
	static bool Regex(const std::string& filter, const std::string& content, std::vector<std::string>& results)
	{
		if (filter.empty() || content.empty()) { return false; }

		std::regex filter_data(filter);

		for (std::sregex_iterator it(content.begin(), content.end(), filter_data); it != std::sregex_iterator(); it++)
		{
			results.push_back(it->str());
		}

		return true;
	}

	//Format("test: <1> = {1}, <2> = {2}, <3> = {3}", 1, 2.0, "3"); -> "test: <1> = 1, <2> = 2.000000, <3> = 3"
	template<typename... Args>
	static std::string Format(const std::string& content, Args... args)
	{
		std::string ret = content;
		std::vector<std::string> args_vec;
		std::initializer_list<int>{([&] {args_vec.push_back(std::to_string(args)); }(), 0)...};
		std::vector<std::string> sources;
		Regex(R"([{]\d+[}])", ret, sources);
		if (sources.size() != args_vec.size())
		{
			return std::string();
		}
		for (int i = 0; i < sources.size(); i++)
		{
			Replace(ret, sources[i], args_vec[i]);
		}
		return ret;
	}
};
}
#endif // ! _POSIX_UTILS_
