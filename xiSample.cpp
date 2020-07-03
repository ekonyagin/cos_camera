#include <stdio.h>
#define CHANNEL_NUM 3

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <memory.h>
#include <string>
#include "cos_camera.h"

using namespace cv;

struct ImwriteArgs{
	int number;
	int format;
	Mat * img;
};

void* WriteImg(void* args){
	struct ImwriteArgs * write_args = (ImwriteArgs*)args;
	std::string fname = "image_" + std::to_string(write_args->number) + ".jpg";
	const char *filename = fname.c_str();
	imwrite(filename, *write_args->img);
	return NULL;
}

int main(int argc, char* argv[])
{
	Camera cam = Camera();
	
	//printf("User API: height is %d, width is %d\n", height, width);
	int height = cam.GetHeight(), width = cam.GetWidth();
	uint8_t * pixels_corrected = (uint8_t * )malloc(height * width * CHANNEL_NUM * sizeof(uint8_t));
	cam.Start();
	//printf("height is %d, width is %d\n", height, width);

	for (int images=0; images < 100; images++)
	{
		// getting image from camera
		
		cam.GetFrame(pixels_corrected);
		Mat img(height, width, CV_8UC3, (void*)pixels_corrected);
		// see https://www.ximea.com/support/gfiles/buffer_policy_in_xiApi.png to correct image reading!
		struct ImwriteArgs write_args;
		write_args.number = images;
		write_args.img = &img;
		WriteImg((void*)&write_args);
		//printf("Image %d (%dx%d) received from camera.\n", images, width, height);
	}
	cam.Stop();
	free(pixels_corrected);
	
	printf("Done\n");
	return 0;
}

