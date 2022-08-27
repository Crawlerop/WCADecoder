// WCADecoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "WICA/Wca3.h"
#define MSF_GIF_IMPL
#include "msf_gif.h"

int main(int argc, char *argv[])
{
	CWca3* WICAInterface = new CWca3;
	MsfGifState gifState = {};
	MsfGifResult result;
	msf_gif_bgra_flag = true;

	if (argc < 3) {
		std::cerr << "Not enough arguments\n";
		std::cerr << "Usage: " << argv[0] << " file output\n";
		return 1;
	}
	WICAInterface->Load(argv[1]);
	//printf("Resolution: %ix%i\nFrames: %i\n", WICAInterface->GetWidth(), WICAInterface->GetHeight(), WICAInterface->GetFrameNum());
	msf_gif_begin(&gifState, WICAInterface->GetWidth(), WICAInterface->GetHeight());
	for (int c = 0; c <= WICAInterface->GetFrameNum(); c++) {
		WICAInterface->Decode(c);
		//std::cout << WICAInterface->GetDelay() << "\n";
		msf_gif_frame(&gifState, WICAInterface->GetImage(), WICAInterface->GetDelay(), 16, WICAInterface->GetWidth() * 4);
		
		/*
		std::string fn(argv[1]);
		fn += "_" + NumberToString(c) + "_" + NumberToString(WICAInterface->GetWidth()) + "x" + NumberToString(WICAInterface->GetHeight()) + ".raw";
		FILE* fdo = fopen(fn.c_str(), "wb");
		if (fdo == (FILE*)NULL) {
			printf("File open error");
			return FALSE;
		}
		fwrite(WICAInterface->GetImage(), 1, (WICAInterface->GetWidth() * WICAInterface->GetHeight()) * 4, fdo);
		fclose(fdo);
		*/
	}
	result = msf_gif_end(&gifState);

	if (result.data) {
		FILE* fp = fopen(argv[2], "wb");
		fwrite(result.data, result.dataSize, 1, fp);
		fclose(fp);
	}

	msf_gif_free(result);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
