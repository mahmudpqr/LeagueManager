#ifndef LEAGUEMANAGER_H
#define LEAGUEMANAGER_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

// Forward declarations
class Player;
class Club;
class Match;
class League;

// Random generators
extern random_device rd;
extern mt19937 gen;
extern uniform_real_distribution<> randomRating;
extern uniform_int_distribution<> randomScore;

// Player positions
enum Position {
    GK, DEF, MID, FW
};

// Player class

class Player {
    string name;
    Club* clubPtr = nullptr;
    Position position = GK;
    double avgRating = 0;

public:
    Player();
    Player(string, Position, Club*);

    Position getPosition();
    string getPositionString();
    void updateRating(double, int);

    friend void showTopPlayers(League&);
};

// Team class

class Club {
    string name;
    int matchesPlayed = 0, wins = 0, losses = 0, draws = 0, goalsFor = 0, goalsAgainst = 0, goalDifference = 0, points = 0;
    vector<Player*> squad;

public:
    Club();
    Club(string);

    string getName();
    void addPlayer(Player*);
    void updateClubStats(int, int);

    friend void showStandings(League&);
    friend bool compareClubs(Club*, Club*);
};

// Match class

class Match {
    Club *club1 = nullptr, *club2 = nullptr;
    int score1 = 0, score2 = 0;

public:
    static int matchCount;
    Match();
    Match(Club*, Club*, int, int);

    void showMatchResult();

    friend void showMatchHistory(League&);
};

// League class

class League {
    string name;
    vector<Club*> clubs;
    vector<Match*> matches;
    vector<Player*> allPlayers, gks, defs, mids, fws;

public:
    League();
    League(string);
    ~League();

    void addClub(Club*);
    void addMatch(Match*);
    void addPlayer(Player*);

    friend void showMatchHistory(League&);
    friend void showTopPlayers(League&);
    friend void showStandings(League&);
};

// Helper functions
void simulateMatch(Club&, Club&, League&);
void showMatchHistory(League&);
void showTopPlayers(League&);
void showStandings(League&);

#endif