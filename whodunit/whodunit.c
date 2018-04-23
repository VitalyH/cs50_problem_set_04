#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int main(int argc, char *argv[])
{
    // If != two command-line arguments
    if (argc != 3)
    {
        printf("Usage: whodunnit needs cluefile! \n");
        return 1;
    }

    // If the input file cannot be opened for reading
    FILE *clue_ptr = fopen(argv[1], "r");
    if (clue_ptr == NULL)
    {
        printf("Can't open %s.\n", argv[1]);
        return 2;
    }

    // If the output file cannot be opened for writing
    FILE *solution_ptr = fopen("solution.bmp", "w");
    if (solution_ptr == NULL)
    {
        fclose(clue_ptr);
        fprintf(stderr, "Can't create %s.\n", "solution file");
        return 3;
    }

    // Read infile' BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, clue_ptr);

    // Read infile BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, clue_ptr);

    // Check if file is a 24-bit uncompressed BMP
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(solution_ptr);
        fclose(clue_ptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // Write outfile BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, solution_ptr);

    // Write outfile BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, solution_ptr);

    // Padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // Iterate over infile' scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // Iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // Create temporary storage
            RGBTRIPLE triple;
            // Read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, clue_ptr);
            // Write it to outfile
            if (triple.rgbtRed == 255)
            {
                triple.rgbtRed = 0;
                triple.rgbtBlue = 100;
                triple.rgbtGreen = 60;
            }
            fwrite(&triple, sizeof(RGBTRIPLE), 1, solution_ptr);
        }

        // Skip over padding, if any...
        fseek(clue_ptr, padding, SEEK_CUR);
        // ...and  add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, solution_ptr);
        }
    }

    // Close infile
    fclose(clue_ptr);
    // Close outfile
    fclose(solution_ptr);

    return 0;
}