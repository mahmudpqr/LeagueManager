#include "LeagueManager.h"

// Random Generators
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> randomRating(5.0, 9.5);
uniform_int_distribution<> randomScore(0, 4);

//  Match Static Variable
int Match::matchCount = 0;

// Player Methods

Player::Player() {}
Player::Player(string playerName, Position pos, Club* t) : name(playerName), position(pos), clubPtr(t) {
    clubPtr->addPlayer(this);
}

Position Player::getPosition() {
    return position;
}

string Player::getPositionString() {
    switch(position) {
    case GK:
        return "GK";
    case DEF:
        return "DEF";
    case MID:
        return "MID";
    case FW:
        return "FW";
    default:
        return "";
    }
}

void Player::updateRating(double rating, int matchNumber) {
    if (matchNumber == 0) return;
    avgRating = ((avgRating * (matchNumber - 1)) + rating) / (double)matchNumber;
}

// Club Methods

Club::Club() {}
Club::Club(string clubName) : name(clubName) {}

string Club::getName() {
    return name;
}

void Club::addPlayer(Player* playerPtr) {
    squad.push_back(playerPtr);
}

void Club::updateClubStats(int scored, int conceded) {
    matchesPlayed++;
    goalsFor += scored;
    goalsAgainst += conceded;
    goalDifference += scored - conceded;

    if (scored > conceded) {
        wins++;
        points += 3;
    } else if (scored == conceded) {
        draws++;
        points++;
    } else {
        losses++;
    }

    for (Player* p : squad) {
        double rating = randomRating(gen);
        int matchNumber = Match::matchCount;
        p->updateRating(rating, matchNumber);
    }
}

// Match Methods

Match::Match() {}
Match::Match(Club* t1, Club* t2, int s1, int s2) : club1(t1), club2(t2), score1(s1), score2(s2) {}

void Match::showMatchResult() {
    cout << left << setw(20) << club1->getName()
        << setw(3) << score1
        << " - "
        << setw(3) << score2
        << setw(20) << club2->getName();
}

// League Methods

League::League() {}
League::League(string leagueName) : name(leagueName) {}
League::~League() {
    for (Match* m : matches) delete m;
}

void League::addClub(Club* clubPtr) {
    clubs.push_back(clubPtr);
}
void League::addMatch(Match* matchPtr) {
    matches.push_back(matchPtr);
}

void League::addPlayer(Player* playerPtr) {
    allPlayers.push_back(playerPtr);

    switch(playerPtr->getPosition()) {
    case GK:
        gks.push_back(playerPtr);
        break;
    case DEF:
        defs.push_back(playerPtr);
        break;
    case MID:
        mids.push_back(playerPtr);
        break;
    case FW:
        fws.push_back(playerPtr);
        break;
    }
}

// Helper Functions

void simulateMatch(Club& club1, Club& club2, League& league) {
    int matchNumber = ++Match::matchCount;

    int score1 = randomScore(gen);
    int score2 = randomScore(gen);

    club1.updateClubStats(score1, score2);
    club2.updateClubStats(score2, score1);

    Match* newMatch = new Match(&club1, &club2, score1, score2);
    league.addMatch(newMatch);

    cout << "\n===== Simulating Match ... =====\n\n";
    cout << "Match " << matchNumber << " Results: ";
    newMatch->showMatchResult();
    cout << "\n";
}

void showMatchHistory(League& league) {
    cout << "\n===== Match History =====\n\n";
    if(league.matches.empty()) {
        cout << "\nNo Matches Yet!";
        return;
    }

    int i = 0;
    for (Match* m : league.matches) {
        cout << "Match " << i + 1 << ": ";
        m->showMatchResult();
        cout << "\n";
        i++;
    }
}

