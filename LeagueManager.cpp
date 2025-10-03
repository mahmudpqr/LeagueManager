#include "LeagueManager.h"

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> randomRating(5.0, 9.5);
uniform_int_distribution<> randomScore(0, 4);

vector<League> leagues;
vector<Club> clubs;
vector<Player> players;

Player::Player() {}
Player::Player(string pName, Position pos, Club* cl) : name(pName), position(pos), club(cl) {
    club->addPlayer(this);
}

string Player::getName() {
    return name;
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

Club* Player::getClub() {
    return club;
}

double Player::getRating() {
    return avgRating;
}

void Player::updateRating(double rating, int matchesPlayed) {
    if (matchesPlayed == 0) return;
    avgRating = ((avgRating * (matchesPlayed - 1)) + rating) / (double)matchesPlayed;
}

Club::Club() {}
Club::Club(string cName) : name(cName) {}

string Club::getName() {
    return name;
}

void Club::addPlayer(Player* player) {
    squad.push_back(player);
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
        p->updateRating(rating, matchesPlayed);
    }
}

Match::Match() {}
Match::Match(Club* c1, Club* c2) : club1(c1), club2(c2) {}

void Match::showMatchResult() {
    cout << right << setw(20) << club1->getName()
        << setw(5) << score1
        << " - "
        << left << setw(5) << score2
        << setw(20) << club2->getName();
}

Club* Match::getClub1() {
    return club1;
}
Club* Match::getClub2() {
    return club2;
}

void Match::updateScores(int s1, int s2) {
    score1 = s1;
    score2 = s2;
}

League::League() {}
League::League(string lName) : name(lName) {}
League::~League() {
    for (Match* m : matches) delete m;
}

string League::getName() {
    return name;
}

void League::addClub(Club* club) {
    clubs.push_back(club);
}
void League::addMatch(Match* match) {
    matches.push_back(match);
}

void League::addPlayer(Player* player) {
    allPlayers.push_back(player);

    switch(player->getPosition()) {
    case GK:
        gks.push_back(player);
        break;
    case DEF:
        defs.push_back(player);
        break;
    case MID:
        mids.push_back(player);
        break;
    case FW:
        fws.push_back(player);
        break;
    }
}

void generateFixture(League& league) {
    for( int j = 0; j < league.clubs.size(); j++) {
        for( int k = 0; k < league.clubs.size(); k++) {
            if (j != k) {
                Match* m = new Match(league.clubs[j], league.clubs[k]);
                league.addMatch(m);
            }
        }
    }
}

void enterMatch(League& league, bool showResult = true) {
    if (league.nextMatchIndex >= league.matches.size()) {
        cout << "\n\tSeason completed\n";
        return;
    }
    
    Match* match =league.matches[league.nextMatchIndex];
    league.nextMatchIndex++;

    int s1 = randomScore(gen);
    int s2 = randomScore(gen);

    match->updateScores(s1, s2);
    match->getClub1()->updateClubStats(s1, s2);
    match->getClub2()->updateClubStats(s2, s1);

    if (showResult) {
        showHeader("Full time");
        cout << "\n\t" << string(13, ' ');
        match->showMatchResult();
        cout << "\n";
    }
}

void completeSeason(League& league) {
    if (league.nextMatchIndex >= league.matches.size()) {
        cout << "\n\tSeason completed\n";
        return;
    }

    while (league.nextMatchIndex < league.matches.size()) {
        enterMatch(league, false);
    }

    cout << "\n\tSeason completed\n";
}

void showMatchHistory(League& league) {
    int matchCount = league.nextMatchIndex;
    showHeader("Match History");

    if(matchCount == 0) {
        cout << "\n\tNo matches yet\n";
        return;
    }

    for (int i = 0; i < matchCount; i++) {
        cout << "\n\tMatch " << setw(7) << i + 1;
        league.matches[i]->showMatchResult();
    }

    cout << "\n";
}

bool compareClubs(Club* a, Club* b) {
    if (a->points != b->points) {
        return a->points > b->points;
    }

    if (a->goalDifference != b->goalDifference) {
        return a->goalDifference > b->goalDifference;
    }

    if (a->goalsFor != b->goalsFor) {
        return a->goalsFor > b->goalsFor;
    }

    return a->name < b->name;
}

void showStandings(League& league) {
    vector<Club*> clubsRanked = league.clubs;
    sort(clubsRanked.begin(), clubsRanked.end(), compareClubs);

    showHeader("Standings");
    cout << "\n\t" << left
        << setw(5) << "Pos"
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
    for (Club* c : clubsRanked) {
        cout << "\t" << left
            << setw(5) << pos
            << setw(20) << c->name
            << setw(5) << c->matchesPlayed
            << setw(5) << c->wins
            << setw(5) << c->draws
            << setw(5) << c->losses
            << setw(5) << c->goalsFor
            << setw(5) << c->goalsAgainst
            << setw(5) << c->goalDifference
            << setw(5) << c->points << "\n";
        pos++;
    }
}

