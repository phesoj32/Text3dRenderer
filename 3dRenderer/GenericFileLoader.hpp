#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <fstream>

namespace GenericFileLoader
{
	#pragma pack(push, 1)
	struct pixel24
	{
		uint8_t blue;
		uint8_t green;
		uint8_t red;

		pixel24(uint8_t Blue, uint8_t Green, uint8_t Red)
		{
			blue = Blue;
			green = Green;
			red = Red;
		}
	};
	#pragma pack(pop)

	struct decodedBMP
	{
		//BITMAPFILEHEADER
		uint16_t bfType;
		uint32_t bfSize;
		uint16_t reserved1;
		uint16_t reserved2;
		uint32_t bfOffBits;
		//BITMAPINFOHEADER
		uint32_t biSize;
		int32_t biWidth;
		int32_t biHeight;
		uint16_t biPlane;
		uint16_t biBitCount;
		uint32_t biCompression;
		uint32_t biSizeImage;
		uint32_t biXPelsPerMeter;
		uint32_t biYPelsPerMeter;
		uint32_t biClrUsed;
		uint32_t biClrImportant;

		std::vector<pixel24> pixelArray;

	};

	uint16_t combineUint8(uint8_t high, uint8_t low);
	uint32_t combineUint16(uint16_t high, uint16_t low);

	uint16_t encodeUV(float a);
	float decodeUV(uint16_t a);

	std::vector<uint8_t> readFileBytes(std::string filePath);

	decodedBMP decodeBMP(std::vector<uint8_t> fileBytes, int resolutionDivisor);

	pixel24 sampleBMP(const std::vector<pixel24>* pixels, int32_t biHeight, int32_t biWidth, uint16_t* u, uint16_t* v, bool bilinear);

	
}