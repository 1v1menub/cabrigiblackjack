#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <cstdlib>
#include<limits>

using namespace std;

class Entity;

struct Hand {
    Entity* entity;
    int amount;
    vector<string> cards;

    Hand(Entity* e) {
        entity = e;
    }

    Hand(Entity* e, int a) {
        entity = e;
        amount = a;
    }

    Hand(Hand* ph) {
        entity = ph->entity;
        amount = ph->amount;
        cards.push_back(ph->cards[1]);
    }

    int ReturnTotal() {
        int result = 0;
        bool push = false;
        vector<string> ccards = cards;
        for(int i = 0; i < ccards.size(); i++) {
            if(((int)ccards[i][0] -'0') < 10) {
                result += (int)ccards[i][0] -'0';
            }
            else if(ccards[i][0] == 'T' || ccards[i][0] == 'J' || ccards[i][0] == 'Q' || ccards[i][0] == 'K') {
                result += 10;
            }
            else {
                if(push) {
                    if(result <= 10) {
                        result += 11;
                    }
                    else {
                        result += 1;
                    }
                }
                else {
                    ccards.push_back(ccards[i]);
                    push = true;
                }
            }
        }
        return result;
    }

    int GetAmount() {
        return amount;
    }

    bool SameValue() {
        if(cards[0] == cards[1]) {
            return true;
        }
        else if(cards[0][0] == 'T' || cards[0][0] == 'J' || cards[0][0] == 'Q' || cards[0][0] == 'K') {
            if(cards[1][0] == 'T' || cards[1][0] == 'J' || cards[1][0] == 'Q' || cards[1][0] == 'K') {
                return true;
            }
        }
        return false;
    }
};

class Entity {
public:
    stack<Hand*> hands;
    string name;
    int balance;
    bool liveplay;
    
    void hit(string card) {
        hands.top()->cards.push_back(card);
    }

    string GetName() {
        return name;
    }

    bool IsBust() {
        if(hands.top()->ReturnTotal() > 21) {
            return true;
        }
        else {
            return false;
        }
    }

    bool CloseHand() {
        if(IsBust()) {
            hands.pop();
            return false;
        }
        else {
            hands.pop();
            return true;
        }
    }


    Hand* Stand() {
        Hand* temp = hands.top();
        cout << name << " stands with ";
        ShowTotal();
        CloseHand();
        return temp;
    }

    void ShowHand() {
        for(int i = 0; i < hands.top()->cards.size(); i++) {
            cout << hands.top()->cards[i] << " ";
        }
        cout << endl;
    }

    void ShowTotal() {
        cout << "Total: " << hands.top()->ReturnTotal() << endl;
    }

    int GetHandAmount() {
        return hands.top()->GetAmount();
    }

    int GetHandSize() {
        return hands.top()->cards.size();
    }

    virtual string IsDealt() = 0;
};

class Dealer : public Entity {
public:
    Dealer() {
        name = "Dealer";
    }

    void join() {
        Hand* h = new Hand(this);
        hands.push(h);
    }

    string IsDealt() {
        return "to the dealer";
    }

    void ShowFDHand() {
        cout << hands.top()->cards[0] << " []  ";
    }
};

class Player : public Entity{
private:
    bool livesplit;
    bool livedouble;

public:
    Player(string n) {
        name = n;
        balance = 1000;
    }

