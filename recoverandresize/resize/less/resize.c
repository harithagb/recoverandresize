// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile\n");
        return 1;
    }

    int n = atoi(argv[1]);

    if (n <  0 || n > 100)
    {
        printf("Resize scale must be positive and between 0 to 100\n");
        return 5;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf, bfNew;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    //new file header:
    bfNew = bf;

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi, biNew;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    //new info header:
    biNew = bi;

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    //Write new characteristics of outfile:
    biNew.biWidth = bi.biWidth * n;
    biNew.biHeight = bi.biHeight * n;

    //find infile's padding
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    //find new padding to be written:
    int newPadding = (4 - (biNew.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    //new file sizes:
    biNew.biSizeImage = ((sizeof(RGBTRIPLE) * biNew.biWidth) + newPadding) * abs(biNew.biHeight);
    bfNew.bfSize = biNew.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bfNew, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&biNew, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        for (int h = 0; h < n; h++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile, loop it
                int r = 0;
                while (r < n)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    r++;
                }
            }
            // skip over padding, if any
            fseek(inptr, padding, SEEK_CUR);

            // then add it back (to demonstrate how)
            for (int k = 0; k < newPadding; k++)
            {
                fputc(0x00, outptr);
            }
            //reading row m again
            if (h != n - 1)
            {
                fseek(inptr, -(bi.biWidth * sizeof(RGBTRIPLE) + padding), SEEK_CUR);
            }
        }

    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
