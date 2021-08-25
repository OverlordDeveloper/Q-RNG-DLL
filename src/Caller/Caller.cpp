
#include "Caller.h"

std::string ParseOutput(std::vector<int>& p_vec, int p_maxSize)
{
	std::string l_out = "";
	for (size_t i = 0; i < p_vec.size(); i++)
	{
		l_out += std::to_string(p_vec[i]) + " ";
		//std::cout << p_vec[i] << " ";
	}

	std::cout << std::endl;

	//if (l_out.size() < p_maxSize)
	//	l_out.append(p_maxSize - l_out.size(), ' ');

	return l_out;
}

QGenerator* CreateInstance(int p_width, int p_height, int p_binW, int p_binH, int p_min, int p_max)
{
	return new QGenerator(p_width, p_height, p_binW, p_binH, p_min, p_max);
}

void StartThread(QGenerator* p_gen)
{
	p_gen->SetThreadState(true);
	std::thread t1(&QGenerator::Thread, p_gen);

	t1.join();
}

void SetCamera(QGenerator* p_gen, int p_ID)
{
	p_gen->SetCameraID(p_ID);
}

void PoolData(QGenerator* p_gen)
{
	p_gen->PoolData();
}

void GetNumbers(QGenerator* p_gen, int p_count, char* p_numbers, int p_strLen)
{
	std::vector<int> l_num = p_gen->GetNumbers(p_count);
	
	std::string l_str = ParseOutput(l_num, p_strLen);

	strcpy(p_numbers, l_str.c_str());
}

void ImViewer()
{
	//std::vector<unsigned char> inputImageBytes(data, data + dataLen);
	//cv::Mat image = cv::imdecode(inputImageBytes, 0);
	//cv::Mat processed;
	//cv::cvtColor(image, processed, cv::COLOR_BGR2GRAY);
	//std::vector<unsigned char> bytes;
	//imencode(".jpg", processed, bytes);

	cv::Mat processed = cv::imread("D:/Users/Horia/Desktop/clown.png.jpeg", 0);
	while (true)
	{
		cv::imshow("Im", processed);

		auto k = cv::waitKey(0);

		if (k == 113)
			break;
	}
	std::cout << "Hello world";
}

