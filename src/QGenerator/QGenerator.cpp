#include "QGenerator.h"

std::mutex mutex;


QGenerator::QGenerator(int p_width /*image_width*/, int p_height /*image_height*/, int p_binX/*bin_numberX*/, int p_binY/*bin_numberY*/, int p_min, int p_max)
{
	m_min = p_min;
	m_max = p_max;
	m_width = p_width;
	m_height = p_height;
	m_binW = p_binX;
	m_binH = p_binY;

	m_globalIndex = m_min;
	int l_binWidth = (int)(p_width / (float)p_binX);
	int l_binHeight = (int)(p_height / (float)p_binY);

	///< Create active bins mat

	m_activeBins = cv::Mat(p_height, p_width, CV_32FC3, cv::Scalar(0, 0, 255));

	for (size_t i = 0; i < p_binY; i++)
	{
		cv::line(m_activeBins, cv::Point(i * l_binHeight, 0), cv::Point(i * l_binHeight, p_width), cv::Scalar(0));
	}

	for (size_t i = 0; i < p_binX; i++)
	{
		cv::line(m_activeBins, cv::Point(0, i * l_binWidth), cv::Point(p_height, i * l_binWidth), cv::Scalar(0));
	}

	for (size_t i = 0; i < p_binY; i++)
		for (size_t j = 0; j < p_binX; j++)
		{
			m_container.push_back(new Bin(j * l_binWidth, i * l_binHeight, l_binWidth, l_binHeight, p_min, p_max, false));
		}
}

QGenerator::~QGenerator()
{

}

void QGenerator::SetCameraID(int p_ID)
{
	int apiID = cv::CAP_ANY;
	m_cap.open(p_ID, apiID);
	// check if we succeeded
	if (!m_cap.isOpened()) {
		std::cout << "ERROR! Unable to open camera \n";
	}
}

void QGenerator::Update()
{
	if (m_poolData)
	{
		///< This means retrieve all the data
		m_numbers.clear();
		for (size_t i = 0; i < m_container.size(); i++)
			if (m_container[i]->GetState())
			{
				for (size_t j = 0; j < m_container[i]->GetIndexContainer().size(); j++)
					m_numbers.push_back(m_container[i]->GetIndexContainer()[j]);				

				m_container[i]->Reset();
				m_container[i]->PaintState(m_activeBins, false);
			}

		m_poolData = false;
	}

	///< Update global index
	if (m_globalIndex == m_max - 1)
		m_globalIndex = m_min;
	else
		m_globalIndex += 1;

	cv::Mat l_image, l_gray;

	m_cap.read(l_image);

	cv::resize(l_image, l_image, cv::Size(m_width, m_height));
	// check if we succeeded
	if (l_image.empty()) {
		std::cout << "ERROR! blank frame grabbed\n";
		return;
	}

	cv::cvtColor(l_image, l_gray, cv::COLOR_RGB2GRAY);
	//cv::fastNlMeansDenoising(l_gray, l_gray, 1.0f);

	cv::Scalar l_min(40);
	cv::Scalar l_max(255);

	cv::inRange(l_gray, l_min, l_max, l_gray);

	//cv::circle(l_gray, cv::Point(20, 150), 4, cv::Scalar(255), -1);

	std::vector<std::vector<cv::Point>> l_contours;
	cv::findContours(l_gray, l_contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);


	m_gray = l_gray;
	m_color = l_image;
	for (size_t i = 0; i < l_contours.size(); i++)
	{
		cv::Rect l_rect = cv::boundingRect(l_contours[i]);
		float l_cX = l_rect.x + l_rect.width / 2.0f;
		float l_cY = l_rect.y + l_rect.height / 2.0f;

		cv::Scalar l_color(0, 255, 0);
		cv::circle(m_color, cv::Point(l_cX, l_cY), 9, l_color);

		if (m_container[2]->PointInside(l_cY, l_cX))
		{
			std::cout << "Point inside: " << l_cX << " " << l_cY <<  " " << m_container[2]->ToString() << std::endl;
		}

		for (size_t j = 0; j < m_container.size(); j++)
		{
			if (!m_container[j]->GetState() && m_container[j]->PointInside(l_cY, l_cX))
			{
				m_container[j]->Start(m_globalIndex);
				m_container[j]->PaintState(m_activeBins, true);
				continue;
			}

			if (m_container[j]->GetState())
			{
				if (m_container[j]->PointInside(l_cX, l_cY))
				{
					m_container[j]->AddIndex();
					m_container[j]->PaintState(m_activeBins, true);
				}

				m_container[j]->Update();
			}
		}
	}


	cv::imshow("Mask", m_gray);

	cv::imshow("Bins", m_activeBins);

	cv::imshow("Color", m_color);

	cv::waitKey(200);
}

int QGenerator::GetRand()
{

}

int QGenerator::GetSize()
{

}

void QGenerator::SetThreadState(bool p_state)
{
	mutex.lock();
	m_threadState = p_state;
	mutex.unlock();
}

void QGenerator::Thread()
{
	while (GetThreadState())
	{
		Update();
	}
}

void QGenerator::ShowMask(bool p_show)
{
	mutex.lock();
	m_showMask = p_show;
	mutex.unlock();
}

void QGenerator::ShowBins(bool p_show)
{
	mutex.lock();
	if ((m_showBins == true) && (p_show == false))
		cv::destroyWindow("Bins");

	m_showBins = p_show;
	mutex.unlock();
}

