#ifndef LEAGUEMANAGER_H
#define LEAGUEMANAGER_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>

using namespace std;

class Player;
class Club;
class Match;
class League;

extern random_device rd;
extern mt19937 gen;
extern uniform_real_distribution<> randomRating;
extern uniform_int_distribution<> randomScore;

enum Position {
    GK, DEF, MID, FW
};

class Player {
    string name;
    Position position = GK;
    Club* club = nullptr;
    double avgRating = 0;

public:
    Player();
    Player(string, Position, Club*);

    string getName();
    Position getPosition();
    string getPositionString();
    Club* getClub();
    double getRating();
    void updateRating(double, int);
};

class Club {
    string name;
    int matchesPlayed = 0, wins = 0, losses = 0, draws = 0,
        goalsFor = 0, goalsAgainst = 0, goalDifference = 0, points = 0;
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

class Match {
    Club *club1 = nullptr, *club2 = nullptr;
    int score1 = 0, score2 = 0;

public:
    Match();
    Match(Club*, Club*);

    Club* getClub1();
    Club* getClub2();
    void updateScores(int, int);
    void showMatchResult();
};

class League {
    string name;
    int nextMatchIndex = 0;
    vector<Club*> clubs;
    vector<Match*> matches;
    vector<Player*> allPlayers, gks, defs, mids, fws;

public:
    League();
    League(string);
    ~League();

    string getName();
    void addClub(Club*);
    void addMatch(Match*);
    void addPlayer(Player*);

    friend void generateFixture(League&);
    friend void enterMatch(League&, bool);
    friend void completeSeason(League&);
    friend void showMatchHistory(League&);
    friend void showStandings(League&);
    friend void showTopPlayers(League&);
};

void generateFixture(League&);
void enterMatch(League&, bool);
void completeSeason(League&);
void showMatchHistory(League&);

bool compareClubs(Club*, Club*);
void showStandings(League&);
void showTopPlayers(League&);

int createLeague();
void registerClubs(int, ifstream&);
void registerClubs(int, ifstream&, istream&);

void readInt (int& n);
int countNames (ifstream&);
void showHeader(string);
void showFooter();

#endif