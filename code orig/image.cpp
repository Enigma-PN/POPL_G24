#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

Image::Image(const char* filename)
{
    if (read(filename))  
    {
        printf("Read %s \n", filename);
        size = w * h * channels;
        printf("%d %d %ld \n", w, h, size);
    }
    else
    {
        printf("Error reading file %s \n", filename);
    }
}

Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels)   
{
    size = w * h * channels;
    data = new uint8_t[size];
    memset(data, 0, size);
}

Image::Image(const Image& img) : Image(img.w, img.h, img.channels) // Copy constructor
{
    // data = new uint8_t[size];
    memcpy(data, img.data, size);
}

Image::~Image()
{
    stbi_image_free(data);
}

bool Image::read(const char* filename)
{
    data = stbi_load(filename, &w, &h, &channels, 0);
    return data != NULL;
}

bool Image::write(const char* filename)
{
    ImageType type = getFileType(filename);
    int success = 0; // Initialize success variable
    switch (type)
    {
        case PNG: success = stbi_write_png(filename, w, h, channels, data, w * channels); break;
        case JPG: success = stbi_write_jpg(filename, w, h, channels, data, 100); break;
        case BMP: success = stbi_write_bmp(filename, w, h, channels, data); break;
        case TGA: success = stbi_write_tga(filename, w, h, channels, data); break;
        default:
            printf("Unsupported image type.\n");
    }
    // printf("%d \n", success);
    return success != 0;
}

ImageType Image::getFileType(const char* filename)
{
    const char* ext = strrchr(filename, '.'); // Get the extension of the file
    if (ext != NULL)
    {
        if (strcmp(ext, ".png") == 0)
        {
            return PNG;
        }
        else if (strcmp(ext, ".jpg") == 0)
        {
            return JPG;
        }
        else if (strcmp(ext, ".bmp") == 0)
        {
            return BMP;
        }
        else if (strcmp(ext, ".tga") == 0)
        {
            return TGA;
        }
    }
    return UNKNOWN; // Default assumption of an unknown file type
}

Image& Image :: grayscale_lum(){
    if(channels < 3) {
        printf("Image %p already has less than 3 channels, i.e. it is already assumed to be in greyscal.\n", this);
    }
    else
    {
        for(int i = 0; i < size; i+= channels)
        {
            int gray = 0.2126*data[i] + 0.7152*data[i+1] + 0.0722*data[i+2];
            // int gray = 255 - data[i];
            memset(data + i, gray, 3);
        }
    }
    return *this;  
}

Image& Image :: grayscale_avg(){
    if(channels < 3) {
        printf("Image %p already has less than 3 channels, i.e. it is already assumed to be in greyscale.\n", this);
    }
    else
    {
        for(int i = 0; i < size; i+= channels)
        {
            int gray = (data[i] + data[i+1] + data[i+2])/3;
            memset(data + i, gray, 3);
        }
    }
    return *this;    
}

Image& Image :: colorMask(float r, float g, float b){
    if(channels < 3) {
        printf("\e[31m[ERROR]] Color mask requires atleast 3 channels, but this had %d channels\e[0m\n", channels);
    }
    else
    {
        for(int i = 0; i < size; i+= channels)
        {
            data[i] *= r;
            data[i+1] *= g;
            data[i+2] *= b;
        }
    }
    return *this;
}

Image& Image :: colorSwapRG(){
    if(channels < 3) {
        printf("\e[31m[ERROR]] Color mask requires atleast 3 channels, but this had %d channels\e[0m\n", channels);
    }
    else
    {
        for(int i = 0; i < size; i+= channels)
        {
            uint8_t temp = data[i];
            data[i] = data[i+1];
            data[i+1] = temp;
        }
    }
    return *this;
}

Image& Image :: remRed(){
    if(channels < 3) {
        printf("\e[31m[ERROR]] Color mask requires atleast 3 channels, but this had %d channels\e[0m\n", channels);
    }
    else
    {
        for(int i = 0; i < size; i+= channels)
        {
            if((data[i] > data[i+1]) && (data[i] > data[i+2]))
            {
                uint8_t temp = data[i];
                data[i] = data[i+1];
                data[i+1] = temp;                
            }
        }
    }
    return *this;
}

Image& Image :: encodeMessage(const char* message)
{
    uint32_t len = strlen(message) * 8; // Total number of bits required to store the length value
    if(len + STEG_HEADER_SIZE > size)  // MEssage length + bits required to store the length
    {
        printf("\e[31m[ERROR] This message is too large (%ld bits / %zu bits)\e[0m\n", len+STEG_HEADER_SIZE, size);
        return *this;
    }
    printf("LENGTH %d\n", len);

    for(uint8_t i = 0; i < STEG_HEADER_SIZE; ++i)
    {
        data[i] &= 0xFE;// Case when we want to encode a zero but there is already a 1 as LSB in data
        data[i] |= (len >> (STEG_HEADER_SIZE - 1 - i)) & 1UL;
    }

    for(uint32_t i = 0; i < len; ++i)
    {
        data[i+STEG_HEADER_SIZE] &= 0xFE;// Case when we want to encode a zero but there is already a 1 as LSB in data
        data[i+STEG_HEADER_SIZE] |= (message[i/8] >> ((len - 1 - i)%8)) & 1UL;
    }

    return *this;
}

Image& Image :: decodeMessage(char* buffer, size_t* messageLength)
{
    uint32_t len = 0; // Extracting message length from the image
    for(uint8_t i = 0; i < STEG_HEADER_SIZE; ++i)
    {
        len = (len << 1) | (data[i] & 1UL);
    }
    *messageLength = len/8;

    for(uint32_t i = 0; i < len; ++i)
    {
        buffer[i/8] = (buffer[i/8] << 1) | (data[i + STEG_HEADER_SIZE] & 1);
    }
    // printf("MSG_LENGTH %d\n", len);
    return *this;   
}
