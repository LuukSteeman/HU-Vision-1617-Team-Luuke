#include "StudentPreProcessing.h"
#include "ImageFactory.h"
#include <cmath>
#include <functional>
#include <vector>
#include <iomanip>
#include <iostream>
#include <math.h>

std::vector<std::vector<int>> gradenVector;

void randopzwart(IntensityImage & returnImage, int wdth, int hght) {
	for (int x = 0; x < wdth; ++x) {
		returnImage.setPixel(x, hght - 1, 0);
		returnImage.setPixel(x, 0, 0);
	}
	for (int y = 0; y < hght; ++y) {
		returnImage.setPixel(0, y, 0);
		returnImage.setPixel(wdth - 1, y, 0);
	}
}


void apply_gaussian(IntensityImage & returnImage, int matrix[3][3], int x, int y) {
	int result = 0;
	int kernelX[3][3] = {
		{ { 1 },{ 2 },{ 1 } },
		{ { 2 },{ 4 },{ 2 } },
		{ { 1 },{ 2 },{ 1 } }
	};
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result += (kernelX[i][j] * matrix[i][j]);
		}
	}
	result /= 16;
	returnImage.setPixel(x, y, result);
}

void apply_kernel(IntensityImage & returnImage, int matrix[3][3], int x, int y, std::vector<std::vector<int>>& gradenVector) {
	int result = 0;
	int resultX = 0;
	int resultY = 0;
	int kernelX[3][3] = {
		{ { -1 },{ 0 },{ 1 } },
		{ { -2 },{ 0 },{ 2 } },
		{ { -1 },{ 0 },{ 1 } }
	};
	int kernelY[3][3] = {
		{ { -1 },{ -2 },{ -1 } },
		{ { 0 },{ 0 },{ 0 } },
		{ { 1 },{ 2 },{ 1 } }
	};
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			resultX += (kernelX[i][j] * matrix[i][j]);
		}
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			resultY += (kernelY[i][j] * matrix[i][j]);
		}
	}
	float resultYf = resultY;
	float resultXf = resultX;
	float pi = atan(1) * 4;
	double hoek = (((atan2(resultYf, resultXf)) * 180) / pi);
	int hoekRound = 0;
	if (hoek > 22.5 && hoek < 67.5) {
		hoekRound = 45;
	}
	else if (hoek > 67.5 && hoek < 112.5) {
		hoekRound = 90;
	}
	else if (hoek > 112.5 && hoek< 157.5) {
		hoekRound = 135;
	}
	gradenVector[x][y] = hoekRound;

	int G = sqrt(((resultY*resultY) + (resultX*resultX)));
	if (G > 255) G = 255;
	if (G < 0) G = 0;
	returnImage.setPixel(x, y, G);
}


/*






*/

void find_edge(IntensityImage & image, int x_shift, int y_shift, int x, int y, int dir, int lower_threshold, std::vector<std::vector<int>> & gradenVector)
{
	int width = image.getWidth();
	int height = image.getHeight();
	int new_x = 0;
	int new_y = 0;

	bool edge_end = false;
	if (x_shift < 0)
	{
		if (x > 0)
		{
			new_x = x + x_shift;
		}
		else
		{
			edge_end = true;
		}
	}
	else if (x < width - 1)
	{
		new_x = x + x_shift;
	}
	else
	{
		edge_end = true;
	}

	if (y_shift < 0)
	{
		if (y > 0)
		{
			new_y = y + y_shift;
		}
		else
		{
			edge_end = true;
		}
	}
	else if (y < height - 1)
	{
		new_y = y + y_shift;
	}
	else
	{
		edge_end = true;
	}

	while ((gradenVector[new_x][new_y] == dir) && !edge_end && (image.getPixel(new_x, new_y) > lower_threshold))
	{
		image.setPixel(new_x, new_y, 255);
		if (x_shift < 0)
		{
			if (new_x > 0)
			{
				new_x += x_shift;
			}
			else
			{
				edge_end = true;
			}
		}
		else if (new_x < width - 1)
		{
			new_x += x_shift;
		}
		else
		{
			edge_end = true;
		}

		if (y_shift < 0)
		{
			if (new_y > 0)
			{
				new_y += y_shift;
			}
			else
			{
				edge_end = true;
			}
		}
		else if (new_y < height - 1)
		{
			new_y += y_shift;
		}
		else
		{
			edge_end = true;
		}
	}
}

