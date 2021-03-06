//============================================================================
// Name        : blackjackSim.cpp
// Author      : Alex Moreno
// Description : A game which allows one player to play blackjack.
//============================================================================

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // For rand()
#include <ctime>   // For seeding rand with system time
using namespace std;

const int HAND_VALUE_LIMIT = 21;
const int DEALER_RESOLUTION_LIMIT = 17;


//-------------------------------------------------------------------------------------------------
// Stores the value (in blackjack), rank and suit of a card. Allows for printing and initializing
// using an integer from 1 to 52.
class Card {
public:
	// Default constructor for the card class.
	Card() {
		value = 0;
		rank = "NULL";
		suit = "NULL";
	}

	// Takes in an integer from 1 to 52 which determines which card in the deck the Card instance
	// is set as, by changing the value, rank and suit.
	void setTo(int cardNum) {
		// If an invalid cardNum is entered, nothing is initialized.
		if (cardNum >= 1 && cardNum <= 52) {
			// Determines the suit of the card.
			if (cardNum <= 13) {
				suit = "Spades";
			}
			else if (cardNum <= 26) {
				suit = "Hearts";
			}
			else if (cardNum <= 39) {
				suit = "Diamonds";
			}
			else {
				suit = "Clubs";
			}

			// Determines the blackjack value and rank of the card. Aces are said to have value 1.
			switch (cardNum % 13) {
			case 1:
				value = 1;
				rank = "Ace";
				break;
			case 2:
				value = 2;
				rank = "Two";
				break;
			case 3:
				value = 3;
				rank = "Three";
				break;
			case 4:
				value = 4;
				rank = "Four";
				break;
			case 5:
				value = 5;
				rank = "Five";
				break;
			case 6:
				value = 6;
				rank = "Six";
				break;
			case 7:
				value = 7;
				rank = "Seven";
				break;
			case 8:
				value = 8;
				rank = "Eight";
				break;
			case 9:
				value = 9;
				rank = "Nine";
				break;
			case 10:
				value = 10;
				rank = "Ten";
				break;
			case 11:
				value = 10;
				rank = "Jack";
				break;
			case 12:
				value = 10;
				rank = "Queen";
				break;
			case 0:
				value = 10;
				rank = "King";
				break;
			}
		}
	}

	// Getter functions for the value, rank and suit
	int getValue() const {
		return value;
	}
	string getRank() const {
		return rank;
	}
	string getSuit() const {
		return suit;
	}

private:
	int value; // The blackjack value of the card, set to 1 for Aces
	string rank;
	string suit;
};


//---------------------------------------------------------------------------------------------
// Overloads the << operator to allow printing of Cards. Prints as a string describing the card,
// such as "Ace of Spades" or "Ten of Hearts".
ostream& operator<<(ostream& out, const Card& card) {
	return out << card.getRank() << " of " << card.getSuit();
}


//-------------------------------------------------------------------------------------------------
// A shoe is a number of 52 card decks put together into one deck. An instance of this class holds
// all the cards in a shoe, and allows for shuffling of the entire shoe and for dealing cards.
class Shoe {
public:
	// A constructor for the Shoe class. The number of decks in the shoe is passed in as an int.
	// Fills the shoe with the correct number of cards, all in order (not shuffled). If no
	// arguments are present, the default number of decks is one.
	Shoe(int numDecks = 1) {
		numCards = numDecks * 52;
		cards.resize(numCards);
		// Creates all the cards in the shoe in sequential order.
		for (int i = 0; i < numCards; ++i) {
			cards.at(i).setTo(i % 52 + 1); // The "% 52" causes a new deck to be started every 52 cards.
		}
	}

	// Sets the contents of the Shoe. A vector of cards is passed in, and the Shoe's cards vector
	// changes its contents to those of the passed in vector.
	void setCards(vector<Card> newCards) {
		cards = newCards;
		numCards = static_cast<int>(newCards.size());
	}

