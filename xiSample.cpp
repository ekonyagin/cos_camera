#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#define CHANNEL_NUM 3
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#include <memory.h>
#include <string>
#include "cos_camera.h"

<<<<<<< HEAD
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

=======
>>>>>>> parent of ffeb664... saving through opencv enabled
int main(int argc, char* argv[])
{
	Camera cam = Camera();
	
	//printf("User API: height is %d, width is %d\n", height, width);
	int height = cam.GetHeight(), width = cam.GetWidth();
	uint8_t * pixels_corrected = (uint8_t * )malloc(height * width * CHANNEL_NUM * sizeof(uint8_t));
	cam.Start();
<<<<<<< HEAD
	//printf("height is %d, width is %d\n", height, width);

	for (int images=0; images < 100; images++)
=======
	printf("height is %d, width is %d\n", height, width);
	for (int images=0; images < 5; images++)
>>>>>>> parent of ffeb664... saving through opencv enabled
	{
		// getting image from camera
		
		cam.GetFrame(pixels_corrected);
		// see https://www.ximea.com/support/gfiles/buffer_policy_in_xiApi.png to correct image reading!
<<<<<<< HEAD
		struct ImwriteArgs write_args;
		write_args.number = images;
		write_args.img = &img;
		WriteImg((void*)&write_args);
		//printf("Image %d (%dx%d) received from camera.\n", images, width, height);
=======
		
		std::string fname = "image_" + std::to_string(images) + ".png";
		const char *filename = fname.c_str();
		stbi_write_png(filename, width, height, CHANNEL_NUM, pixels_corrected, width * CHANNEL_NUM);
		printf("Image %d (%dx%d) received from camera.\n", images, width, height);
>>>>>>> parent of ffeb664... saving through opencv enabled
	}
	cam.Stop();
	free(pixels_corrected);
	
	printf("Done\n");
	return 0;
}

