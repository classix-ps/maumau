#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define D 32
#define Computer true

const char* symbols[4] = {"PIK", "KREUZ", "HERZ", "KARO"};
const char* values[8] = {"7", "8", "9", "10", "BUBE", "DAME", "KOENIG", "ASS"};

// Prints card given its integer form
void printCard(int c) {
    printf("%s %s", symbols[c / 8], values[c % 8]);
}

// Prints all cards in hand
void printHand(size_t currentPlayer, int* hands) {
    char hand[480] = "";
    size_t cardCount = 1;
    bool beginning = true;
    for (size_t handPos = 0; handPos < D; handPos++) {
        int currentCard = hands[currentPlayer * D + handPos];
        if (currentCard != -1) {
            if (beginning) {
                beginning = false;
            }
            else {
                strcat(hand, ",  ");
            }

            char num[6];
            sprintf(num, "(%d) ", cardCount);
            strcat(hand, num);
            char card[13];
            sprintf(card, "%s %s", symbols[currentCard / 8], values[currentCard % 8]);
            strcat(hand, card);

            cardCount++;
        }
    }

    printf(hand);
}

// Counts cards in players hand
size_t countCards(size_t currentPlayer, int* hands) {
    size_t cardCount = 0;
    for (size_t handPos = 0; handPos < D; handPos++) {
        cardCount += hands[currentPlayer * D + handPos] != -1;
    }

    return cardCount;
}

// Prints all potential options for player's actions
void printOptions(size_t currentPlayer, int* hands) {
    size_t cardCount = countCards(currentPlayer, hands);

    printf("[1 bis %d] - Karte spielen,  [0] - Aufnehmen,  [-1] - Spiel beenden\n", cardCount);
}

// Shuffles deck
void shuffle(int* deck) {
    srand(time(NULL));
    for (size_t i = 0; i < D - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (D - i) + 1);
        int t = deck[j];
        deck[j] = deck[i];
        deck[i] = t;
    }
}

// Deals cards to all players
void deal(int* deck, int* hands, int players, int dealt) {
    for (size_t player = 0; player < players; player++) {
        for (size_t dealing = 0; dealing < dealt; dealing++) {
            hands[player * D + dealing] = deck[player * dealt + dealing];
        }
        for (size_t rest = dealt; rest < D; rest++) {
            hands[player * D + rest] = -1;
        }
    }
}

// Draw card from pile
void drawCard(size_t currentPlayer, int* hands, int card) {
    for (size_t handPos = 0; handPos < D; handPos++) {
        if (hands[currentPlayer * D + handPos] == -1) {
            hands[currentPlayer * D + handPos] = card;
            return;
        }
    }
}

// Checks if deck is empty
bool isDeckEmpty(size_t currentCard) {
    if (currentCard > D) {
        printf("Deck is empty.\n");
        return true;
    }

    return false;
}

// Check if player has empty hand
bool isPlayerFinished(size_t currentPlayer, int* hands) {
    if (!countCards(currentPlayer, hands)) {
        printf("Player %d has won!\n", currentPlayer + 1);
        return true;
    }
    else {
        return false;
    }
}

// Check if chosen card in hand is compatible with card on pile
bool isValidMove(int* pileCard, size_t currentPlayer, int option, int* hands) {
    for (size_t handPos = 0, cardCount = 0; handPos < D; handPos++) {
        int chosenCard = hands[currentPlayer * D + handPos];
        if (chosenCard != -1) {
            if (cardCount == option) {
                if (*pileCard % 8 == chosenCard % 8 || *pileCard / 8 == chosenCard / 8) {
                    *pileCard = chosenCard;
                    hands[currentPlayer * D + handPos] = -1;
                    return true;
                }
            }
            cardCount++;
        }
    }

    return false;
}

void computerMove(int* pileCard, size_t currentPlayer, int* hands, int* deck, size_t* deckPos) {
    size_t cardCount = countCards(currentPlayer, hands);

    // Play card
    for (size_t handPos = 0; handPos < D; handPos++) {
        int chosenCard = hands[currentPlayer * D + handPos];
        if (chosenCard != -1 && (*pileCard % 8 == chosenCard % 8 || *pileCard / 8 == chosenCard / 8)) {
            *pileCard = chosenCard;
            hands[currentPlayer * D + handPos] = -1;
            printf("Player %d has played ", currentPlayer + 1);
            printCard(chosenCard);
            printf(" and now has %d cards in hand.\n", cardCount - 1);
            return;
        }
    }

    // Draw from pile
    drawCard(currentPlayer, hands, deck[*deckPos]);
    *deckPos++;
    printf("Player %d has drawn a card and now has %d cards in hand.\n", currentPlayer + 1, cardCount + 1);
}

int main (int argc, char* argv[]) {
    if (argc != 3) {
        printf("Incorrect command line arguments. Usage: <maumau Executable> <Number of Players> <Number of cards dealt>\n");
        return EXIT_FAILURE;
    }

    int players = atoi(argv[1]);
    int dealt = atoi(argv[2]);

    if (players * dealt >= D) {
        printf("Deck is not large enough to be dealt to all players.\n");
        return EXIT_FAILURE;
    }

    int hands[D][D];

    int deck[D];
    for (int i = 0; i < D; i++) {
        deck[i] = i;
    }

    shuffle(deck);

    deal(deck, hands, players, dealt);

    size_t deckPos = players * dealt;
    int pileCard = deck[deckPos++];
    size_t currentPlayer = 0;
    while (!isDeckEmpty(deckPos)) {
        printf("Player %d ist dran. ", currentPlayer + 1);

        printf("Es liegt:\t");
        printCard(pileCard);
        printf("\n");

        if (currentPlayer && Computer) {
            computerMove(&pileCard, currentPlayer, hands, deck, &deckPos);
        }
        else {
            printf("\nIhre Karten sind:\n  ");
            printHand(currentPlayer, hands);
            printf("\n\n");

            printOptions(currentPlayer, hands);

            int input = -2;
            char term;
            while (input != -1) {
                printf("Ihr Zug: ");
                if (scanf("%d%c", &input, &term) != 2 || term != '\n') {
                    printf("Ungueltige Eingabe. Bitte versuchen Sie es erneut.\n");
                    scanf("%*s");
                    continue;
                }

                if (input == -1) {
                    printf("Das Spiel wurde beendet.\n");
                    return EXIT_SUCCESS;
                }
                else if (input == 0) {
                    drawCard(currentPlayer, hands, deck[deckPos]);
                    deckPos++;
                    break;
                }
                else if (!isValidMove(&pileCard, currentPlayer, input - 1, hands)) {
                    printf("Ungueltige Karte. Bitte versuchen Sie es erneut.\n");
                    continue;
                }
                else {
                    break;
                }
            }
        }

        printf("\n\n");

        if (isPlayerFinished(currentPlayer, hands)) {
            break;
        }

        currentPlayer = (currentPlayer + 1) % players;
    }

    return EXIT_SUCCESS;
}