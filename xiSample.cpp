#include <stdio.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <memory.h>
#include <string>

#include <time.h>
#include <omp.h>

#include <fstream>

#define CHANNEL_NUM 1
#define BUFFER_SIZE 8

#include "cos_camera.h"

struct ImwriteArgs{
	int number;
	int format;
	int height;
	int width;
	int cam_nr;
	uint8_t * pixels_corrected;
};

void* WriteImg(void* args){
	struct ImwriteArgs * write_args = (struct ImwriteArgs*)args;
	std::string fname = "image_" + std::to_string(write_args->cam_nr)+\
			"_"+ std::to_string(write_args->number) + ".png";
	const char *filename = fname.c_str();
	
	cv::Mat image_to_write(write_args->height, write_args->width,\ 
		CV_8UC1, (void*)write_args->pixels_corrected);
	cv::imwrite(filename, image_to_write);
	return NULL;
}

int GetCamerasNumber(){
	json cfg;
	std::ifstream i("cameras_description.json");
	i >> cfg;
	return (int)cfg["n_cameras"];
}

const int GetFramesNumber(const int buffer_size){
	json cfg;
	std::ifstream i("cameras_description.json");
	i >> cfg;
	int n_images = (int)cfg["n_images"];
	return n_images/buffer_size;
}

int main(int argc, char* argv[])
{
	const int N_CAMERAS = GetCamerasNumber();
	printf("USER_API: n_cameras is %d\n", N_CAMERAS);
	
	omp_set_num_threads(BUFFER_SIZE*N_CAMERAS);
	
	Camera * cam[N_CAMERAS];
	uint8_t ** pixels_corrected[N_CAMERAS];
	uint8_t ** pixels_corrected_old[N_CAMERAS];

	struct ImwriteArgs write_args[N_CAMERAS][BUFFER_SIZE];
	struct ImwriteArgs write_args_old[N_CAMERAS][BUFFER_SIZE];

	int height[N_CAMERAS], width[N_CAMERAS];
	int img_size[N_CAMERAS];
	
	for(int N = 0; N < N_CAMERAS; N++){
		cam[N] = new Camera(N);


		height[N] = cam[N]->GetHeight(), width[N] = cam[N]->GetWidth();
		img_size[N] = height[N] * width[N] * CHANNEL_NUM * sizeof(uint8_t);

		pixels_corrected[N] = (uint8_t**)malloc(BUFFER_SIZE * sizeof(uint8_t*));
		pixels_corrected_old[N] = (uint8_t**)malloc(BUFFER_SIZE * sizeof(uint8_t*));
		
		for (int i = 0; i < BUFFER_SIZE; i++){
			pixels_corrected[N][i] = (uint8_t * )malloc(img_size[N]);
			pixels_corrected_old[N][i] = (uint8_t * )malloc(img_size[N]);
		}
		
		
		for (int i = 0; i < BUFFER_SIZE; i++){
			write_args_old[N][i].height = height[N];
			write_args_old[N][i].width = width[N];
			write_args_old[N][i].cam_nr = N;
		}
		//printf("Allocated successfully!\n");
		cam[N]->Start();
	}
	
	
	int N_IMG = GetFramesNumber(BUFFER_SIZE);
	printf("USER_API: n_frames is %d\n", N_IMG);
	//Recording part
	//printf("Starting rec!\n");
	double start = omp_get_wtime();
	
	for (int images=0; images < N_IMG; images++){	
		
		for(int N = 0; N < N_CAMERAS; N++){
			for (int i = 0; i < BUFFER_SIZE; i++){
				//printf("Entered rec loop! N is %d, i is %d\n",N, i);
				cam[N]->GetFrame(pixels_corrected_old[N][i]);
				//printf("Got picture from camera!\n");
				write_args_old[N][i].number = images * BUFFER_SIZE + i;
				write_args_old[N][i].pixels_corrected = pixels_corrected_old[N][i];
			}
		}
		//printf("Exited rec loop!\n");

		#pragma omp barrier
		
		#pragma omp parallel
		{
			#pragma omp for nowait collapse(2)
			for(int N = 0; N < N_CAMERAS; N++){
				for (int i = 0; i < BUFFER_SIZE; i++){
					//printf("Entered writing loop!\n");
					memcpy((void*)pixels_corrected[N][i], (void*)pixels_corrected_old[N][i], img_size[N]);
					//printf("Copying OK!\n");
					memcpy((void*)&write_args[N][i], (void*)&write_args_old[N][i], sizeof(ImwriteArgs));
					//printf("Copying-2 OK!\n");
					write_args[N][i].pixels_corrected = pixels_corrected[N][i];
					WriteImg((void*)&write_args[N][i]);
				} 
			}	
		}		
	}

	#pragma omp barrier
	//Recording ends here
	
	double end = omp_get_wtime();
	
	for(int N = 0; N < N_CAMERAS; N++){
		cam[N]->Stop();
	}
	
	printf("USER_API: Time elapsed (png): %f seconds.\n", end-start);
	
	for (int N = 0; N < N_CAMERAS; N++){
		for (int i = 0; i < BUFFER_SIZE; i++){
			free(pixels_corrected[N][i]);
		}
		free(pixels_corrected[N]);
		delete cam[N];
	}
	
	printf("USER_API: Done!\n");
	return 0;
}