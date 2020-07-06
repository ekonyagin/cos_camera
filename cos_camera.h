#pragma once
					
#include <stdio.h>
#include <m3api/xiApi.h> // Linux, OSX
#include <memory.h>
#include <string.h>
#include <string>

#include <nlohmann/json.hpp>
#include <fstream> 

using json = nlohmann::json;

class Camera{
private:
	XI_IMG image;
	HANDLE xiH = NULL;
	int height, width;
	
public:
	Camera(){
		memset(&image, 0, sizeof(image));
		image.size = sizeof(XI_IMG);
		
		printf("Starting config..\n");
		ConfigureCamera();
	}
	void ConfigureCamera(){
		xiOpenDevice(0, &xiH);
		std::ifstream i("camera_cfg.json");
		json cfg;
		i >> cfg;

		int exposure = (int)cfg["exposure"];
		int gain = (int)cfg["gain"];
		int framerate = (int)cfg["framerate"];
		width = (int)cfg["width"];
		height = (int)cfg["height"];
		int offset_x = (int)cfg["offset_X"];
		int offset_y = (int)cfg["offset_Y"];
		int img_data_format = (int)cfg["rgb"];
		bool auto_wb = (int)cfg["auto_wb"];

		xiSetParamInt(xiH, XI_PRM_EXPOSURE, exposure);
		xiSetParamInt(xiH, XI_PRM_GAIN, gain);
		
		if (img_data_format == 1) {
			xiSetParamInt(xiH, XI_PRM_IMAGE_DATA_FORMAT, XI_RGB24);
			xiSetParamInt(xiH, XI_PRM_AUTO_WB, auto_wb);
		} else
			xiSetParamInt(xiH, XI_PRM_IMAGE_DATA_FORMAT, XI_RAW8);
		
		xiSetParamInt(xiH, XI_PRM_WIDTH, width);
		xiSetParamInt(xiH, XI_PRM_OFFSET_X, offset_x);

		xiSetParamInt(xiH, XI_PRM_HEIGHT, height);
		xiSetParamInt(xiH, XI_PRM_OFFSET_Y, offset_y);

		xiSetParamInt(xiH,XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FRAME_RATE);
		// set frame rate
		xiSetParamInt(xiH,XI_PRM_FRAMERATE,framerate);
	}
	void Start(){
		printf("Starting acquisition...\n");
		xiStartAcquisition(xiH);
	}
	void GetFrame(uint8_t* pixels_corrected){
		xiGetImage(xiH, 5000, &image);
		uint8_t * pixels = (uint8_t*)image.bp;
		memcpy(pixels_corrected, pixels, GetHeight() * GetWidth() * CHANNEL_NUM * sizeof(uint8_t));
	}
	void Stop(){
		printf("Stopping acquisition...\n");
		xiStopAcquisition(xiH);
	}

	int GetHeight(){ return height;}
	int GetWidth(){ return width;}
	~Camera(){
		xiCloseDevice(xiH);
	}
};




