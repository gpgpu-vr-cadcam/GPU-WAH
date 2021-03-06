#include "decompress.h"
#include "kernels.h"
#include <thrust/scan.h>
#include <thrust/device_ptr.h>
#include "timeMeasuring.h"

unsigned int* decompress(
		unsigned int* data,
		unsigned long long int dataSize,
		unsigned long long int* outSize,
		float* pTransferToDeviceTime,
		float* pCompressionTime,
		float* ptranserFromDeviceTime){

	// times to be measured
	float transferToDeviceTime;
	float compressionTime;
	float transferFromDeviceTime;

	// start measuring time
	CREATE_TIMER
	START_TIMER

	unsigned int *data_gpu, *result_gpu, *finalOutput_gpu, *output_cpu;
	unsigned long long int* counts_gpu;
	int blockCount = dataSize / 1024;

	if(dataSize % 1024 > 0){
		blockCount++;
	}
	cudaMalloc((void**)&data_gpu, sizeof(int)*dataSize);
	cudaMalloc((void**)&counts_gpu, sizeof(unsigned long long int)*dataSize);
	cudaMemcpy(data_gpu, data, sizeof(int)*dataSize, cudaMemcpyHostToDevice);

	STOP_TIMER
	GET_RESULT(transferToDeviceTime)
	START_TIMER
//	counts_cpu = (unsigned int*) malloc(sizeof(int)*dataSize);
	dim3 blockDim(32, 32);
	// get blocked sizes
	getCounts<<<blockCount,blockDim>>>(data_gpu, counts_gpu, dataSize);
	unsigned long long int lastBlockSize;
	cudaMemcpy(&lastBlockSize, counts_gpu  + (dataSize - 1), sizeof(long long int), cudaMemcpyDeviceToHost);
	// scan block sizes
	thrust::device_ptr<unsigned long long int> countsPtr(counts_gpu);
	// get counts
	thrust::exclusive_scan(countsPtr, countsPtr + dataSize, countsPtr);
	unsigned long long int lastOffset;
//	thrust::inclusive_scan(counts_cpu, counts_cpu + dataSize, counts_cpu);
	cudaMemcpy(&lastOffset, counts_gpu + (dataSize - 1), sizeof(long long int), cudaMemcpyDeviceToHost);
	unsigned long long int outputSize = lastBlockSize + lastOffset;
	unsigned long long int realSize = 31*outputSize;

	if(realSize % 32 > 0){
		realSize /=32;
		realSize++;
	}
	else{
		realSize /=32;
	}
	SAFE_ASSIGN(outSize, realSize);
//	free(counts_cpu);
	cudaMalloc((void**)&result_gpu, sizeof(int) * outputSize);

	decompressWords<<<blockCount,blockDim>>>(data_gpu, counts_gpu, result_gpu, dataSize);
	cudaFree(data_gpu);
	cudaFree(counts_gpu);

	blockCount = outputSize / 1024;
	if(dataSize % 1024 > 0){
		blockCount++;
	}

	cudaMalloc((void**)&finalOutput_gpu, sizeof(int)*outputSize);
	mergeWords<<<blockCount,blockDim>>>(result_gpu, finalOutput_gpu, outputSize);
	cudaFree(result_gpu);

	STOP_TIMER
	GET_RESULT(compressionTime)
	START_TIMER

	output_cpu = (unsigned int*)malloc(sizeof(int) * outputSize);
	cudaMemcpy(output_cpu, finalOutput_gpu, sizeof(int) * outputSize, cudaMemcpyDeviceToHost);
	cudaFree(finalOutput_gpu);
	STOP_TIMER
	GET_RESULT(transferFromDeviceTime)

	SAFE_ASSIGN(pCompressionTime, compressionTime);
	SAFE_ASSIGN(pTransferToDeviceTime, transferToDeviceTime);
	SAFE_ASSIGN(ptranserFromDeviceTime, transferFromDeviceTime);

	return output_cpu;
}
