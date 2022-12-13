#pragma once
#include <string>
#include <map>
#include "big_integer.h"
namespace Team {
	class TeamMetaData {
		//Every team that this team beat, and by how much
		std::map <const std::string, int> results;

	public:
		const std::string name;
		//A unique, prime identifier to do fancy loop checking
		const BigInteger uid;

		TeamMetaData(const std::string& n, const std::string& id);

		void addResult(const std::string& opponentName, int difference);

		const std::map<const std::string, int> getResults() const { return results; }
	};

	class Team {
		int curBest;

		BigInteger curBestPath;

	public:
		const TeamMetaData metadata;

		Team(const TeamMetaData& m);

		int getCurBest() const { return curBest; }

		BigInteger getCurBestPath() const { return curBestPath; }

		void setCurBestPath(BigInteger b) { curBestPath = b; }

		bool updateCurBest(const std::map<std::string, Team*>& teams);
	};
}