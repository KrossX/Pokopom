/* Copyright (c) 2014 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#ifndef REGINI_H
#define REGINI_H

namespace regini
{
	using std::string;
	using std::vector;

	struct entry_type
	{
		string key;
		string value;
	};

	struct section_type
	{
		string name;
		vector<entry_type> entry;
	};

	class regini_file
	{
		string filename;

		section_type* current_section;
		vector<section_type> vsection;
		
		void add_section(string in);
		void add_entry(string in);

		section_type* get_section(string section_name);
		entry_type*   get_entry(string section_name, string key_name);

		bool read_string(string section, string key, string &out);
		void write_string(string section, string key, string value);

	public:
		template<class T>
		T read(string section, string key, T default_value)
		{
			string value;
			if (read_string(section, key, value)) default_value = std::stoi(value);
			return static_cast<T>(default_value);
		}

		template<>
		int read(string section, string key, int default_value)
		{
			string value;
			if (read_string(section, key, value)) default_value = std::stoi(value);
			return default_value;
		}

		template<>
		long read(string section, string key, long default_value)
		{
			string value;
			if (read_string(section, key, value)) default_value = std::stol(value);
			return default_value;
		}

		template<>
		long long read(string section, string key, long long default_value)
		{
			string value;
			if (read_string(section, key, value)) default_value = std::stoll(value);
			return default_value;
		}

		template<>
		unsigned long read(string section, string key, unsigned long default_value)
		{
			string value;
			if (read_string(section, key, value)) default_value = std::stoul(value);
			return default_value;
		}

		template<>
		unsigned long long read(string section, string key, unsigned long long default_value)
		{
			string value;
			if (read_string(section, key, value)) default_value = std::stoull(value);
			return default_value;
		}

		template<>
		float read(string section, string key, float default_value)
		{
			string value;
			if (read_string(section, key, value)) default_value = std::stof(value);
			return default_value;
		}

		template<>
		double read(string section, string key, double default_value)
		{
			string value;
			if (read_string(section, key, value)) default_value = std::stod(value);
			return default_value;
		}

		template<>
		long double read(string section, string key, long double default_value)
		{
			string value;
			if (read_string(section, key, value)) default_value = std::stold(value);
			return default_value;
		}

		template<class T>
		void write(string section, string key, T value)
		{
			string string_value = std::to_string(value);
			write_string(section, key, string_value);
		}

		template<>
		void write<const char*>(string section, string key, const char* value)
		{
			string string_value(value);
			write_string(section, key, string_value);
		}

		template<>
		void write<string>(string section, string key, string value)
		{
			write_string(section, key, value);
		}

		bool open(string filename);
		void save();
	};
}

#endif