void showTopPlayers(League& league) {
    Player *bestPlayer = league.allPlayers[0], *bestGK = league.gks[0], *bestDEF = league.defs[0],
        *bestMID = league.mids[0], *bestFW = league.fws[0];
    double maxPlayer = 0.0, maxGK = 0.0, maxDEF = 0.0, maxMID = 0.0, maxFW = 0.0;

    for (Player* p : league.allPlayers) {
        if (p->avgRating > maxPlayer) {
            maxPlayer = p->avgRating;
            bestPlayer = p;
        }

        if (p->position == GK && p->avgRating > maxGK) {
            maxGK = p->avgRating;
            bestGK = p;
        } else if (p->position == DEF && p->avgRating > maxDEF) {
            maxDEF = p->avgRating;
            bestDEF = p;
        } else if (p->position == MID && p->avgRating > maxMID) {
            maxMID = p->avgRating;
            bestMID = p;
        } else if (p->position == FW && p->avgRating > maxFW) {
            maxFW = p->avgRating;
            bestFW = p;
        }
    }

    cout << fixed << setprecision(1);

    cout << "\n===== Top Players =====\n\n";
    cout << left << setw(25) << "Category"
        << setw(10) << "Pos"
        << setw(20) << "Name"
        << setw(10) << "Rating"
        << setw(20) << "Club" << "\n\n";

    cout << left << setw(25) << "Player of the Season"
        << setw(10) << bestPlayer->getPositionString()
        << setw(20) << bestPlayer->name
        << setw(10) << bestPlayer->avgRating
        << setw(20) << bestPlayer->clubPtr->getName() << "\n";

    cout << left << setw(25) << "Best Goalkeeper"
        << setw(10) << bestGK->getPositionString()
        << setw(20) << bestGK->name
        << setw(10) << bestGK->avgRating
        << setw(20) << bestGK->clubPtr->getName() << "\n";

    cout << left << setw(25) << "Best Defender"
        << setw(10) << bestDEF->getPositionString()
        << setw(20) << bestDEF->name
        << setw(10) << bestDEF->avgRating
        << setw(20) << bestDEF->clubPtr->getName() << "\n";

    cout << left << setw(25) << "Best Midfielder"
        << setw(10) << bestMID->getPositionString()
        << setw(20) << bestMID->name
        << setw(10) << bestMID->avgRating
        << setw(20) << bestMID->clubPtr->getName() << "\n";

    cout << left << setw(25) << "Best Forward"
        << setw(10) << bestFW->getPositionString()
        << setw(20) << bestFW->name
        << setw(10) << bestFW->avgRating
        << setw(20) << bestFW->clubPtr->getName() << "\n";

}

bool compareClubs(Club* a, Club* b) {
    if (a->points != b->points)
        return a->points > b->points;
    if (a->goalDifference != b->goalDifference)
        return a->goalDifference > b->goalDifference;
    if (a->goalsFor != b->goalsFor)
        return a->goalsFor > b->goalsFor;
    return a->name < b->name;
}

void showStandings(League& league) {
    vector<Club*> clubsRanked = league.clubs;
    sort(clubsRanked.begin(), clubsRanked.end(), compareClubs);

        cout << "\n===== Standings =====\n\n";
    cout << left << setw(4)  << "Pos"
        << setw(20) << "Club"
        << setw(5) << "MP"
        << setw(5) << "W"
        << setw(5) << "D"
        << setw(5) << "L"
        << setw(5) << "GF"
        << setw(5) << "GA"
        << setw(5) << "GD"
        << setw(5) << "Pts" << "\n\n";

    int pos = 1;
    for (Club* t : clubsRanked) {
        cout << left << setw(4)  << pos
            << setw(20) << t->getName()
            << setw(5) << t->matchesPlayed
            << setw(5) << t->wins
            << setw(5) << t->draws
            << setw(5) << t->losses
            << setw(5) << t->goalsFor
            << setw(5) << t->goalsAgainst
            << setw(5) << t->goalDifference
            << setw(5) << t->points << "\n";
        pos++;
    }
}