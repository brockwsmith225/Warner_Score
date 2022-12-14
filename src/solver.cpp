#include <solver.h>
#include <algorithm>
#include <random>
#include <thread>
#include <iostream>

Solver::SolverMetadata::SolverMetadata() {
	bestScoresQueue = std::vector<std::map<std::string, long>>{};
	bestScores = std::map<std::string, long>{};
	teamData = std::vector<Team::TeamMetaData>{};
}

Solver::SolverMetadata* Solver::SolverMetadata::instance{ nullptr };

Solver::SolverMetadata* Solver::SolverMetadata::getInstance() {
	if (!instance)
		instance = new SolverMetadata();
	return instance;
}

/**
* Gets the solution for a single ordering of the team list
*/
std::map<std::string, long> Solver::getSolution(const std::vector<Team::Team*>& teams, const std::map<std::string, Team::Team*>& teamsMap) {
	bool changed = true;
	std::map<std::string, long> results;
	while (changed) {
		changed = false;
		for (auto& t : teams) {
			changed |= t->updateCurBest(teamsMap);
		}
	}
	for (const auto& t : teams) {
		results[t->metadata.name] = t->getCurBest();
	}

	return results;
}

/**
* Gets the team list from the team map
*/
std::vector<Team::Team*> Solver::getTeamsList(const std::map<std::string, Team::Team*>& teamsMap) {
	std::vector<Team::Team*> teams;
	for (const auto& pair : teamsMap) {
		teams.push_back(pair.second);
	}
	return teams;
}

/**
* Initializes a team map from teh metadata
*/
std::map<std::string, Team::Team*> Solver::initializeTeamsMap(const std::vector<Team::TeamMetaData>& metaData) {
	std::map<std::string, Team::Team*> teamsMap;
	for (const auto& team : metaData) {
		teamsMap[team.name] = new Team::Team(team);
	}
	return teamsMap;
}

/**
* Function to shuffle and zero out the teams
* The team with uid 1 should have a best path of 1
*/
void Solver::shuffleTeamsList(std::vector<Team::Team*>& teams) {
	auto rng = std::default_random_engine{};
	std::shuffle(std::begin(teams), std::end(teams), rng);

	// Zero out teams
	for (auto& team : teams) {
		team->setCurBest(0);
		if (!(team->metadata.uid == BigInteger::ONE)) {
			team->setCurBestPath(BigInteger("0"));
		}
		else {
			team->setCurBestPath(BigInteger::ONE);
		}
	}
}

/**
* Function to run a single worker thread
*/
void Solver::runWorkerThread() {
	auto solverData = SolverMetadata::getInstance();
	auto teamData = solverData->teamData;
	auto teamsMap = initializeTeamsMap(teamData);
	auto teamsList = getTeamsList(teamsMap);

	
	while (true) {
		auto solution = getSolution(teamsList, teamsMap);

		//Report solution
		solverData->queueAccess.lock();
		solverData->bestScoresQueue.push_back(solution);
		solverData->queueAccess.unlock();
		solverData->queueCV.notify_all();

		shuffleTeamsList(teamsList);
	}
}

/**
* Function to get a list of primes for unique ids
*/
std::vector<std::string> Solver::getPrimesList(int numTeams) {
	//TODO: Implement
	return {};
}


/**
* Function to initialize the team metadata
*/
void Solver::initializeTeamMetadata() {
	//TODO: Actually read shit
	auto solverData = SolverMetadata::getInstance();
	solverData->teamData.push_back(Team::TeamMetaData("B", "3"));
	solverData->teamData[0].addResult("W", 1);
	solverData->teamData[0].addResult("A", 10);
	solverData->teamData[0].addResult("C", -1);
	solverData->teamData.push_back(Team::TeamMetaData("C", "5"));
	solverData->teamData[1].addResult("B", 1);
	solverData->teamData[1].addResult("A", -1);
	solverData->teamData.push_back(Team::TeamMetaData("A", "2"));
	solverData->teamData[2].addResult("D", 1);
	solverData->teamData[2].addResult("C", 1);
	solverData->teamData[2].addResult("B", -10);
	solverData->teamData.push_back(Team::TeamMetaData("D", "7"));
	solverData->teamData[3].addResult("W", 1);
	solverData->teamData[3].addResult("A", -1);
	solverData->teamData.push_back(Team::TeamMetaData("W", "1"));
}


/**
* Main thread primary solver function
*/
void Solver::runSolver() {
	auto solverData = SolverMetadata::getInstance();
	initializeTeamMetadata();
	std::vector<std::thread> workerThreads{};

	for (int i = 0; i < NUM_THREADS; i++) {
		workerThreads.push_back(std::thread(runWorkerThread));
	}

	while (true) {
		//Process results as they come
		std::unique_lock<std::mutex> lk{ solverData->queueAccess };
		solverData->queueCV.wait(lk, [] {return !SolverMetadata::getInstance()->bestScoresQueue.empty(); });

		auto solution = solverData->bestScoresQueue[0];
		solverData->bestScoresQueue.erase(solverData->bestScoresQueue.begin());
		lk.unlock();

		bool changed = false;
		for (const auto& pair : solution) {
			if (solverData->bestScores.find(pair.first) == solverData->bestScores.end() || solverData->bestScores[pair.first] < pair.second) {
				solverData->bestScores[pair.first] = pair.second;
				changed = true;
			}
		}

		if (changed) {
			for (const auto& pair : solverData->bestScores) {
				std::cout << pair.first << " " << pair.second << std::endl;
			}
		}
	}
}