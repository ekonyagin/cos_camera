#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#define CHANNEL_NUM 3
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#include <memory.h>
#include <string>
#include "cos_camera.h"

int main(int argc, char* argv[])
{
	Camera cam = Camera();
	
	//printf("User API: height is %d, width is %d\n", height, width);
	int height = cam.GetHeight(), width = cam.GetWidth();
	uint8_t * pixels_corrected = (uint8_t * )malloc(height * width * CHANNEL_NUM * sizeof(uint8_t));
	cam.Start();
	printf("height is %d, width is %d\n", height, width);
	for (int images=0; images < 5; images++)
	{
		// getting image from camera
		
		cam.GetFrame(pixels_corrected);
		// see https://www.ximea.com/support/gfiles/buffer_policy_in_xiApi.png to correct image reading!
		
		std::string fname = "image_" + std::to_string(images) + ".png";
		const char *filename = fname.c_str();
		stbi_write_png(filename, width, height, CHANNEL_NUM, pixels_corrected, width * CHANNEL_NUM);
		printf("Image %d (%dx%d) received from camera.\n", images, width, height);
	}
	cam.Stop();
	free(pixels_corrected);
	
	printf("Done\n");
	return 0;
}

