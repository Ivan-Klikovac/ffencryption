#include <stdio.h>
#include <string.h>

void xor(char* data, size_t data_length, char* password)
{
    for(size_t i = 0; i < data_length; i++)
        data[i] ^= password[i % strlen(password)];
}

int main(int argc, char* argv[])
{
    if(argc == 1) // first check this in case no arguments were given
    {
        printf("\nUsage: ffencryption path\\to\\filename encrypt/decrypt password\nRun ffencryption -help for more details\n\n");
        return 0;
    }

    if(strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0)
    {
        printf("\n| File/Folder Encryption\n");
        printf("Usage: ffe path\\to\\filename encrypt/decrypt password\n");
        printf("\n| Details:\n");
        printf("(Folders are not yet supported)\n");
        printf("Encrypted files will have a .ffe extension. If you attempt to decrypt a file without the extension .ffe, FFE will give a warning. If you're sure it's the correct file, just continue.\n");
        printf("If the file to be decrypted has no extension, its name will be appended with _decrypted. If it has an unexpected extension, the extension .dec will be added. In either case, you'll probably have to return the file to its original extension to use it, but the data will be there.\n");
        printf("This program is not exactly meant for very sensitive data.\n");
    }

    else if(argc < 4)
    {
        printf("Usage: ffe path\\to\\filename encrypt/decrypt password\nRun ffe -help for more details\n");
        return 0;
    }

    char path[1024] = {0};
    int cmd = 0;
    char password[1024] = {0};

    strcpy(path, argv[1]);

    if(strcmp(argv[2], "encrypt") == 0)
        cmd = 1;
    else if(strcmp(argv[2], "decrypt") == 0)
        cmd = 0;
    else
    {
        printf("Invalid arguments, check ffencryption -help\n");
        return 0;
    }

    strcpy(password, argv[3]);

    if(cmd == 1) // encrypt
    {


        // copying the file's contents into a new file
        FILE* original_file = fopen(path, "rb");
        if(original_file == NULL)
        {
            printf("Failed to open the file\n");
            return 1;
        }

        // extracting the filename from the path to know what to call the new file
        char* name_ptr = path;
        char filename[128];
        while(strchr(name_ptr, '\\') != NULL)
            name_ptr = strchr(name_ptr, '\\') + 1;

        // got the filename, adding the .ffe extension
        strcpy(filename, name_ptr);
        strcat(filename, ".ffe");

        FILE* encrypted_file = fopen(filename, "wb");

        char buffer[1024];
        size_t bytes_read;
        while((bytes_read = fread(buffer, 1, sizeof(buffer), original_file)) > 0)
        {
            xor(buffer, bytes_read, password);
            fwrite(buffer, 1, bytes_read, encrypted_file);
        }

        printf("Encrypted. Resulting file: %s\n", filename);

        return 0;
    }

    if(cmd == 0) // decrypt
    {
        FILE* encrypted_file = fopen(path, "rb");
        if(encrypted_file == NULL)
        {
            printf("Failed to open the file\n");
            return 1;
        }

        char* name_ptr = path;
        char filename[128];
        while(strchr(name_ptr, '\\') != NULL)
            name_ptr = strchr(name_ptr, '\\') + 1;

        strcpy(filename, name_ptr);
        name_ptr = filename;

        while(strchr(name_ptr, '.') != NULL)
            name_ptr = strchr(name_ptr, '.') + 1;

        if(strchr(filename, '.') == NULL) // if there's no extension at all
        {
            printf("File has no extension. \nContinue decrypting? (Y/N): ");
            char response = getchar();
            if(response != 'Y' && response != 'y') return 1;
            strcat(filename, "_decrypted");
        }

        else if(strcmp(name_ptr - 1, ".ffe") != 0)
        {
            printf("Unexpected extension: %s\nContinue decrypting? (Y/N): ", name_ptr - 1);
            char response = getchar();
            if(response != 'Y' && response != 'y') return 1;
            *(name_ptr - 1) = 0; // only want to do this if it has an extension at all
            strcat(filename, ".dec");
        }

        else *(name_ptr - 1) = 0; // null term at the last dot to erase the .ffe extension


        FILE* decrypted_file = fopen(filename, "wb");

        char buffer[1024];
        size_t bytes_read;
        while((bytes_read = fread(buffer, 1, sizeof(buffer), encrypted_file)) > 0)
        {
            xor(buffer, bytes_read, password);
            fwrite(buffer, 1, bytes_read, decrypted_file);
        }

        printf("Decrypted. Resulting file: %s\n", filename);

        return 0;
    }
}
