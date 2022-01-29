// After the completion of part of week 6 problem set I decided to redo some of the previous exercises.
// The hash function idea was adapted from geeksforgeeks.org searching for ways to find substrings repeats in a given string.
// Basicly everything stays exactly the same as the last time that I submitted the problem, it's just more cleaner now.

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <cs50.h>
#include <stdlib.h>

#define MAX 256

string encrypt(string text, string crypt, string cipher);
bool hasRepeat(char *str);
bool fullAlpha(char *str);

int main(int argc, char *argv[])
{
    // Ensures the user only input 2 command-line arguments
    if (argc != 2)
    {
        printf("Usage: ./substitution key\n");
        return 1;
    }

    // Make sure that the key only contains 26 non-repeatable alphabetic characters.
    else if (strlen(argv[1]) != 26 || !fullAlpha(argv[1]) || hasRepeat(argv[1]))
    {
        printf("Key must contain 26 alphabetic characters.\n");
        return 1;
    }

    else
    {
        printf("plaintext: ");

        string input = get_string("");

        // Malloc so we can use a variable size input from the user and print the ciphertext version of it in the screen.
        // Worth mentioned that I had to search about variable size strings before using this function in my first submission
        // of the problem (back in June when I had not yet watched the week 4 class.)
        char *cipher = malloc((strlen(input)) * sizeof(char));

        if (cipher != NULL)
        {
            printf("ciphertext: %s\n", encrypt(input, argv[1], cipher));

            // Free the memory allocated.
            free(cipher);
            return 0;
        }

        else
        {
            // Return 1 if we have a problem with the memory malloc'd.
            return 1;

        }
    }
}

string encrypt(string text, string crypt, string cipher)
{
    // Reference string to compare with the crypt key.
    char key[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Loops the text input through the key. When a match is found assign the ciphered key provided by the user.
    for (int i = 0; i < strlen(key); i++)
    {
        for (int k = 0; k < strlen(text); k++)
        {
            if (isalpha(text[k]))
            {
                if (isupper(text[k]))
                {
                    crypt[i] = toupper(crypt[i]);
                    key[i] = toupper(key[i]);

                    if (text[k] == key[i])
                    {
                        cipher[k] = crypt[i];
                    }
                }
                else
                {
                    crypt[i] = tolower(crypt[i]);
                    key[i] = tolower(key[i]);

                    if (text[k] == key[i])
                    {
                        cipher[k] = crypt[i];
                    }
                }
            }
            else
            {
                cipher[k] = text[k];
            }
        }
    }
    return cipher;
}

// Checks for characters repeats in the ciphered key by creating a hash table with no collisions possible.
// When found a repetition of a character returns True.
bool hasRepeat(char *str)
{
    int c;
    int pos = -1;
    int hash[MAX] = {0};
    for (int i = 0; i < strlen(str); i++)
    {
        c = (int)str[i];
        if (hash[c] == 0)
        {
            hash[c]++;
        }
        else if (hash[c] == 1)
        {
            hash[c]++;
            return true;
        }
    }
    return false;
}

// Checks if a ciphered key contains only alphabetic characters
bool fullAlpha(char *str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (!isalpha(str[i]))
        {
            return false;
        }
    }
    return true;
}