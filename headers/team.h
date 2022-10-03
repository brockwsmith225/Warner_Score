#pragma once
#include <string>
#include <map>
namespace Team {
	class TeamMetaData {
		//Every team that this team beat, and by how much
		std::map <const std::string, unsigned int> wins;

	public:
		const std::string name;
		//A unique, prime identifier to do fancy loop checking
		const unsigned int uid;

		TeamMetaData(const std::string& n, const unsigned int id);

		void addWin(const std::string& n, unsigned int m);

		const std::map<const std::string, unsigned int> getWins() const { return wins; }
	};

	class Team {
		int curBest;

		unsigned long curBestPath;

	public:
		const TeamMetaData metadata;

		Team(const TeamMetaData& m);

		int getCurBest() const { return curBest; }

		unsigned long getCurBestPath() const { return curBestPath; }

		void setCurBestPath(int b) { curBestPath = b; }

		bool updateCurBest(const std::map<std::string, Team*>& teams);
	};
}