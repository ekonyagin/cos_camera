#include <stdio.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <memory.h>
#include <string>

#include <time.h>
#include <omp.h>

#define CHANNEL_NUM 3
#define BUFFER_SIZE 8

#include "cos_camera.h"

struct ImwriteArgs{
	int number;
	int format;
	int height;
	int width;
	uint8_t * pixels_corrected;
};

void* WriteImg(void* args){
	struct ImwriteArgs * write_args = (struct ImwriteArgs*)args;
	std::string fname = "image_" + std::to_string(write_args->number) + ".png";
	const char *filename = fname.c_str();
	cv::Mat image_to_write(write_args->height, write_args->width, CV_8UC1, (void*)write_args->pixels_corrected);
	cv::imwrite(filename, image_to_write);
	return NULL;
}

int main(int argc, char* argv[])
{
	omp_set_num_threads(BUFFER_SIZE);

	Camera cam = Camera();
	
	int height = cam.GetHeight(), width = cam.GetWidth();
	
	uint8_t ** pixels_corrected = (uint8_t**)malloc(BUFFER_SIZE * sizeof(uint8_t*));
	uint8_t ** pixels_corrected_old = (uint8_t**)malloc(BUFFER_SIZE * sizeof(uint8_t*));
	
	for (int i = 0; i < BUFFER_SIZE; i++){
		pixels_corrected[i] = (uint8_t * )malloc(height * width * CHANNEL_NUM * sizeof(uint8_t));
		pixels_corrected_old[i] = (uint8_t * )malloc(height * width * CHANNEL_NUM * sizeof(uint8_t));
	}
	struct ImwriteArgs write_args[BUFFER_SIZE];
	struct ImwriteArgs write_args_old[BUFFER_SIZE];
	
	for (int i = 0; i < BUFFER_SIZE; i++){
		write_args_old[i].height = height;
		write_args_old[i].width = width;
	}
	
	
	cam.Start();
	//printf("height is %d, width is %d\n", height, width);
	int N_IMG = 100/BUFFER_SIZE;
	double start = omp_get_wtime();
	for (int images=0; images < N_IMG; images++){	
		for (int i = 0; i < BUFFER_SIZE; i++){
			cam.GetFrame(pixels_corrected_old[i]);
			write_args_old[i].number = images * BUFFER_SIZE + i;
			write_args_old[i].pixels_corrected = pixels_corrected_old[i];
		}
		#pragma omp barrier
		
		#pragma omp parallel
		{
			#pragma omp for nowait
			for (int i = 0; i < BUFFER_SIZE; i++){
				memcpy((void*)pixels_corrected[i], (void*)pixels_corrected_old[i], height * width * CHANNEL_NUM * sizeof(uint8_t));
				memcpy((void*)&write_args[i], (void*)&write_args_old[i], sizeof(ImwriteArgs));
				write_args[i].pixels_corrected = pixels_corrected[i];
				WriteImg((void*)&write_args[i]);
			} 
		}		
	}
	#pragma omp barrier
	double end = omp_get_wtime();
	cam.Stop();
	printf("Time elapsed (png): %f seconds.\n", end-start);
	
	for (int i = 0; i < BUFFER_SIZE; i++){
		free(pixels_corrected[i]);
	}
	free(pixels_corrected);
	printf("Done\n");
	return 0;
}