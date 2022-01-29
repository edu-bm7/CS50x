#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// A BYTE type so we can check the first four bytes of a block of data.
typedef uint8_t BYTE;

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage: recover [file]\n");
        return 1;
    }

    // Open the file given by the user and check for errors with the openning.
    FILE *disk = fopen(argv[1], "r");
    if (disk == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        return 2;
    }

    // The block of data to be read.
    size_t block = 0;

    // The buffer used to store 512 bytes of data.
    BYTE buffer[512];

    // Variable used to close the files except the first file. And it prints an error if the number of files surpass three digits.
    int n = 0;
    // Counter variable to set the name of the file.
    int counter = 0;
    // Variable used to write data to the file image only after the first image was found.
    int fileFound = -1;
    // The images files.
    FILE *file;

    // Read data and stores it into the buffer while it doesn't reach EOF.
    while ((block = fread(&buffer, sizeof(buffer), 1, disk)) == 1)
    {
        // Check for the presence of a jpeg image.
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] >> 4) == 0xe)
        {
            fileFound = 1;

            // Close the last file. Only works after the first one.
            if (n > 0)
            {
                fclose(file);
            }

            if (n > 999)
            {
                fprintf(stderr, "Limit number of files reached. 999 files.\n");
                return 3;
            }

            // Allocate memory to the string with the size of 8 characters (###.jpg + the NULL terminate character).
            char *fileName = (char *) malloc(8 * sizeof(char));

            // The next three if statetments are the same, they assign the name of the file to a string then open the file with that name based on the counter value.
            if (counter < 10)
            {
                sprintf(fileName, "00%i.jpg", counter);
                file = fopen(fileName, "w");
                if (file == NULL)
                {
                    fprintf(stderr, "Could not open %s.\n", fileName);
                    free(fileName);
                    return 4;
                }
            }

            else if (counter < 100 && counter >= 10)
            {
                sprintf(fileName, "0%i.jpg", counter);
                file = fopen(fileName, "w");
                if (file == NULL)
                {
                    fprintf(stderr, "Could not open %s.\n", fileName);
                    free(fileName);
                    return 4;
                }
            }

            else if (counter < 1000 && counter >= 100)
            {
                sprintf(fileName, "%i.jpg", counter);
                file = fopen(fileName, "w");
                if (file == NULL)
                {
                    fprintf(stderr, "Could not open %s.\n", fileName);
                    free(fileName);
                    return 4;
                }
            }

            // Free the string variable that have been malloc'd.
            free(fileName);
            counter++;
            n++;
        }

        // Write data from the buffer to the file only if a first file was found.
        if (fileFound == 1)
        {
            fwrite(&buffer, sizeof(buffer), 1, file);
        }
    }
    fclose(file);
    fclose(disk);
}




