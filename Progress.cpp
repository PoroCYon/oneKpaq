#include "Progress.hpp"
#include "onekpaq_common.h"
#include <assert.h>

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
	// std::string out = "";
	for (auto &it:sorted) {
		auto progress = it.second;
		if (progress.total == 0) {
			spinner += progress.value;
		} else {
			assert(progress.value <= progress.total);
			float stage = 1.f / progress.total;
			range *= stage;
			value += (progress.value-1) * range;
			// out += std::string("[") + progress.name + std::string(" ") + std::to_string(progress.value) + std::string("/") + std::to_string(progress.total) + std::string("] ");
		}
	}
	const char spinners[4] = {'/', '-', '\\', '|'};
	int barLength = 50;
	int bar = value * (barLength - 1);

	fprintf(stderr, "[%.*s>%.*s] %3.2f %c   \r",
		bar,           "==================================================",
		barLength-bar, "                                                  ",
		value*100.f, spinners[spinner % 4]);
}