    bool join() {
        int amount;
        if(balance == 0) {
            cout << name << " has no chips and will be removed from the table" << endl;
            cout << "----------------------------------------------------------------------------" << endl;
            return false;
        }
        do {
            cout << name << "'s current balance: " << balance  << endl;
            cout << name << " please enter valid bet amount: ";
            while(!(cin >> amount)) {
                cout << "Invalid input, try again: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        } while(amount > balance || amount <= 0);
        balance -= amount;
        Hand* h = new Hand(this, amount);
        hands.push(h);
        cout << "----------------------------------------------------------------------------" << endl;
        return true;
    }

    string IsDealt() {
        string x = "to the player " + name;
        return x;
    }

    void CalcLiveDouble() {
        if((hands.top()->amount) <= balance) {
            if(hands.top()->cards.size() == 2) {
                livedouble = true;
                return;
            }
        }
        livedouble = false;
    }

    void CalcLiveSplit() {
        if(hands.top()->amount <= balance) {
            if(hands.top()->cards.size() == 2) {
                if(hands.top()->SameValue()) {
                    livesplit = true;
                    return;
                }
            }
        }
        livesplit = false;
    }

    bool getLiveDouble() {
        return livedouble;
    }

    bool getLiveSplit() {
        return livesplit;
    }

    void Surrender() {
        int change = hands.top()->GetAmount();
        cout << name << " surrenders " << change << endl;
        balance += (change / 2);
        CloseHand();
    }

    bool HasBlackjack() {
        if(hands.top()->ReturnTotal() == 21) {
            balance += hands.top()->GetAmount() * 2.5;
            cout << name << " has a natural blackjack and wins " << hands.top()->GetAmount() * 1.5 << endl;
            CloseHand();
            return true;
        }
        else {
            return false;
        }
    }

    bool DoubleBet() {
        if(livedouble) {
            balance -= hands.top()->amount;
            hands.top()->amount = hands.top()->amount * 2;
            return true;
        }
        return false;
    }

    bool Split() {
        if(livesplit) {
            balance -= hands.top()->amount;
            Hand* temp = new Hand(hands.top());
            hands.top()->cards.pop_back();
            hands.push(temp);
            if(temp->ReturnTotal() == 11) {
                cout << name << " splits As" << endl;
            }
            else {
                cout << name << " splits " << temp->ReturnTotal() << endl;
            }
            return true;
        }
        return false;
    }
};


class Game {
private:
    vector<Player*> players;
    vector<string> deck;
    Dealer* dealer;
    queue<Hand*> closedHands;
    int runningCount;
    int trueCount;
    int dealtCards;

public:

    Game() {
        runningCount = 0;
        trueCount = 0;
        dealtCards = 0;
        dealer = new Dealer();
    }

    bool PlayRound() {
        cout << "Placing bets" << endl;
        dealer->join();
        vector<int> RemovedPlayers;
        for(int i = 0; i < players.size(); i++) {
            if(!players[i]->join()) {
                RemovedPlayers.push_back(i);
            }
        }
        int c = 0;
        for(int i = 0; i < RemovedPlayers.size(); i++) {
            players.erase(players.begin() + (RemovedPlayers[i] - c));
            c++;
        }
        if(players.size() == 0) {
            cout << "There are no players at the table so the game cannot continue" << endl;
            cout << "----------------------------------------------------------------------------" << endl;
            return false;
        }
        cout << "The round has begun" << endl;
        Shuffle();
        for(int i = 0; i < players.size(); i++) {
            Deal(players[i]);
        }
        Deal(dealer);
        for(int i = 0; i < players.size(); i++) {
            Deal(players[i]);
        }
        DealFD(dealer);
        cout << "----------------------------------------------------------------------------" << endl;
        for(int i = 0; i < players.size(); i++) {
            if(players[i]->HasBlackjack()) {
                cout << "----------------------------------------------------------------------------" << endl;
            }
        }
        for(int i = 0; i < players.size(); i++) {
            while(!(players[i]->hands.empty())) {
                if(players[i]->GetHandSize() == 1) {
                    Deal(players[i]);
                }
                players[i]->CalcLiveDouble();
                players[i]->CalcLiveSplit();
                cout << players[i]->GetName() << " to play: ";
                players[i]->ShowHand();
                players[i]->ShowTotal();
                cout << "Dealers Hand: ";
                dealer->ShowFDHand();
                cout << endl << "Press [0] to hit" << "  " << "Press [1] to stand" << "  " << "Press [2] to surrender" << "  ";
                if(players[i]->getLiveDouble()) {
                    cout << "Press [3] to double" << "  ";
                }
                if(players[i]->getLiveSplit()) {
                    cout << "Press [4] to split";
                }
                cout << endl <<"Press [9] to see counts" << endl;
                static int a;
                cin >> a;
                switch (a) {
                    case 0:
                        Deal(players[i]);
                        if(players[i]->IsBust()) {
                            cout << "Hand busts" << endl << players[i]->name << " loses " << players[i]->GetHandAmount() << endl;
                            players[i]->CloseHand();
                        }
                        break;
                    case 1:
                        closedHands.push(players[i]->Stand());
                        break;
                    case 2:
                        players[i]->Surrender();
                        break; 
                    case 3:
                        if(players[i]->DoubleBet()) {
                            cout << players[i]->name << " doubles down" << endl;
                            Deal(players[i]);
                            if(!players[i]->IsBust()) {
                                closedHands.push(players[i]->Stand());
                            }
                            else {
                                cout << "Hand busts" << endl << players[i]->name << " loses " << players[i]->GetHandAmount() << endl;
                                players[i]->CloseHand();
                            }
                        }
                        break;
                    case 4:
                        players[i]->Split();
                        break;
                    case 9:
                        PrintRunning();
                        PrintTrue();
                    default:
                        cout << "Enter valid command" << endl;
                }
                cout << "----------------------------------------------------------------------------" << endl;
            }
        }
        cout << "Facedown card was a: " << dealer->hands.top()->cards[1] << endl;
        dealer->ShowTotal();
        if(((int)dealer->hands.top()->cards[1][0] - '0') <= 6) {
            runningCount++;
        }
        else if(dealer->hands.top()->cards[1][0] == 'T' || dealer->hands.top()->cards[1][0] == 'J' || dealer->hands.top()->cards[1][0] == 'Q' || dealer->hands.top()->cards[1][0] == 'K' || dealer->hands.top()->cards[1][0] == 'A') {
            runningCount--;
        }
        dealtCards++;
        trueCount =  runningCount / ((312 - dealtCards) / 52);
        if(dealer->hands.top()->ReturnTotal() == 21) {
            cout << "Dealer natural blackjack: All active players lose" << endl;
            cout << "----------------------------------------------------------------------------" << endl;
            while(!closedHands.empty()) {
                closedHands.pop();
            }
            return true;
        }
        while(dealer->hands.top()->ReturnTotal() < 17) {
            Deal(dealer);
            dealer->ShowTotal();
            if(dealer->hands.top()->ReturnTotal() > 21) {
                cout << "Dealer busts" << endl;
                int n = closedHands.size();
                for(int i = 0; i < n; i++) {
                    closedHands.front()->entity->balance += (closedHands.front()->amount * 2);
                    cout << "Player " << closedHands.front()->entity->name << " wins " << closedHands.front()->GetAmount() << endl;
                    closedHands.pop();
                }
                cout << "----------------------------------------------------------------------------" << endl;
                return true;
            }
        };
        cout << "Dealer stands with ";
        dealer->ShowTotal();
        int n = closedHands.size();
        for(int i = 0; i < n; i++) {
            if(closedHands.front()->ReturnTotal() > dealer->hands.top()->ReturnTotal()) {
                closedHands.front()->entity->balance += closedHands.front()->amount * 2;
                cout << closedHands.front()->entity->name << " wins " << closedHands.front()->GetAmount() << endl;
            }
            else if(closedHands.front()->ReturnTotal() == dealer->hands.top()->ReturnTotal()) {
                closedHands.front()->entity->balance += closedHands.front()->amount;
                cout << closedHands.front()->entity->name << " is pushed " << closedHands.front()->GetAmount() << endl;
            }
            else {
                cout << closedHands.front()->entity->name << " loses " << closedHands.front()->GetAmount() << endl;
            }
            closedHands.pop();
        }
        cout << "----------------------------------------------------------------------------" << endl;
        return true;
    } 

    void RegisterPlayer() {
        cout << "Enter player name: ";
        string name;
        cin >> name;
        Player* p = new Player(name);
        players.push_back(p);
        cout << "Player " << p->GetName() << " successfully registered" << endl << "----------------------------------------------------------------------------" << endl;
    }

    void Shuffle() {
        if(deck.size() <= 52) {
            dealtCards = 0;
            runningCount = 0;
            trueCount = 0;
            deck = {"AS", "2S", "3S", "4S", "5S", "6S", "7S", "8S", "9S", "TS", "JS", "QS", "KS", "AD", "2D", "3D", "4D", "5D", "6D", "7D", "8D", "9D", "TD", "JD", "QD", "KD", "KC", "QC", "JC", "TC", "9C", "8C", "7C", "6C", "5C", "4C", "3C", "2C", "AC", "KH", "QH", "JH", "TH", "9H", "8H", "7H", "6H", "5H", "4H", "3H", "2H", "AH", "AS", "2S", "3S", "4S", "5S", "6S", "7S", "8S", "9S", "TS", "JS", "QS", "KS", "AD", "2D", "3D", "4D", "5D", "6D", "7D", "8D", "9D", "TD", "JD", "QD", "KD", "KC", "QC", "JC", "TC", "9C", "8C", "7C", "6C", "5C", "4C", "3C", "2C", "AC", "KH", "QH", "JH", "TH", "9H", "8H", "7H", "6H", "5H", "4H", "3H", "2H", "AH", "AS", "2S", "3S", "4S", "5S", "6S", "7S", "8S", "9S", "TS", "JS", "QS", "KS", "AD", "2D", "3D", "4D", "5D", "6D", "7D", "8D", "9D", "TD", "JD", "QD", "KD", "KC", "QC", "JC", "TC", "9C", "8C", "7C", "6C", "5C", "4C", "3C", "2C", "AC", "KH", "QH", "JH", "TH", "9H", "8H", "7H", "6H", "5H", "4H", "3H", "2H", "AH", "AS", "2S", "3S", "4S", "5S", "6S", "7S", "8S", "9S", "TS", "JS", "QS", "KS", "AD", "2D", "3D", "4D", "5D", "6D", "7D", "8D", "9D", "TD", "JD", "QD", "KD", "KC", "QC", "JC", "TC", "9C", "8C", "7C", "6C", "5C", "4C", "3C", "2C", "AC", "KH", "QH", "JH", "TH", "9H", "8H", "7H", "6H", "5H", "4H", "3H", "2H", "AH", "AS", "2S", "3S", "4S", "5S", "6S", "7S", "8S", "9S", "TS", "JS", "QS", "KS", "AD", "2D", "3D", "4D", "5D", "6D", "7D", "8D", "9D", "TD", "JD", "QD", "KD", "KC", "QC", "JC", "TC", "9C", "8C", "7C", "6C", "5C", "4C", "3C", "2C", "AC", "KH", "QH", "JH", "TH", "9H", "8H", "7H", "6H", "5H", "4H", "3H", "2H", "AH", "AS", "2S", "3S", "4S", "5S", "6S", "7S", "8S", "9S", "TS", "JS", "QS", "KS", "AD", "2D", "3D", "4D", "5D", "6D", "7D", "8D", "9D", "TD", "JD", "QD", "KD", "KC", "QC", "JC", "TC", "9C", "8C", "7C", "6C", "5C", "4C", "3C", "2C", "AC", "KH", "QH", "JH", "TH", "9H", "8H", "7H", "6H", "5H", "4H", "3H", "2H", "AH"};
            srand(time(NULL));
            for(int i = 0; i < 312; i++) {
                int x = rand() % 312;
                swap(deck[i], deck[x]);
            }
            cout << "Deck is being shuffled" << endl;
        }
    }

    string Deal(Entity* p) {
        string card = deck.back();
        deck.pop_back();
        if(((int)card[0] -'0') <= 6) {
            runningCount++;
        }
        else if(card[0] == 'T' || card[0] == 'J' || card[0] == 'Q' || card[0] == 'K' || card[0] == 'A') {
            runningCount--;
        }
        dealtCards++;
        trueCount =  runningCount / ((312 - dealtCards) / 52);
        cout << card << " is dealt " << p->IsDealt() << endl;
        p->hit(card);
        return card;
    }

    string DealFD(Dealer* p) {
        string card = deck.back();
        deck.pop_back();
        cout << "A card is dealt face down to the dealer" << endl;
        p->hit(card);
        return card;
    }

    void PrintRunning() {
        cout << "Running count: " << runningCount << endl;
    }

    void PrintTrue() {
        cout << "True count: " << trueCount << endl;
    }
};

int main() {
    Game g;
    int p = 0;
    while(p <= 0) {
        cout << "Enter valid number of players: ";
        while(!(cin >> p)) {
            cout << "Invalid input, try again: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    for(int i = 0; i < p; i++) {
        g.RegisterPlayer();
    }
    int n = 0;
    while(n <= 0) {
        cout << "Enter valid number of rounds: ";
        while(!(cin >> n)) {
            cout << "Invalid input, try again: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    cout << "----------------------------------------------------------------------------" << endl;
    int i = 0; 
    bool c = true;
    while(i < n && c) {
        c = g.PlayRound();
    }
    string xd;
    cout << "Enter anything to exit: ";
    cin >> xd;
}