#include <stdio.h>
#include <cs50.h>
#include <stdbool.h>


int findnum(long x);
bool luhn(long x);
bool amExpress(long x);
bool masterCard(long x);
bool visa(long x);

int main(void)
{
    long input = 0;
    do
    {
        // prompt the user for a card number and loop it while this number is <= 0
        input = get_long("Type your card number(only numbers): ");
    }
    while (input <= 0);
    {
        int digits = findnum(input);  // finds out the lenght of the card number.
        if (digits == 15)  // if the lenght is 15, call american express function
        {
            amExpress(input);  // american express card validation
        }
        else if (digits == 13) // if the lenght of the card number is 13, calls visa card function
        {
            visa(input);  // visa card validation
        }
        // if the lenght of the card number is 16 and it starts with 4 call the visa function
        else if (digits == 16 && (input / 1000000000000000 == 4))
        {
            visa(input);
        }
        // if the lenght of the card number is 16 and it doesnt starts with 4 call the mastercard function
        else if (digits == 16 && (input / 1000000000000000 != 4))
        {
            masterCard(input);  // mastercard validation
        }
        else
        {
            printf("INVALID\n");  // print invalid if none condition is true
        }
    }
}

bool amExpress(long x)
{
    // if the first 2 digits is either 34 or 37 call luhn function
    if ((x / 10000000000000 == 34) || (x / 10000000000000 == 37))
    {

        if (luhn(x))  // if luhn function is true print the name of the card and return true.
        {
            printf("AMEX\n");
            return true;
        }
        else  // if the luhn validation is false print invalid and return false
        {
            printf("INVALID\n");
            return false;
        }
    }
    else  // if the card number does not start with either 34 or 37, print invalid and return false
    {
        printf("INVALID\n");
        return false;
    }
}

bool masterCard(long x)
{
    // call the luhn function if the card number first 2 digits is between 51 and 55
    if ((x / 100000000000000 >= 51) && (x / 100000000000000 <= 55))
    {
        if (luhn(x))  // if the luhn function returns true print the card name and return true, else print invale and return false
        {
            printf("MASTERCARD\n");
            return true;
        }
        else
        {
            printf("INVALID\n");
            return false;
        }
    }
    else  // if the card number does not start with the first two digits that validate mastercard cards, print invalid and return false
    {
        printf("INVALID\n");
        return false;
    }
}

bool visa(long x)
{
    // if the card lenght is either 16 or 13 and starts with 4, call the luhn function
    if ((x / 1000000000000000 == 4) || (x / 1000000000000 == 4))
    {
        if (luhn(x))  // if the luhn function returns true, print the card name and return true. else print invalid and return false
        {
            printf("VISA\n");
            return true;
        }
        else
        {
            printf("INVALID\n");
            return false;
        }
    }
    else  // print invalid and return false if the card number does not start with 4
    {
        printf("INVALID\n");
        return false;
    }
    return false;
}

int findnum(long x)
{
    // finds out the number of digits by dividing the card number by 10 on each iteration untill the card number divided by 10 is equal 0
    int numDigits;
    for (numDigits = 0; x != 0; x /= 10, numDigits++);
    return numDigits;
}


bool luhn(long x)
{
    int sum = 0, count = 0;  // the total sum and a counter
    int digit = 0; // the digit from the card number

    // iterate over the card number digits by dividing the card number by 10.
    for (count = 0; x != 0; count++, x /= 10)
    {
        // the sum is initialize to 0, then the sum is equal to the sum + the last digit of the card number if the counter is even.
        if (count % 2 == 0)
        {

            sum += (int)(x % 10);
        }
        // if the counter is odd, multiply the last digit by 2 and if this multiplication results in a 2 digits number, sum both digits.
        else
        {
            digit = 2 * (int)(x % 10);
            sum += (digit / 10) + (digit % 10);
        }
    }
    return (sum % 10) == 0;  // verify if the last digit of the sum is 0, if it isn't, return false.
}