	// Shuffles all the cards in the shoe.
	void shuffle() {
		int randIdx; // The index in the shoe of the next randomly chosen card.
		Card tempCard; // Facilitates swapping of cards within the shoe.

		// Iterates through all the cards in the shoe. Each iteration starts with the first i cards
		// in the shoe already shuffled, and swaps the card at index i with a random card from the
		// last numCards - i cards in the shoe (which could result in no change since the card
		// could swap with itself), making the first i + 1 cards in the shoe suffled at the end of
		// the iteration. Doesn't do this for the last card because if all of the cards in the shoe
		// have been shuffled except for the last one, then the entire shoe is already shuffled.
		for (int i = 0; i < numCards - 1; ++i) {
			randIdx = rand() % (numCards - i) + i; // Picks a random unshuffled card.

			// Skips the swapping step if the card is going to swap with itself.
			if (randIdx == i) {
				continue;
			}

			// Swaps the cards at i and randIdx.
			tempCard = cards.at(i);
			cards.at(i) = cards.at(randIdx);
			cards.at(randIdx) = tempCard;
		}
	}

	// Returns the card from the end of the shoe and removes that card from the shoe.
	Card dealCard() {
		Card cardToReturn = cards.back();
		cards.pop_back(); // Removes the card from the shoe.
		--numCards; // There's now one less card in the shoe.
		return cardToReturn;
	}

	// Returns the number of cards in the shoe.
	int getNumCards() {
		return numCards;
	}

	//-------------------------------------------------------------------------------------------------
	// If the Shoe isn't empty, does nothing. Otherwise, replenishes the Shoe with the passed in vector
	// of Cards. Then shuffles the Shoe, and prints a message saying that it has done so. Also empties
	// the passed in vector of Cards.
	void refillIfEmpty(vector<Card>& newCards) {
		if (numCards == 0) {
			setCards(newCards);
			numCards = static_cast<int>(newCards.size());
			shuffle();
			cout << "Discarded cards have been reshuffled and put back into the shoe." << endl;
			newCards.clear();
		}
	}

private:
	int numCards;
	vector<Card> cards;
};


//-------------------------------------------------------------------------------------------------
// A hand in blackjack. Can calculate it's value and cards can be added to it.
class BlackjackHand {
public:
	// The default constructor for this class simply initializes the hand's value to 0.
	BlackjackHand() {
		handValue = 0;
	}

	// Adds a card to the hand, calculates the hand's new value and determines if the hand is soft.
	void addCard(Card newCard) {
		cards.push_back(newCard);

		soft = false; // Will be set to true if the hand is soft.
		handValue = 0; // The value of the hand.
		int numAces = 0; // The number of aces in the hand.

		// Sums the values of the cards in the hand and counts the aces.
		for (int i = 0; i < static_cast<int>(cards.size()); ++i) {
			if (cards.at(i).getRank() == "Ace") {
				++numAces;
			}
			handValue += cards.at(i).getValue();
		}

		// Adds 10 to the value for each ace in the hand until doing so would make the value
		// exceed 21.
		for (int i = 0; i < numAces; ++i) {
			if (handValue <= 11) {
				handValue += 10;
				soft = true; // If an ace is being counted as having value 11 the hand is soft.
			}
			else {
				break;
			}
		}
	}

	// Returns the value of the entire hand, counting aces such as to maximize the value without
	// exceeding 21 if possible.
	int value() {
		return handValue;
	}

	// Returns true if the hand is soft.
	bool isSoft() {
		return soft;
	}

	// Returns a vector of the Cards in the hand. Function is const so it can be used in the
	// blackjackHand printing function.
	vector<Card> getCards() const {
		return cards;
	}

private:
	int handValue;
	vector<Card> cards;
	bool soft; // True if the hand has an ace that is being counted as having a value of 11.
};


//---------------------------------------------------------------------------------------------
// Overloads the << operator to allow printing of blackjackHands. Prints each card in the
// hand, separated by commas.
ostream& operator<<(ostream& out, const BlackjackHand& hand) {
	for (int i = 0; i < static_cast<int>(hand.getCards().size()) - 1; ++i) {
		out << hand.getCards().at(i) << ", ";
	}
	// The last card won't be followed by a comma.
	if (hand.getCards().size() > 0) {
		out << hand.getCards().back();
	}

	return out;
}


