// WCADecoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "WICA/Wca3.h"
#define MSF_GIF_IMPL
#include "msf_gif.h"
#include "spng.h"

BYTE* flip_pixels(BYTE* bgr, int size) {
	int o;
	BYTE* d = new BYTE[size+1];

	for (o = 0; o < size; o += 4) {
		d[o] = bgr[o + 2];
		d[o + 1] = bgr[o + 1];
		d[o + 2] = bgr[o];
		d[o + 3] = bgr[o + 3];
	}

	return d;
}

int main(int argc, char *argv[])
{
	CWca3* WICAInterface = new CWca3;
	MsfGifState gifState = {};
	MsfGifResult result;

	spng_ctx* png_ctx = NULL;
	struct spng_ihdr png_ihdr = { 0 }; /* zero-initialize to set valid defaults */
	int png_result = 0;
	int out_type = 0;
	size_t png_size;
	void* png_buf = NULL;
	BYTE* raw_buf = NULL;
	FILE* png_file = NULL;
	char temp_name[128];

	int img_size = 0;

	msf_gif_bgra_flag = true;

	if (argc < 3) {
		std::cerr << "Not enough arguments\n";
		std::cerr << "Usage: " << argv[0] << " file output\n";
		return 1;
	}

	if (strcmp(argv[2], "-frames") == 0) {
		out_type = 1;
	}

	if (!WICAInterface->Load(argv[1])) {
		std::cerr << "Failed to open file\n";
		return 1;
	};


	//printf("Resolution: %ix%i\nFrames: %i\n", WICAInterface->GetWidth(), WICAInterface->GetHeight(), WICAInterface->GetFrameNum());
	if (out_type == 0) {
		msf_gif_begin(&gifState, WICAInterface->GetWidth(), WICAInterface->GetHeight());
	}
	else {
		png_ihdr.width = WICAInterface->GetWidth();
		png_ihdr.height = WICAInterface->GetHeight();
		png_ihdr.bit_depth = 8;
		png_ihdr.color_type = SPNG_COLOR_TYPE_TRUECOLOR_ALPHA;
	}

	for (int c = 0; c < WICAInterface->GetFrameNum(); c++) {
		WICAInterface->Decode(c);
		//std::cout << WICAInterface->GetDelay() << "\n";
		if (out_type == 0) {
			msf_gif_frame(&gifState, WICAInterface->GetImage(), WICAInterface->GetDelay(), 16, WICAInterface->GetWidth() * 4);
		}
		else {
			png_ctx = spng_ctx_new(SPNG_CTX_ENCODER);

			spng_set_option(png_ctx, SPNG_IMG_COMPRESSION_LEVEL, 9);
			spng_set_option(png_ctx, SPNG_ENCODE_TO_BUFFER, 1);
			spng_set_ihdr(png_ctx, &png_ihdr);

			img_size = (WICAInterface->GetWidth() * WICAInterface->GetHeight()) * 4;
			raw_buf = flip_pixels(WICAInterface->GetImage(), img_size);

			png_result = spng_encode_image(png_ctx, raw_buf, img_size, SPNG_FMT_PNG, SPNG_ENCODE_FINALIZE);

			if (png_result) {
				std::cerr << "PNG encoded failed with error: " << spng_strerror(png_result) << "\n";
				spng_ctx_free(png_ctx);
				return 1;
			}

			png_buf = spng_get_png_buffer(png_ctx, &png_size, &png_result);

			if (png_buf == NULL)
			{
				std::cerr << "PNG get buffer failed with error: " << spng_strerror(png_result) << "\n";
				spng_ctx_free(png_ctx);
				return 1;
			}

			sprintf(temp_name, "%s_%02d_%02d.png", argv[1], c + 1, WICAInterface->GetDelay());

			png_file = fopen(temp_name, "wb");
			if (png_file == NULL) {
				std::cerr << "Cannot open output file.\n";
				free(png_buf);
				spng_ctx_free(png_ctx);
				return 1;
			}

			fwrite(png_buf, png_size, 1, png_file);

			free(png_buf);
			spng_ctx_free(png_ctx);
		}
		
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

	if (out_type == 0) {
		result = msf_gif_end(&gifState);

		if (result.data) {
			FILE* fp = fopen(argv[2], "wb");
			fwrite(result.data, result.dataSize, 1, fp);
			fclose(fp);
		}

		msf_gif_free(result);
	}
	else {

	}
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
