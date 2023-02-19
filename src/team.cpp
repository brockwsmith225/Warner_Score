#include "team.h"
#include <iostream>

Team::TeamMetaData::TeamMetaData(const std::string& n, const std::string& id) : name{ n }, uid{ id }, results{ } {}

void Team::TeamMetaData::addResult(const std::string& opponentName, int difference) {
	if (results.find(opponentName) == results.end()) {
		results[opponentName] = difference;
	}
	else if (results[opponentName] < difference) {
		results[opponentName] = difference;
	}
}

Team::Team::Team(const TeamMetaData& m) : metadata{ m }, curBest{ 0 }, curBestPath{ "0" }, listBestPath{} {}

bool Team::Team::updateCurBest(const std::map<std::string, Team*>& teams) {
	bool changed{ false };
	auto results = metadata.getResults();
	for (const auto& result : results) {
		int margin = result.second;
		auto defeatedTeam = *teams.at(result.first);
		auto dtBest = defeatedTeam.getCurBest();
		auto dtBestPath = defeatedTeam.getCurBestPath();
		auto dtListBestPath = defeatedTeam.getListBestPath();
		//If the best path isn't divisible by the team's uid, the team is not in the path (and if uid is 0, there is no current valid path)
		if (std::find(dtListBestPath.begin(), dtListBestPath.end(), metadata.name) == dtListBestPath.end()) {
			if (dtBest + margin > curBest) {
				curBest = dtBest + margin;
				setListBestPath(dtListBestPath, result.first);
				//curBestPath = dtBestPath * metadata.uid;
				changed = true;
			}
		}
	}
	return changed;
}