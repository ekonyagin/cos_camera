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
	Camera();
	void ConfigureCamera();
	void Start();
	void GetFrame(uint8_t *);
	void Stop();
	int GetHeight();
	int GetWidth();
	~Camera(){
		xiCloseDevice(xiH);
	}
};

Camera::Camera(){
	memset(&image, 0, sizeof(image));
	image.size = sizeof(XI_IMG);
	
	printf("Starting config..\n");
	ConfigureCamera();
}

int Camera::GetHeight(){ return height;}
int Camera::GetWidth(){ return width;}

void Camera::ConfigureCamera(){
	
	xiOpenDevice(0, &xiH);
	std::ifstream i("camera_cfg.json");
	json cfg;
	i >> cfg;

	int exposure = (int)cfg["exposure"]; 
	width = (int)cfg["width"];
	height = (int)cfg["height"];
	int offset_x = (int)cfg["offset_X"];
	int offset_y = (int)cfg["offset_Y"];
	int img_data_format;
	bool auto_wb = (int)cfg["auto_wb"];

	xiSetParamInt(xiH, XI_PRM_EXPOSURE, exposure);
	xiSetParamInt(xiH, XI_PRM_IMAGE_DATA_FORMAT, XI_RGB24);
	xiSetParamInt(xiH, XI_PRM_AUTO_WB, auto_wb);
	
	xiSetParamInt(xiH, XI_PRM_WIDTH, width);
	xiSetParamInt(xiH, XI_PRM_OFFSET_X, offset_x);

	xiSetParamInt(xiH, XI_PRM_HEIGHT, height);
	xiSetParamInt(xiH, XI_PRM_OFFSET_Y, offset_y);
	
}

void Camera::Start(){
	printf("Starting acquisition...\n");
	xiStartAcquisition(xiH);
}

void Camera::Stop(){
	printf("Stopping acquisition...\n");
	xiStopAcquisition(xiH);
	
}

void Camera::GetFrame(uint8_t* pixels_corrected){
	xiGetImage(xiH, 5000, &image);
	uint8_t * pixels = (uint8_t*)image.bp;
	memcpy(pixels_corrected, pixels, GetHeight() * GetWidth() * CHANNEL_NUM * sizeof(uint8_t));
		
}