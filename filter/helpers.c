#include "helpers.h"
#include <math.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float rgb_val = 0;
            rgb_val += image[i][j].rgbtBlue;
            rgb_val += image[i][j].rgbtGreen;
            rgb_val += image[i][j].rgbtRed;
            rgb_val /= 3.0;
            rgb_val = roundf(rgb_val);

            image[i][j].rgbtBlue = (int) rgb_val;
            image[i][j].rgbtGreen = (int) rgb_val;
            image[i][j].rgbtRed = (int) rgb_val;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            int temp_r = 0, temp_g = 0, temp_b = 0;
            temp_b = image[i][j].rgbtBlue;
            temp_g = image[i][j].rgbtGreen;
            temp_r = image[i][j].rgbtRed;

            image[i][j].rgbtBlue = image[i][width - 1 - j].rgbtBlue;
            image[i][j].rgbtGreen = image[i][width - 1 - j].rgbtGreen;
            image[i][j].rgbtRed = image[i][width - 1 - j].rgbtRed;

            image[i][width - 1 - j].rgbtBlue = temp_b;
            image[i][width - 1 - j].rgbtGreen = temp_g;
            image[i][width - 1 - j].rgbtRed = temp_r;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE image_copy[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float count = 0;
            float avg_r = 0, avg_g = 0, avg_b = 0;

            // 3x3 box scan
            for (int box_r = i - 1; box_r < i + 2; box_r++)
            {
                for (int box_c = j - 1; box_c < j + 2; box_c++)
                {
                    if (box_r >= 0 && box_r < height && box_c >= 0 && box_c < width)
                    {
                        avg_b += image[box_r][box_c].rgbtBlue;
                        avg_g += image[box_r][box_c].rgbtGreen;
                        avg_r += image[box_r][box_c].rgbtRed;
                        count++;
                    }
                }
            }

            // average each channel
            avg_r /= count;
            avg_g /= count;
            avg_b /= count;

            // rounding
            avg_r = roundf(avg_r);
            avg_g = roundf(avg_g);
            avg_b = roundf(avg_b);

            // assign new value
            image_copy[i][j].rgbtBlue = (int) avg_b;
            image_copy[i][j].rgbtGreen = (int) avg_g;
            image_copy[i][j].rgbtRed = (int) avg_r;
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtBlue = image_copy[i][j].rgbtBlue;
            image[i][j].rgbtGreen = image_copy[i][j].rgbtGreen;
            image[i][j].rgbtRed = image_copy[i][j].rgbtRed;
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // init kernel
    int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    // padding the image
    RGBTRIPLE image_padding[height + 2][width + 2];
    RGBTRIPLE image_copy[height][width];
    for (int i = 0; i < height + 2; i++)
    {
        for (int j = 0; j < width + 2; j++)
        {
            if (i == 0 || i == height + 1 || j == 0 || j == width + 1)
            {
                image_padding[i][j].rgbtBlue = 0;
                image_padding[i][j].rgbtGreen = 0;
                image_padding[i][j].rgbtRed = 0;
            }
            else
            {
                image_padding[i][j].rgbtBlue = image[i - 1][j - 1].rgbtBlue;
                image_padding[i][j].rgbtGreen = image[i - 1][j - 1].rgbtGreen;
                image_padding[i][j].rgbtRed = image[i - 1][j - 1].rgbtRed;
            }
        }
    }

    // convolusion
    for (int i = 1; i < height + 1; i++)
    {
        for (int j = 1; j < width + 1; j++)
        {

            // gx
            float x_r = 0, x_g = 0, x_b = 0;
            for (int box_r = 0, index_i = i - 1; box_r < 3; box_r++, index_i++)
            {
                for (int box_c = 0, index_j = j - 1; box_c < 3; box_c++, index_j++)
                {
                    x_b += gx[box_r][box_c] * image_padding[index_i][index_j].rgbtBlue;
                    x_g += gx[box_r][box_c] * image_padding[index_i][index_j].rgbtGreen;
                    x_r += gx[box_r][box_c] * image_padding[index_i][index_j].rgbtRed;
                }
            }


            // gy
            float y_r = 0, y_g = 0, y_b = 0;
            for (int box_r = 0, index_i = i - 1; box_r < 3; box_r++, index_i++)
            {
                for (int box_c = 0, index_j = j - 1; box_c < 3; box_c++, index_j++)
                {
                    y_b += gy[box_r][box_c] * image_padding[index_i][index_j].rgbtBlue;
                    y_g += gy[box_r][box_c] * image_padding[index_i][index_j].rgbtGreen;
                    y_r += gy[box_r][box_c] * image_padding[index_i][index_j].rgbtRed;
                }
            }

            // compute rgb value
            float temp_r = 0,  temp_g = 0,  temp_b = 0;
            temp_r = roundf(sqrt(pow(x_r, 2) + pow(y_r, 2)));
            temp_g = roundf(sqrt(pow(x_g, 2) + pow(y_g, 2)));
            temp_b = roundf(sqrt(pow(x_b, 2) + pow(y_b, 2)));

            // clip value
            if (temp_r > 255)
            {
                temp_r = 255;
            }
            if (temp_g > 255)
            {
                temp_g = 255;
            }
            if (temp_b > 255)
            {
                temp_b = 255;
            }

            // assign value to copy
            image_copy[i - 1][j - 1].rgbtBlue = temp_b;
            image_copy[i - 1][j - 1].rgbtGreen = temp_g;
            image_copy[i - 1][j - 1].rgbtRed = temp_r;
        }
    }

    // write back from copy to image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = image_copy[i][j];
        }
    }

    return;
}
