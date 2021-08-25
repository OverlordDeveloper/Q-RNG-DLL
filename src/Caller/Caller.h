#pragma once
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "../QGenerator/QGenerator.h"
#include <vector>
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

	extern __declspec(dllexport) void ImViewer();

	extern __declspec(dllexport) QGenerator* CreateInstance(int p_width, int p_height, int p_binW, int p_binH, int p_min, int p_max);

	extern __declspec(dllexport) void StartThread(QGenerator *p_gen);

	extern __declspec(dllexport) void SetCamera(QGenerator* p_gen, int p_ID);

	extern __declspec(dllexport) void PoolData(QGenerator* p_gen);

	extern __declspec(dllexport) void GetNumbers(QGenerator* p_gen, int p_count, char* p_numbers, int p_strLen);

#ifdef __cplusplus
}
#endif