#include <ctype.h>
#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Points assigned to each letter of the alphabet
int POINTS[] = {1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10};
char letters[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};


int compute_score(string word);

int main(void)
{
    // Get input words from both players
    string word1 = get_string("Player 1: ");
    string word2 = get_string("Player 2: ");

    // Score both words
    int score1 = compute_score(word1);
    int score2 = compute_score(word2);

    // check who got the greater sum or if it was a tie.
    if (score1 > score2)
    {
        printf("Player 1 wins!");
    }
    else if (score2 > score1)
    {
        printf("Player 2 wins!");
    }
    else
    {
        printf("Tie!");
    }
    // TODO: Print the winner
}

int compute_score(string word)
{
    // get the number of elements in the array
    size_t n = sizeof(letters) / sizeof(letters[0]);
    // initialize the sum of points.
    int sum = 0;

    // loops through the word provided and compare it to the alphabetic array
    for (int i = 0; i < strlen(word); i++)
    {
        // if a given letter is lowercase, change it to uppercase so we can compare it with the alphabet array
        if (islower(word[i]))
        {
            word[i] = toupper(word[i]);
        }

        // iterate over the alphabet array and when the letter matches, sum it with the associate indexes from POINTS array.
        for (int k = 0; k < n; k++)
        {
            if (word[i] == letters[k])
            {
                sum += POINTS[k];
            }
        }
    }
    return sum;
}
