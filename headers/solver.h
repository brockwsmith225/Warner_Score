#include "team.h"
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <condition_variable>

#define NUM_THREADS 8


namespace Solver {
	class SolverMetadata {
		static SolverMetadata* instance;
	public:
		//---------------------Global Variables------------------------
		//Constructor
		SolverMetadata();
		//Queue for found results
		std::vector<std::map<std::string, long>> bestScoresQueue;
		//Current best scores
		std::map<std::string, long> bestScores;
		//Metadata of teams
		std::vector<Team::TeamMetaData> teamData;
		//Singleton getter
		static SolverMetadata* getInstance();
		//----------------------Threading Synchronization--------------
		std::mutex queueAccess; //Lock queue reads and writes
		std::condition_variable queueCV; //Condition to denote queue has been written to

	};
	//----------------------Worker Thread Functions-----------------
	std::map<std::string, long> getSolution(const std::vector<Team::Team*>& teams, const std::map<std::string, Team::Team*>& teamsMap);
	std::vector<Team::Team*> getTeamsList(const std::map<std::string, Team::Team*>& teamsMap);
	std::map<std::string, Team::Team*> initializeTeamsMap(const std::vector<Team::TeamMetaData>& metaData);
	void shuffleTeamsList(std::vector<Team::Team*>& teams);
	void runWorkerThread();
	//----------------------Main Thread Functions-------------------
	//--------Initialization functions---------
	std::vector<std::string> getPrimesList(int numTeams, int numGenerated = -1);
	void initializeTeamMetadata();
	//--------Main Solver----------------------
	void runSolver();

}
