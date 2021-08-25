#pragma once
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo/cuda.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp>
#include <vector>
#include <iostream>
#include <thread>

class QGenerator
{
public:
	QGenerator(int p_width, int p_height, int p_binW, int p_binH, int p_min, int p_max);

	~QGenerator();

	void SetCameraID(int p_ID);

	void Test();

	void Update();

	int GetRand();

	int GetSize();

	void SetThreadState(bool p_state);

	void Thread();

	void ShowMask(bool p_show);

	void ShowBins(bool p_show);

	bool GetMaskState();

	bool GetBinsState();

	bool GetThreadState();

	void PoolData();

	std::vector<int> GetNumbers(int p_count = -1);

	class Bin
	{
	public:
		Bin(int p_x, int p_y, int p_width, int p_height, int p_min, int p_max, bool p_start = true);

		void Start(int p_startValue);

		void Update();

		bool GetState();

		int GetIndex();

		bool PointInside(int p_x, int p_y);

		std::string ToString();

		void AddIndex();

		void Reset();

		std::vector<int> GetIndexContainer();

		void PaintState(cv::Mat& p_image, bool p_active);
	private:

		bool m_started;
		int m_index;
		std::vector<int> m_indexContainer;
		int m_x, m_y, m_height, m_width, m_max, m_min;

	};

	cv::Mat m_gray;
	cv::Mat m_color;
	cv::Mat m_activeBins;

private:
	int m_globalIndex;
	cv::VideoCapture m_cap;
	std::vector<Bin*> m_container;

	std::vector<int> m_numbers;

	bool m_threadState;
	bool m_showMask;
	bool m_showBins;
	bool m_poolData;

	int m_width, m_height, m_binW, m_binH, m_min, m_max;

};

