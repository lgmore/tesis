// CLAHEVS.cpp : Defines the entry point for the console application.

#include "stdafx.h"

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS



#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <ctime>



typedef web::json::value JsonValue;
typedef web::json::value::value_type JsonValueType;
typedef std::wstring String;
typedef std::wstringstream StringStream;

// cpprest provides macros for all streams but std::clog in basic_types.h
#ifdef _UTF16_STRINGS
// On Windows, all strings are wide
#define uclog std::wclog
#else
// On POSIX platforms, all strings are narrow
#define uclog std::clog
#endif // endif _UTF16_STRINGS

using namespace std;
using namespace web::http::experimental::listener;
using namespace web::http;
using namespace web;
using namespace cv;
using namespace std;
using cv::CLAHE;

double getPSNR(const Mat& I1, const Mat& I2)
{
	Mat s1;
	absdiff(I1, I2, s1);       // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
	s1 = s1.mul(s1);           // |I1 - I2|^2

	Scalar s = sum(s1);         // sum elements per channel

	double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

	if (sse <= 1e-10) // for small values return zero
		return 0;
	else
	{
		double  mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0*log10((255 * 255) / mse);
		return psnr;
	}
}

void logclahe(std::string msg)
{
	time_t now = time(0);
	struct tm tm;
	localtime_s(&tm, &now);
	ofstream out("clahe.log", ios::out|ios::app);
	out << tm.tm_year+1900 << '/' << tm.tm_mon+1 << '/' << tm.tm_mday
		<< ' ' << tm.tm_hour << ':' << tm.tm_min << ':' << tm.tm_sec << ": ";
	out << msg << endl;
	out.close();
}

void respond(const http_request& request, const status_code& status, const json::value& response) {
	json::value resp;
	resp[U("status")] = json::value::number(status);
	resp[U("response")] = response;

	// Pack in the current time for debugging purposes.
	time_t now = time(0);
	utility::stringstream_t ss;
	struct tm timeinfo;


	cout << now << endl;
	ss << localtime_s(&timeinfo, &now);
	resp[U("server_time")] = json::value::string(ss.str());

	request.reply(status, resp);
}

std::wstring JsonValueTypeToString(const JsonValueType& type)
{
	switch (type)
	{

	case web::json::value::value_type::Array: return L"Array";
	case web::json::value::value_type::Boolean: return L"Boolean";
	case web::json::value::value_type::Null: return L"Null";
	case web::json::value::value_type::Number: return L"Number";
	case web::json::value::value_type::Object: return L"Object";
	case web::json::value::value_type::String: return L"String";
	}
}

void Externalize(JsonValue& json)
{
	for (auto iter = json.as_object().cbegin(); iter != json.as_object().cend(); ++iter)
	{
		auto k = iter->first;
		auto v = iter->second;

		auto key = k;
		auto value = v.serialize();

		wcout << key << L" : " << value << " (" << JsonValueTypeToString(v.type()) << ")" << endl;
	}
}

json::value getImagenOriginal(){

	logclahe("llamado a imagen original");
	Mat m = imread("C:\\Users\\marcos\\Lenna.jpg", CV_LOAD_IMAGE_GRAYSCALE); //input image

	json::value retorno;
	json::value valores;
	int contador = 0;
	for (int j = 0; j<m.rows; j++)
	{
		for (int i = 0; i<m.cols; i++)
		{

			valores[contador] = m.at<uchar>(j, i);
			contador++;

		}
	}
	retorno[L"valores"] = valores;
	retorno[L"filas"] = json::value::number(m.rows);
	retorno[L"columnas"] = json::value::number(m.cols);
	imwrite("Lennaoriginal.jpg", m);
	return retorno;
}

json::value getCLAHE(json::value parametros){

	Mat m = imread("C:\\Users\\marcos\\Lenna.jpg", CV_LOAD_IMAGE_GRAYSCALE); //input image
	
	//imshow("lena_GRAYSCALE", m);

	
	 
	double clipLimit = parametros[L"clipLimit"].as_double();
	int ventanax = parametros[L"ventanax"].as_integer();
	int ventanay = parametros[L"ventanay"].as_integer();

	std::ostringstream logbuffer;

	logbuffer << "vent x: " << ventanax << " venty: " << ventanay << " cliplimit: " << clipLimit << endl;

	logclahe(logbuffer.str());



	Ptr<CLAHE> clahe = createCLAHE();
	clahe->setClipLimit(clipLimit);
	clahe->setTilesGridSize(Size(ventanax, ventanay));

	Mat dst;
	clahe->apply(m, dst);
	json::value retorno;
	retorno[L"filas"] = json::value::number(dst.rows);
	retorno[L"columnas"] = json::value::number(dst.cols);
	retorno[L"psnr"] = json::value::number(getPSNR(m, dst));
	int contador = 0;
	json::value valores;
	for (int j = 0; j<dst.rows; j++)
	{
		for (int i = 0; i<dst.cols; i++)
		{

			valores[contador] = dst.at<uchar>(j, i);
			contador++;

		}
	}
	retorno[L"valores"] = valores;
	//imshow("lena_CLAHE", dst);
	
	time_t now = time(0);
	struct tm tm;
	localtime_s(&tm, &now);
	std::ostringstream nombreimagen;
	nombreimagen << tm.tm_year + 1900 << tm.tm_mon + 1 << tm.tm_mday << tm.tm_hour << tm.tm_min << tm.tm_sec;
	imwrite("Lenna"+nombreimagen.str()+".jpg", dst);

	return retorno;

}


int main()
{
	// Synchronously bind the listener to all nics.
	uclog << U("Starting listener.") << endl;
	http_listener listener(U("http://localhost:8080/json"));
	http_listener listener2(U("http://localhost:8081/json"));
	listener.open().wait();
	listener2.open().wait();

	// Handle incoming requests.
	uclog << U("Setting up JSON listener.") << endl;

	listener2.support(methods::POST, [](http_request req){

		json::value retorno = getImagenOriginal();
		req.reply(status_codes::OK,retorno);

	});

	listener.support(methods::POST, [](http_request req) {
		auto http_get_vars = uri::split_query(req.request_uri().query());

		auto payload = req.extract_json().get();
		json::value retorno = getCLAHE(payload);

		req.reply(status_codes::OK, retorno);
		//respond(req, status_codes::OK, retorno);
	});

	// Wait while the listener does the heavy lifting.
	// TODO: Provide a way to safely terminate this loop.
	uclog << U("Waiting for incoming connection...") << endl;
	while (true) {

		this_thread::sleep_for(chrono::milliseconds(2000));
	}

	// Nothing left to do but commit suicide.
	uclog << U("Terminating JSON listener.") << endl;
	listener.close();
	listener2.close();


	return 0;
}




