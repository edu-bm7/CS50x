#include <stdio.h>
#include <cs50.h>
#include <stdlib.h>

void wall(string x);
void spaces(int x);
void hashes(int x);
string get_height(void);

int main(void)
{
    string prompt = get_height();

    wall(prompt);

}

string get_height(void)
{
    string height = "";
    do
    {
        height = get_string("Height: ");
    }
    while (atoi(height) < 1 || atoi(height) > 8);
    return height;

}


void wall(string x)
{
    int height = atoi(x);
    int initial_space = (height - 1);
    int middle_space = 2;

    for (int i = 0; i < (height); i++)
    {
        spaces(initial_space);
        hashes((i + 1));
        spaces(middle_space);
        hashes((i + 1));
        printf("\n");
        initial_space--;
    }
}

void spaces(int x)
{
    for (int i = 0; i < x; i++)
    {
        printf(" ");
    }
}

void hashes(int x)
{
    for (int i = 0; i < x; i++)
    {
        printf("#");
    }

}