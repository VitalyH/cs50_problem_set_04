#include <stdio.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
    // Check correctness of the user input
    if (argc != 2)
    {
        fprintf(stderr, "Error! Try: ./recover file_name\n");
        return 1;
    }

    // Get name of the input file
    char *cardRaw = argv[1];

    // Check if we can open that file
    FILE *card;
    if ((card = fopen(cardRaw, "r")) == NULL)
    {
        fprintf(stderr, "Couldn't open %s\n", argv[1]);
        return 2;
    }

    // Variables
    // A counter for recovered images
    int iCounter = 0;
    // An array for bytes
    unsigned char buffer[512];
    // An array for images names
    char filename[8];
    // An indicator to make sure we know whatever we
    // opened the file or not
    bool fOpen = false;
    // Temporary storage for an images
    FILE *img;

    // Condition for non-end of input file
    while (fread(buffer, 512, 1, card) != 0)
    {
        // Check if block of bytes is the beggining of JPEG
        if (buffer[0] == 0xff && buffer[1] == 0xd8
            && buffer[2] == 0xff && ((buffer[3]
                                      & 0xf0) == 0xe0))
        {
            // If the file is open - close it
            if (fOpen)
            {
                fclose(img);
            }

            sprintf(filename, "%03i.jpg", iCounter);
            iCounter++;
            img = fopen(filename, "w");
            fOpen = true;
            fwrite(buffer, 512, 1, img);
        }

        // If not - keep writing
        else
        {
            if (fOpen)
            {
                fwrite(buffer, 512, 1, img);
            }
        }
    }

    // Close all
    fclose(card);
    fclose(img);

    // Done!
    return 0;
}