void suppress_non_maximum(int x_shift, int y_shift, int x, int y, int dir, int lower_threshold, IntensityImage & returnImage)
{
	int width = returnImage.getWidth();
	int height = returnImage.getHeight();
	int new_x = 0;
	int new_y = 0;
	bool edge_end = false;
	float non_max[10000][3];
	int pixel_count = 0;
	int count;
	int max[3];

	if (x_shift < 0)
	{
		if (x > 0)
		{
			new_x = x + x_shift;
		}
		else
		{
			edge_end = true;
		}
	}
	else if (x < width - 1)
	{
		new_x = x + x_shift;
	}
	else
	{
		edge_end = true;
	}

	if (y_shift < 0)
	{
		if (y > 0)
		{
			new_y = y + y_shift;
		}
		else
		{
			edge_end = true;
		}
	}
	else if (y < height - 1)
	{
		new_y = y + y_shift;
	}
	else
	{
		edge_end = true;
	}

	while ((gradenVector[new_x][new_y] == dir) && !edge_end && (returnImage.getPixel(new_x, new_y) == 255))
	{
		if (x_shift < 0)
		{
			if (new_x > 0)
			{
				new_x += x_shift;
			}
			else
			{
				edge_end = true;
			}
		}
		else if (new_x < width - 1)
		{
			new_x += x_shift;
		}
		else
		{
			edge_end = true;
		}

		if (y_shift < 0)
		{
			if (new_y > 0)
			{
				new_y += y_shift;
			}
			else
			{
				edge_end = true;
			}
		}
		else if (new_y < height - 1)
		{
			new_y += y_shift;
		}
		else
		{
			edge_end = true;
		}

		non_max[pixel_count][0] = new_x;
		non_max[pixel_count][1] = new_y;
		non_max[pixel_count][2] = gradenVector[new_x][new_y];
		pixel_count++;
	}

	edge_end = false;
	x_shift *= -1;
	y_shift *= -1;
	if (x_shift < 0)
	{
		if (x > 0)
		{
			new_x = x + x_shift;
		}
		else
		{
			edge_end = true;
		}
	}
	else if (x < width - 1)
	{
		new_x = x + x_shift;
	}
	else
	{
		edge_end = true;
	}

	if (y_shift < 0)
	{
		if (y > 0)
		{
			new_y = y + y_shift;
		}
		else
		{
			edge_end = true;
		}
	}
	else if (y < height - 1)
	{
		new_y = y + y_shift;
	}
	else
	{
		edge_end = true;
	}

	while ((gradenVector[new_x][new_y] == dir) && !edge_end && (returnImage.getPixel(new_x, new_y) == 255))
	{
		if (x_shift < 0)
		{
			if (new_x > 0)
			{
				new_x += x_shift;
			}
			else
			{
				edge_end = true;
			}
		}
		else if (new_x < width - 1)
		{
			new_x += x_shift;
		}
		else
		{
			edge_end = true;
		}

		if (y_shift < 0)
		{
			if (new_y > 0)
			{
				new_y += y_shift;
			}
			else
			{
				edge_end = true;
			}
		}
		else if (new_y < height - 1)
		{
			new_y += y_shift;
		}
		else
		{
			edge_end = true;
		}

		non_max[pixel_count][0] = new_x;
		non_max[pixel_count][1] = new_y;
		non_max[pixel_count][2] = gradenVector[new_x][new_y];
		pixel_count++;
	}

	max[0] = 0;
	max[1] = 0;
	max[2] = 0;
	for (count = 0; count < pixel_count; count++)
	{
		if (non_max[count][2] > max[2])
		{
			max[0] = non_max[count][0];
			max[1] = non_max[count][1];
			max[2] = non_max[count][2];
		}
	}
	for (count = 0; count < pixel_count; count++)
	{
		returnImage.setPixel(non_max[count][0], non_max[count][1], 0);
	}
}


void edge_thinning(std::vector<std::vector<int>> & gradenVector, IntensityImage & returnImage)
{
	Intensity upper_threshold = 180;
	Intensity lower_threshold = 140;
	for (int x = 0; x < returnImage.getWidth(); x++)
	{
		for (int y = 0; y < returnImage.getHeight(); y++)
		{
			if (returnImage.getPixel(x, y) > upper_threshold)
			{
				int current = gradenVector[x][y];
				switch (current)
				{
				case 0:
					find_edge(returnImage, 0, 1, x, y, 0, lower_threshold, gradenVector);
					break;
				case 45:
					find_edge(returnImage, 1, 1, x, y, 45, lower_threshold, gradenVector);
					break;
				case 90:
					find_edge(returnImage, 1, 0, x, y, 90, lower_threshold, gradenVector);
					break;
				case 135:
					find_edge(returnImage, 1, -1, x, y, 135, lower_threshold, gradenVector);
					break;
				default:
					returnImage.setPixel(x, y, 0);
				}
			}
			else
			{
				returnImage.setPixel(x, y, 0);
			}
		}
	}
}

