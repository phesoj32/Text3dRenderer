#include "GenericFileLoader.hpp"

#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <algorithm>

namespace GenericFileLoader
{
	uint16_t combineUint8(uint8_t high, uint8_t low) {
		return ((uint16_t)high << 8) | low;
	}

	uint32_t combineUint16(uint16_t high, uint16_t low) {
		return ((uint32_t)high << 16) | low;
	}

	uint16_t encodeUV(float a)
	{
		return (uint16_t)(a * 65535.0f);
	}

	float decodeUV(uint16_t a)
	{
		return a / 65535.0f;
	}


	std::vector<uint8_t> readFileBytes(std::string filePath)
	{
		std::ifstream file(filePath, std::ios::binary | std::ios::ate);
		if (!file)
		{
			throw std::runtime_error("Unable to open file: " + filePath);
		}

		const std::streampos end = file.tellg();
		if (end == std::streampos(-1))
		{
			throw std::runtime_error("Unable to determine file size: " + filePath);
		}

		const std::streamoff fileSize = static_cast<std::streamoff>(end);
		if (fileSize < 0)
		{
			throw std::runtime_error("Invalid file size: " + filePath);
		}

		std::vector<uint8_t> fileData;
		const auto unsignedFileSize = static_cast<std::uintmax_t>(fileSize);
		if (unsignedFileSize > fileData.max_size()
			|| unsignedFileSize > static_cast<std::uintmax_t>(std::numeric_limits<std::streamsize>::max()))
		{
			throw std::length_error("File is too large to read: " + filePath);
		}

		fileData.resize(static_cast<std::size_t>(fileSize));
		file.seekg(0, std::ios::beg);
		if (!file)
		{
			throw std::runtime_error("Unable to seek to the start of file: " + filePath);
		}

		if (!fileData.empty())
		{
			file.read(reinterpret_cast<char*>(fileData.data()), static_cast<std::streamsize>(fileData.size()));
			if (!file)
			{
				throw std::runtime_error("Unable to read file: " + filePath);
			}
		}

		return fileData;
	}
	 
	decodedBMP decodeBMP(std::vector<uint8_t> fileBytes, int resolutionDivisor)
	{
		//Bitmap file header

		decodedBMP bmp;
		int fileP = 0;
		bmp.bfType =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8);

		fileP += 2;


		bmp.bfSize =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8) |
			(static_cast<uint32_t>(fileBytes[fileP + 2]) << 16) |
			(static_cast<uint32_t>(fileBytes[fileP + 3]) << 24);

		fileP += 4;

		bmp.reserved1 =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8);

		fileP += 2;

		bmp.reserved2 =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8);

		fileP += 2;

		bmp.bfOffBits =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8) |
			(static_cast<uint32_t>(fileBytes[fileP + 2]) << 16) |
			(static_cast<uint32_t>(fileBytes[fileP + 3]) << 24);

		fileP += 4;

		//Dib Header

		bmp.biSize =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8) |
			(static_cast<uint32_t>(fileBytes[fileP + 2]) << 16) |
			(static_cast<uint32_t>(fileBytes[fileP + 3]) << 24);

		fileP += 4;

		bmp.biWidth = static_cast<int32_t>(
			fileBytes[fileP] | (fileBytes[fileP + 1] << 8) |
			(fileBytes[fileP + 2] << 16) | (fileBytes[fileP + 3] << 24)
			);
		fileP += 4;

		bmp.biHeight = static_cast<int32_t>(
			fileBytes[fileP] | (fileBytes[fileP + 1] << 8) |
			(fileBytes[fileP + 2] << 16) | (fileBytes[fileP + 3] << 24)
			);
		fileP += 4;

		bmp.biPlane =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8);

		fileP += 2;

		bmp.biBitCount =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8);

		fileP += 2;

		bmp.biCompression =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8) |
			(static_cast<uint32_t>(fileBytes[fileP + 2]) << 16) |
			(static_cast<uint32_t>(fileBytes[fileP + 3]) << 24);

		fileP += 4;

		bmp.biSizeImage =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8) |
			(static_cast<uint32_t>(fileBytes[fileP + 2]) << 16) |
			(static_cast<uint32_t>(fileBytes[fileP + 3]) << 24);

		fileP += 4;

		bmp.biXPelsPerMeter=
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8) |
			(static_cast<uint32_t>(fileBytes[fileP + 2]) << 16) |
			(static_cast<uint32_t>(fileBytes[fileP + 3]) << 24);

		fileP += 4;
		
		bmp.biYPelsPerMeter =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8) |
			(static_cast<uint32_t>(fileBytes[fileP + 2]) << 16) |
			(static_cast<uint32_t>(fileBytes[fileP + 3]) << 24);

		fileP += 4;

		bmp.biClrUsed =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8) |
			(static_cast<uint32_t>(fileBytes[fileP + 2]) << 16) |
			(static_cast<uint32_t>(fileBytes[fileP + 3]) << 24);

		fileP += 4;

		bmp.biClrImportant =
			static_cast<uint32_t>(fileBytes[fileP]) |
			(static_cast<uint32_t>(fileBytes[fileP + 1]) << 8) |
			(static_cast<uint32_t>(fileBytes[fileP + 2]) << 16) |
			(static_cast<uint32_t>(fileBytes[fileP + 3]) << 24);

		fileP += 4;

		int padding = (4 - (bmp.biWidth * 3) % 4) % 4;


		int pixelP = bmp.bfOffBits;

		bmp.pixelArray.reserve(bmp.biWidth* std::abs(bmp.biHeight));

		int absHeight = std::abs(bmp.biHeight);
		for (int i = 0; i < absHeight; i++) {
			for (int j = 0; j < bmp.biWidth; j++) {
				uint8_t b = fileBytes[pixelP++];
				uint8_t g = fileBytes[pixelP++];
				uint8_t r = fileBytes[pixelP++];
				bmp.pixelArray.push_back(pixel24(b, g, r));
			}
			pixelP += padding;
		}

		return bmp;
	}

	pixel24 sampleBMP(const std::vector<pixel24>* pixels, int32_t biHeight, int32_t biWidth, uint16_t* u, uint16_t* v, bool bilinear) {
		// 1. Convert normalized uint16_t (0-65535) to float (0.0-1.0)
		float normU = decodeUV(*u);
		float normV = decodeUV(*v);

		if (!bilinear) {
			// 2. Map to nearest pixel coordinates and clamp to valid image boundaries
			int x = std::clamp((int)std::round(normU * (biWidth - 1)), 0, biWidth - 1);
			int y = std::clamp((int)std::round(normV * (biHeight - 1)), 0, biHeight - 1);

			// 3. Safe 1D array indexing
			return (*pixels)[x + (y * biWidth)];
		}
		else {
			// Bilinear code goes here...
			pixel24 finalPixel(0, 0, 0);
			return finalPixel;
		}
	}

}

