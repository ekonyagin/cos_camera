#include <stdio.h>
#define CHANNEL_NUM 3

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <memory.h>
#include <string>
#include "cos_camera.h"

#include <omp.h>

using namespace cv;

int main(int argc, char* argv[])
{
	Camera cam = Camera();
	
	//printf("User API: height is %d, width is %d\n", height, width);
	int height = cam.GetHeight(), width = cam.GetWidth();
	uint8_t * pixels_corrected = (uint8_t * )malloc(height * width * CHANNEL_NUM * sizeof(uint8_t));
	cam.Start();
	printf("height is %d, width is %d\n", height, width);
	double start = omp_get_wtime();
	for (int images=0; images < 100; images++)
	{
		// getting image from camera
		
		cam.GetFrame(pixels_corrected);
		Mat img(height, width, CV_8UC3, (void*)pixels_corrected);
		// see https://www.ximea.com/support/gfiles/buffer_policy_in_xiApi.png to correct image reading!
		
		std::string fname = "image_" + std::to_string(images) + ".png";
		const char *filename = fname.c_str();
		imwrite(filename, img);
		printf("Image %d (%dx%d) received from camera.\n", images, width, height);
	}
	double end = omp_get_wtime();
	double delta = end -start;
	printf("Total time to save: %f\n", delta);
	cam.Stop();
	free(pixels_corrected);
	
	printf("Done\n");
	return 0;
}

