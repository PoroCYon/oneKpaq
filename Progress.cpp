#include "Progress.hpp"
#include "onekpaq_common.h"
#include <iomanip>

static Progress gProgress;

void PROGRESS_START(const char *name, size_t total)
{
	gProgress.start(name, total);
}

void PROGRESS_TICK(const char *name)
{
	gProgress.tick(name);
}

void PROGRESS_END(const char *name)
{
	gProgress.end(name);
}

void Progress::start(const std::string& name, size_t total) {
	if (m_startTime == 0) {
		m_startTime = std::time(nullptr);
	}
	ProgressEntry entry{name, total, 0, m_progress.size()};
	m_progress.insert({name, entry});
}

void Progress::tick(const std::string& name) {
	auto it = m_progress.find(name);
	auto progress = it->second;
	progress.value++;

	it->second = progress;
	print();
}

void Progress::end(const std::string& name) {
	m_progress.erase(name);
}

void Progress::print() {
	std::map<size_t, ProgressEntry> sorted;
	for (auto &it:m_progress) {
		sorted.insert({it.second.position, it.second});
	}
	float range = 1.;
	float value = 0.;
	int spinner = 0;
	for (auto &it:sorted) {
		auto progress = it.second;
		if (progress.total == 0) {
			spinner += progress.value;
		} else {
			float stage = 1.f / progress.total;
			range *= stage;
			value += (progress.value-1) * range;
		}
	}
	const char spinners[4] = {'/', '-', '\\', '|'};
	int barLength = 50;
	int bar = value * (barLength - 1);

	std::string etaString = "...";
	if (value > 0) {
		std::time_t now = std::time(nullptr);
		std::time_t eta = m_startTime + (now - m_startTime) / value;
		std::stringstream etaStringStream;
		etaStringStream << std::put_time(std::localtime(&eta), "%F %X");
		etaString = etaStringStream.str();
	}

	fprintf(stderr, "[%.*s>%.*s|%03.2f%%|%c] ETA: %s         \r",
		bar,           "==================================================",
		barLength-bar, "                                                  ",
		value*100.f, spinners[spinner % 4], etaString.c_str());
}