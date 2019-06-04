#include <windows.h>
#include <fstream>
#include <sstream>
#include <string>
#include "pointCloudRender.h"
//#include <vld.h>


static std::string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen <= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	std::string strTemp(pszDst);
	delete[] pszDst;

	return strTemp;
}

// 利用winWIN32_FIND_DATA读取文件下的文件名
static void readImgNamefromFile(char* fileName, std::vector <std::string> &imgNames)
{
	// vector清零 参数设置
	imgNames.clear();
	WIN32_FIND_DATA file;
	int i = 0;
	char tempFilePath[MAX_PATH + 1];
	char tempFileName[256];
	// 转换输入文件名
	sprintf_s(tempFilePath, "%s/*", fileName);
	// 多字节转换
	WCHAR   wstr[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, tempFilePath, -1, wstr, sizeof(wstr));
	// 查找该文件待操作文件的相关属性读取到WIN32_FIND_DATA
	HANDLE handle = FindFirstFile(wstr, &file);
	if (handle != INVALID_HANDLE_VALUE)
	{
		FindNextFile(handle, &file);
		FindNextFile(handle, &file);
		// 循环遍历得到文件夹的所有文件名    
		do
		{
			sprintf_s(tempFileName, "%s", fileName);
			imgNames.push_back(WChar2Ansi(file.cFileName));
			imgNames[i].insert(0, tempFileName);
			i++;
		} while (FindNextFile(handle, &file));
	}
	FindClose(handle);
}

void show_test()
{
	std::vector<std::string> filenames;
	char* filePath;
	filePath = "../bin/files/";

	readImgNamefromFile(filePath, filenames);
	if (filenames.empty())
	{
		printf("File not found.\n");
		return;
	}
	else
	{
		printf("%d files.\n", filenames.size());
	}


	int choice = -1;
	//show options
	while (1)
	{
		printf("Enter your format to continue:\n");
		printf("1: 3 elements per line(x y z).\n");
		printf("2: 5 elements per line(X Y x y z).\n");
		printf("0: Exit.\n");

		scanf_s("%d", &choice);
		if (choice == 2 || choice == 1)
			break;
		else if (choice == 0)
			return;
		else
			printf("Wrong input,please enter again.\n");
	}
	
	
	bz::PointCloudRender ren;
	for (int i = 0; i < filenames.size();i++)
	{
		printf("file : %d\n",i+1);

		std::ifstream ifs(filenames[i]);
		std::string singleLine;
		std::vector<std::vector<float> > pts;

		while (std::getline(ifs, singleLine))
		{
			float X, Y;
			std::vector<float> point(3);

			std::stringstream linestream;
			linestream << singleLine;
			switch (choice)
			{
			case 1:
				linestream  >> point[0] >> point[1] >> point[2];
				break;
			case 2:
				linestream >> X >> Y >> point[0] >> point[1] >> point[2];
				break;
			default:
				printf("unknown format.\n");
				break;   
			}
			
			pts.push_back(point);
		}

		if (ren.LoadPoints(pts))
			ren.show();

		ifs.close();

	}	
}


int main(int argc,char** argv)
{

	show_test();

	system("pause");
	return 0;
}