void showTopPlayers(League& league) {
    Player *bestPlayer = league.allPlayers[0], *bestGK = league.gks[0],
        *bestDEF = league.defs[0], *bestMID = league.mids[0], *bestFW = league.fws[0];
    double maxPlayer = 0.0, maxGK = 0.0, maxDEF = 0.0, maxMID = 0.0, maxFW = 0.0;

    for (Player* p : league.allPlayers) {
        if (p->getRating() > maxPlayer) {
            maxPlayer = p->getRating();
            bestPlayer = p;
        }

        if (p->getPosition() == GK && p->getRating() > maxGK) {
            maxGK = p->getRating();
            bestGK = p;
        } else if (p->getPosition() == DEF && p->getRating() > maxDEF) {
            maxDEF = p->getRating();
            bestDEF = p;
        } else if (p->getPosition() == MID && p->getRating() > maxMID) {
            maxMID = p->getRating();
            bestMID = p;
        } else if (p->getPosition() == FW && p->getRating() > maxFW) {
            maxFW = p->getRating();
            bestFW = p;
        }
    }

    cout << fixed << setprecision(1);
    showHeader("Top Players");

    cout << "\n\t" << left
        << setw(25) << "Category"
        << setw(5) << "Pos"
        << setw(25) << "Name"
        << setw(5) << "Avg"
        << setw(20) << "Club" << "\n\n";

    cout << "\t" << left << setw(25) << "Player of the Season"
        << setw(5) << bestPlayer->getPositionString()
        << setw(25) << bestPlayer->getName()
        << setw(5) << bestPlayer->getRating()
        << setw(20) << bestPlayer->getClub()->getName() << "\n";

    cout << "\t" << left << setw(25) << "Best Goalkeeper"
        << setw(5) << bestGK->getPositionString()
        << setw(25) << bestGK->getName()
        << setw(5) << bestGK->getRating()
        << setw(20) << bestGK->getClub()->getName() << "\n";

    cout << "\t" << left << setw(25) << "Best Defender"
        << setw(5) << bestDEF->getPositionString()
        << setw(25) << bestDEF->getName()
        << setw(5) << bestDEF->getRating()
        << setw(20) << bestDEF->getClub()->getName() << "\n";

    cout << "\t" << left << setw(25) << "Best Midfielder"
        << setw(5) << bestMID->getPositionString()
        << setw(25) << bestMID->getName()
        << setw(5) << bestMID->getRating()
        << setw(20) << bestMID->getClub()->getName() << "\n";

    cout << "\t" << left << setw(25) << "Best Forward"
        << setw(5) << bestFW->getPositionString()
        << setw(25) << bestFW->getName()
        << setw(5) << bestFW->getRating()
        << setw(20) << bestFW->getClub()->getName() << "\n";
}

int createLeague() {
    cin.ignore();
    string lName;

    while (true) {
        cout << "\n\tLeague name: ";
        getline(cin, lName);

        if (lName.empty() || lName.size() > 67) {
            cerr << "\tInvalid name!\n";
        } else break;
    }

    leagues.emplace_back(lName);

    int cNum;
    while (true) {
        cout << "\n\tNumber of clubs (2-20): ";
        readInt(cNum);

        if (cNum < 2 || cNum > 20) {
            cerr << "\tInvalid number!\n";
        } else break;
    }

    return cNum;
}

void registerClubs(int cNum, ifstream& pFile) {
    vector<Position> formation = {GK, DEF, DEF, DEF, DEF, MID, MID, MID, FW, FW, FW};

    cin.ignore();

    for (int i = 0; i < cNum; i++) {
        string cName;

        while (true) {
            cout << "\n\tClub " << i + 1 << ": ";
            getline(cin, cName);
            
            if (cName.empty() || cName.size() > 18) {
                cerr << "\tInvalid name!\n";
            } else break;
        }

        clubs.emplace_back(cName);
        leagues.back().addClub(&clubs.back());
                        
        for (int j = 0; j < 11; j++) {
            string pName;
            getline(pFile, pName);

            players.emplace_back(pName, formation[j], &clubs.back());
            leagues.back().addPlayer(&players.back());
        }
    }

    cout << "\n\tRegistration complete\n";
}

