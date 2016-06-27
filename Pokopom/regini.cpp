/* Copyright (c) 2014 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "regini.h"

namespace regini
{
	template void regini_file::write<int>(string section, string key, int value);
	template void regini_file::write<long>(string section, string key, long value);
	template void regini_file::write<long long>(string section, string key, long long value);
	template void regini_file::write<unsigned>(string section, string key, unsigned value);
	template void regini_file::write<unsigned long>(string section, string key, unsigned long value);
	template void regini_file::write<unsigned long long>(string section, string key, unsigned long long value);
	template void regini_file::write<float>(string section, string key, float value);
	template void regini_file::write<double>(string section, string key, double value);
	template void regini_file::write<long double>(string section, string key, long double value);
	

	const string winreg = "Windows Registry Editor Version 5.00";

	static inline string string_tolower(string in)
	{
		string out = in;
		std::transform(out.begin(), out.end(), out.begin(), ::tolower);
		return out;
	}

	void regini_file::add_section(string in)
	{
		in = in.substr(1, in.size() - 2);

		for (size_t i = 0; i < vsection.size(); i++)
		{
			if (vsection[i].name.compare(in) == 0)
			{
				current_section = &vsection[i];
				return;
			}
		}

		section_type newsection;
		newsection.name = in;
		vsection.push_back(newsection);
		current_section= &vsection.back();
	}

	void regini_file::add_entry(string in)
	{
		size_t pos = in.find('=');

		entry_type newentry;
		newentry.key = in.substr(1, pos - 2);

		for (entry_type et : current_section->entry)
		{
			if (et.key.compare(newentry.key) == 0)
			{
				et.value = in.substr(pos + 2, in.size() - pos - 3);
				return;
			}
		}

		newentry.value = in.substr(pos + 2, in.size() - pos - 3);
		current_section->entry.push_back(newentry);
	}


	bool regini_file::open(string fname)
	{
		filename = fname;
		std::ifstream file(fname);
		vector<string> line;

		vsection.clear();

		if (file.is_open())
		{
			while (!file.eof())
			{
				string newline;
				std::getline(file, newline);
				line.push_back(newline);
			}

			file.close();

			if (!line.empty())
			{
				current_section = nullptr;

				for (string str : line)
				{
					if (str.empty()) continue;

					size_t size = str.size();

					if (str[0] == '[' && str[size - 1] == ']')
					{
						add_section(str);
					}
					else if (str[0] == '"' && str.find('=') != string::npos && current_section != 0)
					{
						add_entry(str);
					}
				}
			}
		}

		return !vsection.empty();
	}

	void regini_file::save()
	{
		if (vsection.empty()) return;

		std::ofstream file(filename, std::ios::out | std::ios::trunc);

		if (file.is_open())
		{
			//file << winreg << "\n\n";

			for (section_type sec : vsection)
			{
				file << "[" << sec.name << "]\n";

				for (entry_type entry : sec.entry)
				{
					file << "\"" << entry.key << "\"" << "=\"" << entry.value << "\"\n";
				}

				file << "\n";
			}

			file.close();
		}
	}

	section_type* regini_file::get_section(string section_name)
	{
		section_name = string_tolower(section_name);

		for (size_t i = 0; i < vsection.size(); i++)
		{
			string sname = string_tolower(vsection[i].name);

			if (sname.compare(section_name) == 0)
				return &vsection[i];
		}

		return nullptr;
	}

	entry_type* regini_file::get_entry(string section_name, string key_name)
	{
		section_type* sec = get_section(section_name);

		if (sec != nullptr)
		{
			key_name = string_tolower(key_name);

			for (size_t i = 0; i < sec->entry.size(); i++)
			{
				string ekeyname = string_tolower(sec->entry[i].key);

				if (ekeyname.compare(key_name) == 0)
					return &sec->entry[i];
			}
		}

		return nullptr;
	}

	bool regini_file::read_string(string section, string key, string &out)
	{
		entry_type *entry = get_entry(section, key);

		if (entry != nullptr)
		{
			out = entry->value;
			return true;
		}
		else
		{
			return false;
		}
	}

	void regini_file::write_string(string section, string key, string value)
	{
		section_type *sec = get_section(section);

		entry_type newentry;
		newentry.key = key;
		newentry.value = value;

		if (sec == nullptr)
		{
			section_type newsection;
			newsection.name = section;
			newsection.entry.push_back(newentry);
			this->vsection.push_back(newsection);
		}
		else
		{
			entry_type *entry = get_entry(section, key);

			if (entry == nullptr)
			{
				sec->entry.push_back(newentry);
			}
			else
			{
				entry->value = newentry.value;
			}
		}
	}


} // end regini namespace