bool QGenerator::GetMaskState()
{
	mutex.lock();
	bool l_state = m_showMask;
	mutex.unlock();

	return l_state;
}

bool QGenerator::GetBinsState()
{
	mutex.lock();
	bool l_state = m_showBins;
	mutex.unlock();

	return l_state;
}

bool QGenerator::GetThreadState()
{
	mutex.lock();
	bool l_state = m_threadState;
	mutex.unlock();

	return l_state;
}

void QGenerator::PoolData()
{
	mutex.lock();
	m_poolData = true;
	mutex.unlock();
}

std::vector<int> QGenerator::GetNumbers(int p_count /*= -1*/)
{
	if ((p_count == -1) || (p_count > m_numbers.size()))
	{
		std::vector<int> l_dummy(m_numbers);
		m_numbers.clear();
		return l_dummy;
	}

	std::vector<int> l_dummy;
	l_dummy.reserve(p_count);

	for (size_t i = 0; i < p_count; i++)
		l_dummy.push_back(m_numbers[i]);

	return l_dummy;
}

void QGenerator::Test()
{
	SetCameraID(0);

	cv::Mat l_image, l_gray;

	m_cap.read(l_image);

	// check if we succeeded
	if (l_image.empty()) {
		std::cout << "ERROR! blank frame grabbed\n";
		return;
	}

	cv::cvtColor(l_image, l_gray, cv::COLOR_RGB2GRAY);

	//cv::fastNlMeansDenoising(l_gray, l_gray, 3.0f, 7, 21);

	//cv::Scalar l_min(40);
	//cv::Scalar l_max(255);

	//cv::inRange(l_gray, l_min, l_max, l_gray);

	//std::vector<std::vector<cv::Point>> l_contours;
	//cv::findContours(l_gray, l_contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

	//std::vector<int> b;

	//for (size_t i = 0; i < l_contours.size(); i++)
	//{
	//	cv::Rect l_rect = cv::boundingRect(l_contours[i]);
	//	float l_cX = l_rect.x + l_rect.width / 2.0f;
	//	float l_cY = l_rect.y + l_rect.height / 2.0f;
	//}


	while (true)
	{
		cv::imshow("frame", l_gray);


		cv::waitKey(0);
	}
}

QGenerator::Bin::Bin(int p_x, int p_y, int p_width, int p_height, int p_min, int p_max, bool p_start /*= true*/)
{
	m_width = p_width;
	m_height = p_height;
	m_x = p_x;
	m_y = p_y;
	m_max = p_max;
	m_min = p_min;

}

bool QGenerator::Bin::PointInside(int p_x, int p_y)
{
	bool l_xAxis = (p_x >= m_x) && (p_x < m_x + m_width);
	bool l_yAxis = (p_y >= m_y) && (p_y < m_y + m_height);

	if (l_xAxis && l_yAxis)
		return true;

	return false;
}

std::string QGenerator::Bin::ToString()
{
	std::string l_out = "";
	l_out += "Position:[" + std::to_string(m_x) + "," + std::to_string(m_y) + "]";
	l_out += "Width/Height:[" + std::to_string(m_width) + "," + std::to_string(m_height) + "]";
	return l_out;
}

void QGenerator::Bin::AddIndex()
{
	m_indexContainer.push_back(m_index);
}

void QGenerator::Bin::Reset()
{
	m_started = false;
	m_indexContainer.clear();
}

std::vector<int> QGenerator::Bin::GetIndexContainer()
{
	return m_indexContainer;
}

void QGenerator::Bin::PaintState(cv::Mat& p_image, bool p_active)
{
	cv::Point p1(m_y + 1, m_x + 1);
	cv::Point p2(m_y + m_height - 1, m_x + m_width - 1);

	if (p_active)
	{
		float l_fontScale = 0.6f * m_width / 13.0f;

		int l_baseline = 0;
		std::string l_text = std::to_string(m_indexContainer.size());
		if (l_text.size() > 2)
			l_text = "?";

		cv::Size l_textSize = cv::getTextSize(l_text, cv::FONT_HERSHEY_PLAIN, l_fontScale, 1.0f, &l_baseline);

		cv::Point l_center(m_y + 1 + (m_height - 1) / 2.0f - l_textSize.width / 2.0f, m_x + 1 + (m_width - 1) / 2.0f + l_textSize.height / 2.0f);
		cv::Point l_center1(m_y + m_height / 2.0f, m_x + m_width / 2.0f);

		cv::rectangle(p_image, p1, p2, cv::Scalar(0, 255, 0), -1);
		cv::putText(p_image, l_text, l_center, cv::FONT_HERSHEY_PLAIN, l_fontScale, cv::Scalar(0));
	}
	else
		cv::rectangle(p_image, p1, p2, cv::Scalar(0, 0, 255), -1);


}

void QGenerator::Bin::Start(int p_startValue)
{
	m_started = true;
	m_index = p_startValue;
}

void QGenerator::Bin::Update()
{
	if (m_started)
	{
		if (m_index == m_max - 1)
		{
			m_index = m_min;
		}
		else
			m_index += 1;
	}
}

bool QGenerator::Bin::GetState()
{
	return m_started;
}

int QGenerator::Bin::GetIndex()
{
	return m_index;
}