void registerClubs(int cNum, ifstream& cFile, istream& pFile) {
    vector<Position> formation = {GK, DEF, DEF, DEF, DEF, MID, MID, MID, FW, FW, FW};

    for (int i = 0; i < cNum; i++) {
        string cName;
        getline(cFile, cName);

        clubs.emplace_back(cName);
        leagues.back().addClub(&clubs.back());
                        
        for (int j = 0; j < 11; j++) {
            string pName;
            getline(pFile, pName);

            players.emplace_back(pName, formation[j], &clubs.back());
            leagues.back().addPlayer(&players.back());
        }
    }

    cout << "\n\tRegistration complete\n";
}

void readInt (int& n) {
    if (!(cin >> n)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        n = -1;
    }
}

int countNames (ifstream& pcFile) {
    int totalNames = 0;
    string temp;

    while (getline(pcFile, temp)) {
        totalNames++;
    }

    pcFile.clear();
    pcFile.seekg(0, ios::beg);

    return totalNames;
}

void showHeader(string title) {
    int padding = 78 - title.size();
    int lp = padding / 2;
    int rp = padding - lp;

    cout << "\n\t" << string(lp, '=') << " " << title << " " << string(rp, '=') << "\n";
}

void showFooter() {
    cout << "\n\t" << string(80, '-') << "\n";
}

int main() {
    showHeader("League Manager");
    int lNum = 0;

    leagues.reserve(10);
    clubs.reserve(200);
    players.reserve(2200);

    while (true) {
        showHeader("Main Menu");
        cout << "\n\t1. New league"
            << "\n\t2. Load league"
            << "\n\t3. Exit\n";

        showFooter();
        cout << "\n\tChoose option: ";
        
        int x;
        readInt(x);

        if (x == 1) {
            if (lNum >= 10) {
                cout << "\n\tMax limit reached\n";
                continue;
            }
            int cNum = createLeague();

            while (true) {
                showHeader("New League");
                cout << "\n\t1. Register clubs"
                    << "\n\t2. Auto register"
                    << "\n\t3. Back\n";

                showFooter();
                cout << "\n\tChoose option: ";
                
                int y;
                readInt(y);

                if (y == 1) {
                    ifstream pFile("Players.txt");
                    int pNames = countNames(pFile);

                    if (!pFile || pNames < cNum * 11) {
                        cerr << "\n\tPlayers missing\n";
                        continue;
                    }
    
                    registerClubs(cNum, pFile);
                    generateFixture(leagues.back());

                    lNum++;
                    break;
                } else if (y == 2) {
                    ifstream cFile("Clubs.txt");
                    ifstream pFile("Players.txt");

                    int cNames = countNames(cFile);
                    int pNames = countNames(pFile);

                    if (!cFile || cNames < cNum) {
                        cerr << "\n\tClubs missing\n";
                        continue;
                    }
                    
                    if (!pFile || pNames < cNum * 11) {
                        cerr << "\n\tPlayers missing\n";
                        continue;
                    }

                    registerClubs(cNum, cFile, pFile);
                    generateFixture(leagues.back());

                    lNum++;
                    break;
                } else if (y == 3) {
                    leagues.pop_back();
                    break;
                } else {
                    cerr << "\n\tInvalid choice!\n";
                }
            }
        } else if (x == 2) {
            while (true) {
                showHeader("Load League");
                if (leagues.empty()) {
                    cerr << "\n\tNo leagues found\n";
                    break;
                }

                int i = 1;
                for (League& l : leagues) {
                    cout << "\n\t" << i << ". " << l.getName();
                    i++;
                }

                cout << "\n\t" << i << ". " << "Back\n";
                showFooter();
                cout << "\n\tChoose option: ";
                
                int y;
                readInt(y);

                if (y >= 1 && y < i ) {
                    League& l = leagues[y - 1];

                    while (true) {
                        showHeader(l.getName());
                        cout << "\n\t1. Enter match"
                            << "\n\t2. Complete season"
                            << "\n\t3. Match history"
                            << "\n\t4. Standings"
                            << "\n\t5. Top players"
                            << "\n\t6. Back\n";

                        showFooter();
                        cout << "\n\tChoose option: ";

                        int z;
                        readInt(z);

                        if (z == 1) {
                            enterMatch(l);
                        } else if (z == 2) {
                            completeSeason(l);
                        } else if (z == 3) {
                            showMatchHistory(l);
                        } else if (z == 4) {
                            showStandings(l);
                        } else if (z == 5) {
                            showTopPlayers (l);
                        } else if (z == 6) {
                            break;
                        } else {
                            cerr << "\n\tInvalid choice!\n";
                        }
                    }
                } else if (y == i) {
                    break;
                } else {
                    cerr << "\n\tInvalid choice!\n";
                }
            }
        } else if (x == 3) {
            break;
        } else {
            cerr << "\n\tInvalid choice!\n";
        }
    }

    showHeader("League Manager");
    cout << "\n";
    return 0;
}