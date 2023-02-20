#include <solver.h>
#include <algorithm>
#include <random>
#include <thread>
#include <iostream>
#include <fstream>
#include <rapidcsv.h>

Solver::SolverMetadata::SolverMetadata() {
	bestScoresQueue = std::vector<std::map<std::string, std::pair<long, std::vector<std::string>>>>{};
	bestScores = std::map<std::string, std::pair<long, std::vector<std::string>>>{};
	teamData = std::vector<Team::TeamMetaData>{};

	rapidcsv::Document games("warner_score.csv");
	auto teams = games.GetColumn<std::string>("team");
	auto warnerScores = games.GetColumn<int>("warner_score");
	for (int i = 0; i < teams.size(); i++) {
		bestScores[teams[i]] = std::make_pair(warnerScores[i], std::vector<std::string>());
	}
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
std::map<std::string, std::pair<long, std::vector<std::string>>> Solver::getSolution(const std::vector<Team::Team*>& teams, const std::map<std::string, Team::Team*>& teamsMap) {
	bool changed = true;
	std::map<std::string, std::pair<long, std::vector<std::string>>> results;
	while (changed) {
		changed = false;
		for (auto& t : teams) {
			changed |= t->updateCurBest(teamsMap);
		}
	}
	for (const auto& t : teams) {
		results[t->metadata.name] = std::make_pair(t->getCurBest(), t->getListBestPath());
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
			team->setListBestPath({});
		}
		else {
			team->setListBestPath({"Warner Pacific"});
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
std::vector<std::string> Solver::getPrimesList(int numTeams, int numGenerated) {
	if (numGenerated == -1) numGenerated = 2800; //Just a starting point
	std::vector<std::string> primes;
	bool* isPrime = new bool[numGenerated];
	isPrime[0] = isPrime[1] = false;//0 and 1 aren't prime
	for (int i = 2; i < numGenerated; i++) isPrime[i] = true;//Initialize all are true
	for (int i = 2; i < numGenerated; i++) {
		if (isPrime[i]) {
			primes.push_back(std::to_string(i));
			//Set multiples of i to not prime
			for (int j = i; j < numGenerated; j += i) {
				isPrime[j] = false;
			}
		}
	}
	if (primes.size() >= numTeams) return primes;
	//Just naively increase number of generated primes until enough are generated
	return getPrimesList(numTeams, numGenerated * 2);
}

/**
* Function to initialize the team metadata
*/
void Solver::initializeTeamMetadata() {
	//Read in game results
	rapidcsv::Document games("Sports_Data_Scraper/games.csv");
	auto teamNames = games.GetColumn<std::string>("team");
	auto opponentNames = games.GetColumn<std::string>("opponent");
	auto teamPoints = games.GetColumn<int>("points");
	auto opponentPoints = games.GetColumn<int>("opp_points");
	
	//Generate Primes
	auto primes = Solver::getPrimesList(500);

	std::cout << "GOT PRIMES" << std::endl;

	auto solverData = SolverMetadata::getInstance();
	std::map<std::string, int> teamNamesSeen;// Which teams we've already seen

	//Warner Pacific is team 1
	solverData->teamData.push_back(Team::TeamMetaData("Warner Pacific", "1"));
	teamNamesSeen["Warner Pacific"] = 0;

	//Process game results
	while (!teamNames.empty()) {
		//Result of game
		std::string team = teamNames.back();
		std::string opponent = opponentNames.back();
		int teamScore = teamPoints.back();
		int opponentScore = opponentPoints.back();
		teamNames.pop_back();
		opponentNames.pop_back();
		teamPoints.pop_back();
		opponentPoints.pop_back();

		if (team == "") continue;

		//Add new team metadatas if needed
		if (teamNamesSeen.find(team) == teamNamesSeen.end()) {
			teamNamesSeen[team] = solverData->teamData.size();
			solverData->teamData.push_back(Team::TeamMetaData(team, primes.back()));
			primes.pop_back();
		}
		//std::cout << "Added teams " << teamNamesSeen.size() << std::endl;
		if (teamNamesSeen.find(opponent) == teamNamesSeen.end()) {
			teamNamesSeen[opponent] = solverData->teamData.size();
			solverData->teamData.push_back(Team::TeamMetaData(opponent, primes.back()));
			primes.pop_back();
		}
		int teamIndex = teamNamesSeen[team];
		int opponentIndex = teamNamesSeen[opponent];

		//Add result of game
		solverData->teamData[teamIndex].addResult(opponent, teamScore - opponentScore);
		solverData->teamData[opponentIndex].addResult(team, opponentScore - teamScore);
	}
	std::cout << teamNamesSeen.size() << std::endl;
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
		std::cout << "Received Results " << changed <<  std::endl;
		
		if (changed) {
			std::ofstream scoreFile;
			scoreFile.open("warner_score.csv", std::ios::out);
			scoreFile << "team,warner_score,path" << std::endl;
			for (const auto& pair : solverData->bestScores) {
				std::string teamsBeat = "";
				for (const auto& team : pair.second.second) {
					teamsBeat += team != *pair.second.second.begin() ? ",'" + team + "'" : "";
				}
				scoreFile << "\"" << pair.first << "\"," << pair.second.first <<  ",\"" << teamsBeat << "\"" << std::endl;
			}
			scoreFile.close();
		}
	}
}