//-------------------------------------------------------------------------------------------------
// Deals a card from the passed in Shoe to the passed in BlackjackHand. If doing so also makes the
// passed in Shoe empty, refills the Shoe with the passed in vector of Cards, clears that passed in
// vector, and shuffles the Shoe.
void dealCardAndRefillShoeIfEmpty(BlackjackHand& hand, Shoe& shoe, vector<Card>& cardsForRefill) {
	hand.addCard(shoe.dealCard());
	shoe.refillIfEmpty(cardsForRefill); // Automatically shuffles the Shoe if refilled.
}


//-------------------------------------------------------------------------------------------------
// Displays the passed in dealer's hand, the passed in user's hand and the passed in user's bet.
void displayGameState(BlackjackHand dealerHand, BlackjackHand userHand, double bet) {
	cout << "The dealer's hand: " << dealerHand << endl;
	cout << "Your hand: " << userHand << endl;
	cout << "Your bet: " << bet << " dollars" << endl << endl;
}

// Displays the passed in dealer's hand and the passed in user's hand.
void displayGameState(BlackjackHand dealerHand, BlackjackHand userHand) {
	cout << "The dealer's hand: " << dealerHand << endl;
	cout << "Your hand: " << userHand << endl;
}

// Displays the passed in dealer's hand.
void displayGameState(BlackjackHand dealerHand) {
	cout << "The dealer's hand: " << dealerHand << endl;
}


