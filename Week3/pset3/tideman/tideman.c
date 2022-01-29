#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
void mergeSort(pair x[], int n);
void merge(pair x[], pair left[], pair right[], int nL, int nR);
bool hasCycle(int index, bool hash[]);
bool cycle(int index);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = (i + 1); j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }

        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    mergeSort(pairs, (pair_count + 1));
    return;
}

// Merge sort function
void mergeSort(pair x[], int n)
{

    if (n < 2)
    {
        return;
    }
    int middle = n / 2;
    pair left[middle];
    pair right[n - middle];

    int elL = middle;
    int elR = (n - middle);

    for (int i = 0; i < middle; i++)
    {
        left[i].winner = x[i].winner;
        left[i].loser = x[i].loser;
    }
    for (int j = middle; j < n; j++)
    {
        right[j - middle].winner = x[j].winner;
        right[j - middle].loser = x[j].loser;
    }

    mergeSort(left, elL);
    mergeSort(right, elR);
    merge(x, left, right, elL, elR);
}

void merge(pair x[], pair left[], pair right[], int nL, int nR)
{
    // Initialize the indexes of the arrays.
    int i = 0;
    int j = 0;
    int k = 0;

    while (i < nL && j < nR)
    {
        // If a given pair of candidates have more votes than the compared one, sort it in in descending order.
        if (preferences[left[i].winner][left[i].loser] > preferences[right[j].winner][right[j].loser])
        {
            x[k].winner = left[i].winner;
            x[k].loser = left[i].loser;
            i++;
        }
        else
        {
            x[k].winner = right[j].winner;
            x[k].loser = right[j].loser;
            j++;
        }
        k++;
    }

    //Copy the rest of Left[] to the main array x[]
    while (i < nL)
    {
        x[k].winner = left[i].winner;
        x[k].loser = left[i].loser;
        i++;
        k++;
    }
    //Copy the rest of Right[] to the main array x[]
    while (j < nR)
    {
        x[k].winner = right[j].winner;
        x[k].loser = right[j].loser;
        j++;
        k++;
    }
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {

        locked[pairs[i].winner][pairs[i].loser] = true;
        if (cycle(pairs[i].winner))
        {
            locked[pairs[i].winner][pairs[i].loser] = false;
        }
    }

    return;
}

bool cycle(int index)
{
    bool wasLock[candidate_count];
    // Initialize wasLock to false
    for (int i = 0; i < candidate_count; i++)
    {
        wasLock[i] = false;
    }
    return hasCycle(index, wasLock);
}

bool hasCycle(int index, bool hash[])
{
    if (hash[index])
    {
        return true;
    }
    hash[index] = true;
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[index][i] && hasCycle(i, hash))
        {
            return true;
        }
    }
    return false;
}

// Print the winner of the election
void print_winner(void)
{
    string win = "";
    // loop over every winner against a loser, if a loser(j) does not have a winner locked,
    // he is the winner of the election (because he didn't lose to anybody)
    for (int j = 0; j < candidate_count; j++)
    {
        for (int i = 0; i < candidate_count; i++)
        {
            if (!locked[i][j])
            {
                if (i == candidate_count - 1)
                {
                    win = candidates[j];
                    printf("%s\n", win);
                    return;
                }
            }
            else
            {
                break;
            }
        }
    }
    return;
}