void find_non_maximum_suppression(IntensityImage & returnImage, std::vector<std::vector<int>> gradenVector)
{
	Intensity lower_threshold = 140;
	for (int x = 1; x < returnImage.getWidth() - 1; x++)
	{
		for (int y = 1; y < returnImage.getHeight() - 1; y++)
		{
			if (returnImage.getPixel(x, y) == 255)
			{
				switch (gradenVector[x][y])
				{
				case 0:
					suppress_non_maximum(1, 0, x, y, 0, lower_threshold, returnImage);
					break;
				case 45:
					suppress_non_maximum(1, -1, x, y, 45, lower_threshold, returnImage);
					break;
				case 90:
					suppress_non_maximum(0, 1, x, y, 90, lower_threshold, returnImage);
					break;
				case 135:
					suppress_non_maximum(1, 1, x, y, 135, lower_threshold, returnImage);
					break;
				default:
					break;
				}
			}
		}
	}
}


IntensityImage * StudentPreProcessing::stepToIntensityImage(const RGBImage &image) const {
	return nullptr;
}

IntensityImage * StudentPreProcessing::stepScaleImage(const IntensityImage &image) const {
	return nullptr;
}

IntensityImage * StudentPreProcessing::stepEdgeDetection(const IntensityImage &image) const {
	int hght = image.getHeight();
	int wdth = image.getWidth();

	IntensityImage * returnImage = ImageFactory::newIntensityImage(wdth, hght);

	randopzwart(*returnImage, wdth, hght);


	gradenVector.resize(wdth);
	for (int x = 0; x < wdth; ++x) gradenVector[x].resize(hght);

	int matrix1[3][3];
	for (int y = 1; y < (hght - 1); y++) {
		for (int x = 1; x < (wdth - 1); x++) {
			matrix1[0][0] = image.getPixel(x - 1, y - 1);
			matrix1[0][1] = image.getPixel(x, y - 1);
			matrix1[0][2] = image.getPixel(x + 1, y - 1);
			matrix1[1][0] = image.getPixel(x - 1, y);
			matrix1[1][1] = image.getPixel(x, y);
			matrix1[1][2] = image.getPixel(x + 1, y);
			matrix1[2][0] = image.getPixel(x - 1, y + 1);
			matrix1[2][1] = image.getPixel(x, y + 1);
			matrix1[2][2] = image.getPixel(x + 1, y + 1);
			apply_gaussian(*returnImage, matrix1, x, y);
		};
	}



	int matrix[3][3];
	for (int y = 1; y < (hght - 1); y++) {
		for (int x = 1; x < (wdth - 1); x++) {
			matrix[0][0] = image.getPixel(x - 1, y - 1);
			matrix[0][1] = image.getPixel(x, y - 1);
			matrix[0][2] = image.getPixel(x + 1, y - 1);
			matrix[1][0] = image.getPixel(x - 1, y);
			matrix[1][1] = image.getPixel(x, y);
			matrix[1][2] = image.getPixel(x + 1, y);
			matrix[2][0] = image.getPixel(x - 1, y + 1);
			matrix[2][1] = image.getPixel(x, y + 1);
			matrix[2][2] = image.getPixel(x + 1, y + 1);
			apply_kernel(*returnImage, matrix, x, y, gradenVector);
		};
	}

	return returnImage;
}


IntensityImage * StudentPreProcessing::stepThresholding(const IntensityImage &image) const {
	IntensityImage * returnImage = ImageFactory::newIntensityImage(image);
	edge_thinning(gradenVector, *returnImage);
	find_non_maximum_suppression(*returnImage, gradenVector);
	for (int x = 0; x < returnImage->getWidth(); x++)
	{
		for (int y = 0; y < returnImage->getHeight(); y++)
		{
			if (returnImage->getPixel(x, y))
			{
				returnImage->setPixel(x, y, 0);
			}
			else
			{
				returnImage->setPixel(x, y, 255);
			}
		}
	}
	return returnImage;
}
