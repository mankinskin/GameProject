#include "image.h"
#include <glm.hpp>
#include <png.h>
#include <SOIL.h>
#include <cstring>

unsigned int getChannels(png_byte type)
{
    if (type == PNG_COLOR_TYPE_GRAY || type == PNG_COLOR_TYPE_PALETTE) {
        return 1;
    }
    else if (type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        return 2;
    }
    else if (type == PNG_COLOR_TYPE_RGB) {
        return 3;
    }
    else if (type == PNG_COLOR_TYPE_RGBA) {
        return 4;
    }
    return -1;
}

png_byte getColorType(unsigned int channels)
{
    if (channels == 1) {
        return PNG_COLOR_TYPE_GRAY;
    }
    else if (channels == 2) {
        return PNG_COLOR_TYPE_GRAY_ALPHA;
    }
    else if (channels == 3) {
        return PNG_COLOR_TYPE_RGB;
    }
    else if (channels == 4) {
        return PNG_COLOR_TYPE_RGBA;
    }
    return -1;
}

Image::Image(std::string pFilename)
{
    read(pFilename);
}

Image::Image(unsigned int pWidth, unsigned int pHeight, 
        unsigned int pChannels, unsigned int pBitDepth, unsigned char* pData)
    :width(pWidth), height(pHeight), channels(pChannels), bit_depth(pBitDepth)
{
    unsigned int size = channels * width * height;
    pixels = (unsigned char*)malloc(size);
    std::memcpy(pixels, pData, size);	
}

void Image::read(std::string pFilename)
{
    FILE* file = fopen(pFilename.c_str(), "rb");
    read(file);
    fclose(file);
}

void Image::read(FILE* file)
{
    if (!file) {
        puts("invalid file pointer!");
        return;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        puts("Failed to read PNG!");
        return;
    }
    png_infop info = png_create_info_struct(png);
    setjmp(png_jmpbuf(png)); 
    png_init_io(png, file);

    png_read_info(png, info);
    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    channels = getChannels(color_type);
    png_byte bit_depth = png_get_bit_depth(png, info);

    png_read_update_info(png, info);

    unsigned int row_size = png_get_rowbytes(png, info);

    png_bytep* rows = (png_bytep*)malloc(sizeof(png_bytep) * height);

    pixels = (unsigned char*)malloc(sizeof(png_byte) * height * row_size);

    for (unsigned int r = 0; r < height; ++r) {
        rows[r] = pixels + row_size * r;
    }

    png_read_image(png, rows);
    pixels = rows[0];
    fclose(file);
}

void Image::write(std::string pFilename)
{
    FILE* file = fopen(pFilename.c_str(), "wb");
    write(file);
    fclose(file);
}

void Image::write(FILE* file)
{
    if (!file) {
        puts("Invalid file pointer!");
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        printf("Failed to write PNG!");
        return;
    }

    png_infop info = png_create_info_struct(png);
    setjmp(png_jmpbuf(png));
    png_init_io(png, file);

    png_set_IHDR(
            png, info, width, height, 
            (png_byte)bit_depth, getColorType(channels),
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT);

    png_set_compression_level(png, 0);
    png_write_info(png, info);

    unsigned int row_size = png_get_rowbytes(png, info);

    png_bytep* rows = (png_bytep*)malloc(sizeof(png_bytep) * height);

    for (unsigned int r = 0; r < height; ++r) {
        rows[ r ] = (png_bytep)&pixels[ row_size * r ];
    }

    png_write_image(png, rows);
    png_write_end(png, NULL);
    fclose(file);
}

Image::~Image()
{
    free(pixels);
}

