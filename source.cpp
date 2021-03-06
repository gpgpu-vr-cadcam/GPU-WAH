
#include "compress.h"
#include "decompress.h"
#include <stdlib.h>
#include <stdio.h>
#include "tests.h"
#include <fstream>
#include <iostream>

//int main()
//{
////	warpCompressionTest();
////	blockCompressionTest();
////	blockMergeTest();
////	blockMergeWithOnesStartsTest();
////	blockMergeAlternatingTest();
////	blockMergeFinalLiterals();
////	blockMergeWanderingLiterals();
////	multiBlockTest();
////	compressAndDecompressTest();
////	zerosTest();
//
//	randomDataTest();
//	return 0;
//}

int main(){
  std::fstream fs;
  fs.open ("results.txt", std::fstream::out | std::fstream::app);
  // write headers
	fs << "Original size [Int] , ";
	fs << "Compressed size [Int] , ";
	fs << "Decompressed size [Int] , ";
	fs << "Density, ";
	fs << "Compression Ratio, ";
	fs << "Compression transfer to device [ms], ";
	fs << "Compression time [ms], ";
	fs << "Compression transfer from device [ms], ";
	fs << "Decompression transfer to device [ms], ";
	fs << "Decompression time [ms],";
	fs << "Decompression transfer from device [ms]" << std::endl;
	for (int s=1;s<=256;s<<=1) {
		for (unsigned int i=1;i<=16;i++) {
			float c_globalTransferToDevice = 0, c_globalTransferFromDevice = 0, c_globalCompression = 0;
			float d_globalTransferToDevice = 0, d_globalTransferFromDevice = 0 , d_globalDecompression = 0;
			unsigned long long int dataSize = s*1024*31*32;
			unsigned int* dataset = (unsigned int*)malloc((dataSize+1) * sizeof(int));
			generateRandomData(dataset, dataSize, 1<<i);
			int globalCompressedSize, globalDecompressedSize;
			int repetition = 10;
			for(int repeat = 0; repeat < repetition; repeat++){
				float c_transferToDevice, c_transferFromDevice, c_compression;
				float d_transferToDevice, d_transferFromDevice, d_decompression;
				unsigned long long int compressedSize, decompressedSize;
				unsigned int* compressed = compress(dataset, dataSize, &compressedSize, &c_transferToDevice, &c_compression, &c_transferFromDevice);
				unsigned int* decompressed = decompress(compressed, compressedSize, &decompressedSize, &d_transferToDevice, &d_decompression, &d_transferFromDevice);
				ASSERT(decompressed, dataset, dataSize);
				free(compressed);
				std::cout << "data matches" << std::endl;
				std::cout << " s: " << s << " i: " << i <<std::endl;
				c_globalCompression += c_compression;
				c_globalTransferFromDevice += c_transferFromDevice;
				c_globalTransferToDevice += c_transferToDevice;
				d_globalDecompression += d_decompression;
				d_globalTransferFromDevice += d_transferFromDevice;
				d_globalTransferToDevice += d_transferToDevice;
				globalCompressedSize = compressedSize;
				globalDecompressedSize = decompressedSize;
				free(decompressed);
			}
			fs << dataSize << ",";
			fs << globalCompressedSize << ", ";
			fs << globalDecompressedSize << ", ";
			fs << i << ", ";
			fs << ((float)globalCompressedSize)/((float)dataSize) << ", ";
			fs << c_globalTransferToDevice / repetition << ", ";
			fs << c_globalCompression / repetition << ", ";
			fs << c_globalTransferFromDevice / repetition << ", ";
			fs << d_globalTransferToDevice / repetition << ", ";
			fs << d_globalDecompression / repetition << ", ";
			fs << d_globalTransferFromDevice / repetition << std::endl;
			free(dataset);
		}
	}
	fs.close();
	return 0;
}
