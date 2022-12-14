#include <iostream>
#include <string>
#include <vector>
#include "solver.h"
#include "team.h"

int main(int argc, char* argv[]) {
	std::vector<Team::TeamMetaData> teamMetas;

	teamMetas.push_back(Team::TeamMetaData("B", "3"));
	teamMetas[0].addResult("W", 1);
	teamMetas[0].addResult("A", 10);
	teamMetas[0].addResult("C", -1);
	teamMetas.push_back(Team::TeamMetaData("W", "1"));
	teamMetas.push_back(Team::TeamMetaData("D", "7"));
	teamMetas[2].addResult("W", 1);
	teamMetas[2].addResult("A", -1);
	teamMetas.push_back(Team::TeamMetaData("A", "2"));
	teamMetas[3].addResult("D", 1);
	teamMetas[3].addResult("C", 1);
	teamMetas[3].addResult("B", -10);
	teamMetas.push_back(Team::TeamMetaData("C", "5"));
	teamMetas[4].addResult("B", 1);
	teamMetas[4].addResult("A", -1);
	std::map<std::string, Team::Team*> teamsMap;
	std::vector<Team::Team* > teams;
	for (const auto& meta : teamMetas) {
		teamsMap[meta.name] = new Team::Team(meta);
		teams.push_back(teamsMap[meta.name]);
	}
	teams[0] = teamsMap["A"];
	teams[1] = teamsMap["B"];
	teams[2] = teamsMap["C"];
	teams[3] = teamsMap["D"];
	teams[4] = teamsMap["W"];
	teamsMap["W"]->setCurBestPath(BigInteger::ONE);
	bool changed = true;
	int i = 0;
	

	//auto result = Solver::getSolution(teams, teamsMap);

	//for (const auto& pair : result) {
	//	std::cout << pair.first << " " << pair.second << std::endl;
	//}

	//Solver::shuffleTeamsList(teams);

	//result = Solver::getSolution(teams, teamsMap);

	//for (const auto& pair : result) {
	//	std::cout << pair.first << " " << pair.second << std::endl;
	//}


	Solver::runSolver();
	

	for (auto& pair : teamsMap) {
		delete pair.second;
	}
}