//-------------------------------------------------------------------------------------------------
int main() {
	string userInput; // Holds the user's string inputs.
	int numDecks; // The number of decks to play with.
	double bet; // The user's bet on their hand.
	double totalWinnings = 0.0; // The total amount the user has gained or lost on all hands so far.
	bool blackjack; // True when the user gets a blackjack.
	bool bust; // True when the user busts.
	vector<Card> discardedCards; // The cards from previous hands that have been discarded but not yet added to the shoe.
	vector<Card> dealerCards; // The cards in the dealer's hand, used for populating discardedCards.
	vector<Card> userCards; // The cards in the user's hand, used for populating discardedCards.

	srand(static_cast<unsigned int>(time(NULL))); // Seeds the random number generator with the system time.

	// Gets the number of decks from the user and makes sure it's positive.
	while (true) {
		cout << "Enter number of decks to be in the shoe: ";
		cin >> numDecks;
		if (numDecks > 0) {
			break;
		}
		else {
			cout << "Please enter a positive number of decks." << endl;
		}
	}

	Shoe shoe(numDecks); // Makes the shoe of cards for this game.
	shoe.shuffle(); // Shuffles the cards.

	// Each iteration of the loop is a single hand played.
	while (true) {
		BlackjackHand userHand; // The user's hand.
		BlackjackHand dealerHand; // The dealer's hand.
		blackjack = false; // The user hasn't gotten a blackjack yet.
		bust = false; // The user hasn't busted yet.

		// Gets the user's initial bet and makes sure it's not negative.
		while (true) {
			cout << "Enter your bet in dollars: ";
			cin >> bet;
			// Ignores up to 100 characters in cin until it encounters a newline, allowing
			// getline to ignore the newline left over from previous input.
			cin.ignore(100, '\n');
			if (bet >= 0) {
				break;
			}
			else {
				cout << "Please enter a nonnegative bet." << endl;
			}
		}

		// Deals the dealer's first card and the user's first two cards.
		dealCardAndRefillShoeIfEmpty(dealerHand, shoe, discardedCards);
		dealCardAndRefillShoeIfEmpty(userHand, shoe, discardedCards);
		dealCardAndRefillShoeIfEmpty(userHand, shoe, discardedCards);

		// Checks if the user got a blackjack.
		if (userHand.value() == HAND_VALUE_LIMIT) {
			displayGameState(dealerHand, userHand);
			// In the case of a player blackjack, the bet is payed out at 1.5 times the normal rate.
			cout << "You got a blackjack! You win " << bet * 1.5 << " dollars." << endl << endl;
			blackjack = true;
			totalWinnings += bet * 1.5;
		}

		if (!blackjack) {
			// Adds cards to the player's hand until they choose to stop.
			while (true) {
				displayGameState(dealerHand, userHand, bet);

				// Exits the while loop if the player's hand has value HAND_VALUE_LIMIT.
				if (userHand.value() == HAND_VALUE_LIMIT) {
					cout << "Your hand now has a value of " << HAND_VALUE_LIMIT << "." << endl;
					break;
				}

				// Checks if the user has busted.
				if (userHand.value() > HAND_VALUE_LIMIT) {
					cout << "Bust! You lose " << bet << " dollars." << endl;
					bust = true;
					totalWinnings -= bet;
					break;
				}

				// Gets the user's decision and makes sure it's valid.
				while (true) {
					cout << "Enter hit, stand, or double down: ";
					getline(cin, userInput);
					if (userInput == "hit" || userInput == "stand" || userInput == "double down") {
						break;
					}
					else {
						cout << "Invalid input: " << userInput << endl;
					}
				}

				// Deals a card to the user if they chose to hit.
				if (userInput == "hit") {
					dealCardAndRefillShoeIfEmpty(userHand, shoe, discardedCards);
				}
				// Exits the while loop if they chose to stand.
				else if (userInput == "stand") {
					break;
				}
				// Doubles the bet, deals a card to the user, shows the hands and bet and exits the
				// while loop if they chose to double down.
				else if (userInput == "double down") {
					bet *= 2;
					dealCardAndRefillShoeIfEmpty(userHand, shoe, discardedCards);
					displayGameState(dealerHand, userHand, bet);

					// Checks if the user has busted.
					if (userHand.value() > HAND_VALUE_LIMIT) {
						cout << "Bust! You lose " << bet << " dollars." << endl;
						bust = true;
						totalWinnings -= bet;
					}

					break;
				}
			}

			// If the player didn't bust, the dealer will hit until it has a value of
			// DEALER_RESOLUTION_LIMIT and doesn't have a soft hand, or has a value of greater than
			// DEALER_RESOLUTION_LIMIT.
			while (!bust && (dealerHand.value() < DEALER_RESOLUTION_LIMIT || (dealerHand.value() == DEALER_RESOLUTION_LIMIT && dealerHand.isSoft()))) {
				cout << "The dealer's hand must still be resolved." << endl;
				dealCardAndRefillShoeIfEmpty(dealerHand, shoe, discardedCards);
				displayGameState(dealerHand);
			}

			// If the player didn't bust, the outcome of the hand is shown.
			if (!bust) {
				cout << "Outcome of this hand:" << endl;
				displayGameState(dealerHand, userHand, bet);

				// Tells the user how much they won or lost and why.
				if (dealerHand.value() > HAND_VALUE_LIMIT) {
					cout << "The dealer busted, so you win " << bet << " dollars." << endl << endl;
					totalWinnings += bet;
				}
				else if (dealerHand.value() > userHand.value()) {
					cout << "The dealer's hand had a higher value than yours, so you lose " << bet << " dollars." << endl << endl;
					totalWinnings -= bet;
				}
				else if (dealerHand.value() < userHand.value()) {
					cout << "The dealer's hand had a lower value than yours, so you win " << bet << " dollars." << endl << endl;
					totalWinnings += bet;
				}
				else {
					cout << "The dealer's hand had the same value as yours, so you don't win or lose any money." << endl << endl;
				}
			}
		}

		// Adds the cards that have been used for the user's and dealer's hands to the discardedCards vector.
		dealerCards = dealerHand.getCards();
		userCards = userHand.getCards();
		discardedCards.insert(discardedCards.end(), dealerCards.begin(), dealerCards.end()); // insert is used here to concatanate vectors.
		discardedCards.insert(discardedCards.end(), userCards.begin(), userCards.end());

		// Tells the user how much money they've won or lost in total so far.
		if (totalWinnings < 0.0) {
			cout << "So far you've lost a total of " << -totalWinnings << " dollars." << endl;
		}
		else {
			cout << "So far you've won a total of " << totalWinnings << " dollars." << endl;
		}

		// Gets the user's decision on wether or not to keep playing and makes sure it's valid.
		while (true) {
			cout << "Would you like to play another hand? Enter yes or no: ";
			cin >> userInput;
			if (userInput == "yes" || userInput == "no") {
				break;
			}
			else {
				cout << "Invalid input: " << userInput << endl;
			}
		}

		if (userInput == "yes") {
			cout << endl;
		}
		else if (userInput == "no") {
			cout << "Thanks for playing!" << endl;
			break;
		}
	}

	system("pause");
	return 0;
}