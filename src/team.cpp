#include "team.h"
#include <iostream>

Team::TeamMetaData::TeamMetaData(const std::string& n, const unsigned int id) : name{ n }, uid{ id }, wins{ } {}

void Team::TeamMetaData::addWin(const std::string& n, unsigned int m) {
	wins[n] = m;
}

Team::Team::Team(const TeamMetaData& m) : metadata{ m }, curBest{ 0 }, curBestPath{ 0 } {}

bool Team::Team::updateCurBest(const std::map<std::string, Team*>& teams) {
	bool changed{ false };
	auto wins = metadata.getWins();
	for (const auto& win : wins) {
		unsigned int margin = win.second;
		auto defeatedTeam = *teams.at(win.first);
		auto dtBest = defeatedTeam.getCurBest();
		auto dtBestPath = defeatedTeam.getCurBestPath();
		//If the best path isn't divisible by the team's uid, the team is not in the path (and if uid is 0, there is no current valid path)
		if (dtBestPath % metadata.uid) {
			if (dtBest + margin > curBest) {
				curBest = dtBest + margin;
				curBestPath = dtBestPath * metadata.uid;
				changed = true;
			}
		}
	}
	return changed;
}