#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    //variable declaration:
    unsigned char buffer[512];
    char jpegName[8];
    int jpegCounter = 0;

    //checking proper usage:
    if (argc != 2)
    {
        printf("Usage: ./recover filename");
        return 1;
    }
    char *infile = argv[1];
    //Step 1: Open the file.
    FILE *inptr = fopen(infile, "r");
    FILE *outptr = NULL;
    if (inptr == NULL)
    {
        printf("Could not open file\n");
        return 2;
    }
    //Step 2: Read the memory card:
    while (fread(buffer, sizeof(buffer), 1, inptr))
    {
        //Step 3: detect the beginning of the jpeg:
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            if (NULL != outptr)
            {
                fclose(outptr);
                outptr = NULL;
            }

            //generating filename into jpegName:
            sprintf(jpegName, "%03d.jpg", jpegCounter);

            //open file as defined by sprintf:
            outptr = fopen(jpegName, "wb");

            //checking for successful open:
            if (outptr == NULL)
            {
                printf("Could not open output file\n");
                return 3;
            }
            //Step 4: write into the outptr file
            fwrite(buffer, sizeof(buffer), 1, outptr);

            //Increment the jpeg counter:
            jpegCounter++;
        }
        else if (NULL != outptr)
        {
            fwrite(buffer, sizeof(buffer), 1, outptr);
        }
    }
    if (NULL != outptr)
    {
        fclose(outptr);
    }
    if (NULL != inptr)
    {
        fclose(inptr);
    }
    return 0;

}
