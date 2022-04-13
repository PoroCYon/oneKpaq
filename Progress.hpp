#pragma once
#include <string>
#include <map>
#include <ctime>

class Progress {
public:
	void start(const std::string& name, size_t total);
	void tick(const std::string& name);
	void end(const std::string& name);

	struct ProgressEntry {
		std::string name;
		size_t total;
		size_t value;
		size_t position;
	};

private:
	void print();
	std::map<std::string, ProgressEntry> m_progress;
	std::time_t m_startTime = 0;
};
