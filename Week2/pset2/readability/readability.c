#include <stdio.h>
#include <cs50.h>
#include <math.h>
#include <string.h>

#include <ctype.h>

void gradeLvl(string x);
float letterAvg(string x);
float sentenceAvg(string x);

int w = 0; // number of words

int main(void)
{
    string input;
    input = get_string("Text: ");       // get a input from user.
    gradeLvl(input);        // function that prints the grade level of the text the user input.
}

void gradeLvl(string x)
{
    float L = letterAvg(x);       // function that count the letters and get the avg of letters/100 words. and number of words.
    float S = sentenceAvg(x);     // function that count the number of sentences and get the average of sentences/100 words.
    // index algorithm
    float index = 0.0588 * L - 0.296 * S - 15.8;
    if (roundf(index) >= 16)        // if the index is greather than 16 prints the grade.
    {
        printf("Grade 16+\n");
    }
    else if (roundf(index) < 1)     // if the index is less than 1 prints the grade.
    {
        printf("Before Grade 1\n");
    }
    else        // prints the grade.
    {
        printf("Grade %i\n", (int)roundf(index));

    }
}

float letterAvg(string x)
{
    float L = 0.0;        // average number of letters/100 words.
    int l = 0;      // number of words and number of letters.

    for (int i = 0; i < strlen(x); i++)      // loop that count the number of words and letters
    {
        if (x[i] == ' ')        // if the char is a space, than it will add 1 to the number of words.
        {
            w++;
        }
        else if (isalnum(x[i]))     // if the char is alphanumeric then adds 1 to the letters.
        {
            l++;
        }
    }
    w++;        // add the last word of the sentence

    float avg = (l * 100.00) / w;       // get the avg of letters/ 100 words.
    return L = roundf(avg * 100) / 100;     // round the avg of letters to .2 decimals of a floating point.
}

float sentenceAvg(string x)
{
    float S = 0.0;        // average number of sentence/100 words.
    int s = 0;      // number of sentences.

    for (int i = 0; i < strlen(x); i++)      // loop that count the number of sentences.
    {
        // if the char is either a '.', '!' or a '?' add 1 to the number of sentences.
        if (x[i] == '.' || x[i] == '!' || x[i] == '?')
        {
            s++;
        }
    }
    float avg = (s * 100.0) / w;       // get the avg of sentences/100 words.
    return S = roundf(avg * 100) / 100;     // round the float number to 2 decimal places.
}
