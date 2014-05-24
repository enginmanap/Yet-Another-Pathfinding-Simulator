#pragma once

/*
	TODO:
	Implement Modified Shepard's Method
*/

namespace yaps {

	/**
	 *	Class which interpolates 2D data with a known scattered set of points.
	 *	For now it uses Inverse distance weighting (IDW) algorithm, Shepard's method.
	 *	@link http://en.wikipedia.org/wiki/Inverse_distance_weighting
	 */
	class ApproximationEngine {
		const int POWER_PARAMETER = 8;  // Shepard's method power parameter
        DataMatrix<float> &data;        // Reference to data matrix that should be interpolated (values 0 are considered missing)
        std::vector<unsigned> paramX;   // Vector of function x arguments
        std::vector<unsigned> paramY;   // Vector of function y arguments
        std::vector<float> valueF;      // Vector of function values in x, y

		/**
		 *	Fill the vectors with appropriate data, such as every index corresponds to the certain function value in x, y
		 */
		void prepareData();
		
		/**
		 *	Calculate weighting parameter from Shepard's method
		 *	@param xi x value of the first point
		 *	@param yi y value of the first point
		 *	@param xj x value of the second point
		 *	@param yj y value of the second point
		 *	@return value of weighting
		 */
		float calculateWeighting(unsigned, unsigned, unsigned, unsigned);

		/**
		 *	Calculate interpolation value for a certain point
		 *	@param x
		 *	@param y
		 */
		float interpolate(unsigned, unsigned);
	public:
		/**
		 *	Constructor
		 *	@param riverBottomRef Init value for riverBottom
		 */
		ApproximationEngine(DataMatrix<float> &);
		
		/**
		 *	Destructor
		 */
		~ApproximationEngine();
		
		/**
		 *	Prepare data and run interpolation algorithm for the whole matrix
		 *	@return reference (the same as was passed to the constructor) to the data matrix filled with approximated values
		 */
		DataMatrix<float> &approximate();
	};

}