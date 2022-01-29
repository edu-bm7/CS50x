#include "helpers.h"
#include <math.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    int average = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            average = round((image[i][j].rgbtRed + image[i][j].rgbtBlue + image[i][j].rgbtGreen) / 3.0);
            image[i][j].rgbtRed = average;
            image[i][j].rgbtGreen = average;
            image[i][j].rgbtBlue = average;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE tmp[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = width - 1, s = 0; j >= 0; j--, s++)
        {
            tmp[i][j] = image[i][s];
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = tmp[i][j];
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE tmp[height][width];
    // Set the tmp RGB colours.
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float count = 0.0;
            int i3[] = {i - 1, i, i + 1};
            int j3[] = {j - 1, j, j + 1};
            int R = 0, G = 0, B = 0;

            // Iterate over every radious around a pixel and add the values of RGB to the total.
            for (int k =  0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    int nbHeight = i3[k];
                    int nbWidth = j3[l];

                    if (nbHeight >= 0 && nbWidth >= 0 && nbHeight < height && nbWidth < width)
                    {
                        R += image[nbHeight][nbWidth].rgbtRed;
                        G += image[nbHeight][nbWidth].rgbtGreen;
                        B += image[nbHeight][nbWidth].rgbtBlue;
                        count++;
                    }
                }
            }

            // Set the averages of the RGB totals to the tmp variable.
            tmp[i][j].rgbtRed = round(R / count);
            tmp[i][j].rgbtGreen = round(G / count);
            tmp[i][j].rgbtBlue = round(B / count);
        }
    }

    // copy the values of the RGB pixels of the tmp variable to the image.
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = tmp[i][j];
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE tmp2[height][width];

    // Set the tmp RGB colours.
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int i3[] = {i - 1, i, i + 1};
            int j3[] = {j - 1, j, j + 1};
            float RGx = 0.0, GGx = 0.0, BGx = 0.0;
            float RGy = 0.0, GGy = 0.0, BGy = 0.0;

            // Iterate over every radious around a pixel and add the values of RGB to the total Gx.
            for (int k =  0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    int nbHeight = i3[k];
                    int nbWidth = j3[l];

                    if (nbHeight >= 0 && nbWidth >= 0 && nbHeight < height && nbWidth < width)
                    {
                        if (nbWidth == j)
                        {
                            RGx += image[nbHeight][nbWidth].rgbtRed * 0;
                            GGx += image[nbHeight][nbWidth].rgbtGreen * 0;
                            BGx += image[nbHeight][nbWidth].rgbtBlue * 0;
                        }

                        else if (nbWidth == (j - 1))
                        {
                            if (nbHeight == i)
                            {
                                RGx += image[nbHeight][nbWidth].rgbtRed * -2;
                                GGx += image[nbHeight][nbWidth].rgbtGreen * -2;
                                BGx += image[nbHeight][nbWidth].rgbtBlue * -2;
                            }
                            else if (nbHeight == (i - 1) || nbHeight == (i + 1))
                            {
                                RGx += image[nbHeight][nbWidth].rgbtRed * -1;
                                GGx += image[nbHeight][nbWidth].rgbtGreen * -1;
                                BGx += image[nbHeight][nbWidth].rgbtBlue * -1;
                            }

                        }

                        else if (nbWidth == (j + 1))
                        {
                            if (nbHeight == i)
                            {
                                RGx += image[nbHeight][nbWidth].rgbtRed * 2;
                                GGx += image[nbHeight][nbWidth].rgbtGreen * 2;
                                BGx += image[nbHeight][nbWidth].rgbtBlue * 2;
                            }
                            else if (nbHeight == (i - 1) || nbHeight == (i + 1))
                            {
                                RGx += image[nbHeight][nbWidth].rgbtRed;
                                GGx += image[nbHeight][nbWidth].rgbtGreen;
                                BGx += image[nbHeight][nbWidth].rgbtBlue;
                            }
                        }
                    }
                }
            }
            // Iterate over every radious around a pixel and add the values of RGB to the total Gy.
            for (int m =  0; m < 3; m++)
            {
                for (int n = 0; n < 3; n++)
                {
                    int nbHeight = i3[m];
                    int nbWidth = j3[n];
                    if (nbHeight >= 0 && nbWidth >= 0 && nbHeight < height && nbWidth < width)
                    {
                        if (nbHeight == i)
                        {
                            RGy += image[nbHeight][nbWidth].rgbtRed * 0;
                            GGy += image[nbHeight][nbWidth].rgbtGreen * 0;
                            BGy += image[nbHeight][nbWidth].rgbtBlue * 0;
                        }

                        else if (nbHeight == (i - 1))
                        {
                            if (nbWidth == j)
                            {
                                RGy += image[nbHeight][nbWidth].rgbtRed * -2;
                                GGy += image[nbHeight][nbWidth].rgbtGreen * -2;
                                BGy += image[nbHeight][nbWidth].rgbtBlue * -2;
                            }
                            else if (nbWidth == (j - 1) || nbWidth == (j + 1))
                            {
                                RGy += image[nbHeight][nbWidth].rgbtRed * -1;
                                GGy += image[nbHeight][nbWidth].rgbtGreen * -1;
                                BGy += image[nbHeight][nbWidth].rgbtBlue * -1;
                            }

                        }

                        else if (nbHeight == (i + 1))
                        {
                            if (nbWidth == j)
                            {
                                RGy += image[nbHeight][nbWidth].rgbtRed * 2;
                                GGy += image[nbHeight][nbWidth].rgbtGreen * 2;
                                BGy += image[nbHeight][nbWidth].rgbtBlue * 2;
                            }
                            else if (nbWidth == (j - 1) || nbWidth == (j + 1))
                            {
                                RGy += image[nbHeight][nbWidth].rgbtRed;
                                GGy += image[nbHeight][nbWidth].rgbtGreen;
                                BGy += image[nbHeight][nbWidth].rgbtBlue;
                            }
                        }
                    }
                }
            }
            // Set the values of the RGB in tmp, capped at 255.
            tmp2[i][j].rgbtRed = round(sqrtf(powf(RGx, 2) + powf(RGy, 2)));
            tmp2[i][j].rgbtGreen = round(sqrtf(powf(GGx, 2) + powf(GGy, 2)));
            tmp2[i][j].rgbtBlue = round(sqrtf(powf(BGx, 2) + powf(BGy, 2)));

            if (round(sqrtf(powf(RGx, 2) + powf(RGy, 2))) > 255)
            {
                tmp2[i][j].rgbtRed = 255;
            }

            if (round(sqrtf(powf(GGx, 2) + powf(GGy, 2))) > 255)
            {
                tmp2[i][j].rgbtGreen = 255;
            }

            if (round(sqrtf(powf(BGx, 2) + powf(BGy, 2))) > 255)
            {
                tmp2[i][j].rgbtBlue = 255;
            }
        }
    }

    // copy the values of the RGB pixels from the tmp variable to the image.
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = tmp2[i][j];
        }
    }
    return;
}


