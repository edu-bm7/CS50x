// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

#include "dictionary.h"

#define MAX_UNSIGNED 490000

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

int new_word = 0;

// TODO: Choose number of buckets in hash table
const unsigned int N = MAX_UNSIGNED;

// Hash table
node *table[N];

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    unsigned int s = strlen(word);
    char cmpword[s];

    // Set word to lower case so the hash value can be the same.
    for (int i = 0; i <= s; i++)
    {
        cmpword[i] = word[i];
        cmpword[i] = tolower(cmpword[i]);
    }

    unsigned long check_value = hashfnc(cmpword);
    check_value = check_value / 10000;
    node *tmp = table[check_value];

    if (table[check_value] == NULL)
    {
        return false;
    }

    // Check the table for the word
    else if (strcasecmp(word, tmp->word) == 0)
    {
        return true;
    }

    // Check the linked list, if it exist, for the word.
    else
    {
        while (tmp != NULL)
        {
            if (strcasecmp(tmp->word, word) == 0)
            {
                return true;
            }
            else
            {
                tmp = tmp->next;
            }
        }
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    unsigned int hash_value = hashfnc(word);
    unsigned int wordlen = strlen(word);
    hash_value = hash_value / 10000;
    // Allocate memory to the table[index] and populate the word if the table is empity.
    if (table[hash_value] == NULL)
    {
        table[hash_value] = calloc(1, sizeof(node));
        for (int i = 0; i <= wordlen; i++)
        {
            table[hash_value]->word[i] = word[i];
        }
        table[hash_value]->next = NULL;
    }


    // Create a Linked list starting from the table[index] if the table is not empity.
    else
    {
        node *linkedNode = calloc(1, sizeof(node));
        for (int i = 0; i <= wordlen; i++)
        {
            linkedNode->word[i] = word[i];
        }

        linkedNode->next = NULL;

        if (table[hash_value]->next == NULL)
        {
            table[hash_value]->next = linkedNode;
        }

        else
        {
            node *tmp = table[hash_value];
            while (tmp->next != NULL)
            {
                tmp = tmp->next;
            }
            tmp->next = linkedNode;
        }
    }
    return hash_value;
}

// Hash function from http://www.sanmayce.com/Fastest_Hash/ FNV1A_Yorikke function
unsigned int hashfnc(const char *word)
{
    unsigned int hash = 5387;
    int i;

    while (!(0 == (i = *word++)))
    {
        hash = ((hash << 7) + hash) + i;
    }
    return hash;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    for (int i = 0; i < N; i++)
    {
        table[i] = NULL;
    }

    FILE *dict = fopen(dictionary, "r");

    if (dict == NULL)
    {
        fclose(dict);
        return false;
    }

    char nword[LENGTH + 1];
    int index = 0;

    // Finds words in dictionary and hash them.
    for (int i = fgetc(dict); i != EOF; i = fgetc(dict))
    {
        nword[index] = i;

        if (nword[index] == '\n')
        {
            nword[index] = '\0';
            hash(nword);
            new_word++;
            index = 0;
        }

        else
        {
            index++;
        }

    }

    fclose(dict);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    return new_word;

}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    return free_nodes();
}

bool free_nodes(void)
{
    //Free nodes of the hash table.
    for (int i = 0; i < N; i++)
    {
        if (table[i] != NULL)
        {
            while (table[i] != NULL)
            {
                node *tmp = table[i]->next;
                free(table[i]);
                table[i] = tmp;
            }
        }
    }
    return true;
}
