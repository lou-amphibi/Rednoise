#include "RNbmp.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
    time_t h;
    int isred;
    int redchance=50;
    srand(time(&h));
    if (argc<3)
    {
        printf("Usage: ./rednoise infile outfile\n");
        exit(1);
    }
    else if (argc==3)
    {
       printf("Default chance is 50\n");
    }
    else if (argc==4)
    {
        redchance=atoi(argv[3]);
        if (redchance>=100 || redchance==0)
        {
            printf("0-99 for redchance. Redchance set to default (50)\n");
            redchance=50;
        }
        else if (redchance==50)
            printf("\x1b[31;1mUSSR\x1b[0m\n");
        else
            printf("Now redchance is %d\n", redchance);
    }
    char* infile = argv[1];
    char* outfile = argv[2];
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        exit(2);
    }

    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        exit(3);
    }

    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        exit(4);
    }
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        for (int j = 0; j < bi.biWidth; j++)
        {
            RGBTRIPLE triple;
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            isred=(rand()%100)+1;
            if (isred==100)
                printf("100!\n");
            if (isred>=1 && isred<=redchance)
            {
                triple.rgbtRed=0xff;
                triple.rgbtBlue=0x00;
                triple.rgbtGreen=0x00;
            }
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }
        fseek(inptr, padding, SEEK_CUR);
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }
    fclose(inptr);
    fclose(outptr);
    printf("\x1b[31;1mRED NOISE\x1b[0m\n");
}

