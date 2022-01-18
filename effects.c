#include "effects.h"

//Kernel matrix for edge detection filter
const double edge_matrix[3][3] = {
		{-1, -1, -1},
		{-1, 8, -1},
		{-1, -1, -1}};

//Kernel matrix for sharpen filter
const double sharp_matrix[3][3] = {
		{0, -1, 0},
		{-1, 5, -1},
		{0, -1, 0}};

//Kernel matrix for blur filter
const double bblur_matrix[3][3] = {
		{0.1111, 0.1111, 0.1111},
		{0.1111, 0.1111, 0.1111},
		{0.1111, 0.1111, 0.1111}};

//Kernel matrix for Gaussian blur filter
const double gblur_matrix[3][3] = {
		{0.0625, 0.125, 0.0625},
		{0.125, 0.25, 0.125},
		{0.0625, 0.125, 0.0625}};
