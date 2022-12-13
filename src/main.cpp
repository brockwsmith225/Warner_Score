#include <iostream>
#include <string>
#include <vector>
#include "team.h"
int main(int argc, char* argv[]) {
	std::vector<Team::TeamMetaData> teamMetas;
	teamMetas.push_back(Team::TeamMetaData("A", "2"));
	teamMetas[0].addResult("D", 1);
	teamMetas[0].addResult("C", 1);
	teamMetas[0].addResult("B", -10);
	teamMetas.push_back(Team::TeamMetaData("C", "5"));
	teamMetas[1].addResult("B", 1);
	teamMetas[1].addResult("A", -1);
	teamMetas.push_back(Team::TeamMetaData("B", "3"));
	teamMetas[2].addResult("W", 1);
	teamMetas[2].addResult("A", 10);
	teamMetas[2].addResult("C", -1);
	teamMetas.push_back(Team::TeamMetaData("D", "7"));
	teamMetas[3].addResult("W", 1);
	teamMetas[3].addResult("A", -1);
	teamMetas.push_back(Team::TeamMetaData("W", "1"));
	std::map<std::string, Team::Team*> teamsMap;
	std::vector<Team::Team* > teams;
	for (const auto& meta : teamMetas) {
		teamsMap[meta.name] = new Team::Team(meta);
		teams.push_back(teamsMap[meta.name]);
	}
	teamsMap["W"]->setCurBestPath(1);
	bool changed = true;
	int i = 0;

	while (changed && i < teams.size()) {
		changed = false;
		//++i;
		for (auto& t : teams) {
			changed |=  t->updateCurBest(teamsMap);
		}
	}
	for (const auto& t : teams) {
		std::cout << t->metadata.name << " " << t->getCurBest() << std::endl;
	}
	for (auto& pair : teamsMap) {
		delete pair.second;
	}
}