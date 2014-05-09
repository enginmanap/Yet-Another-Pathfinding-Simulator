#pragma once
#include <fstream>
#include "DataMatrix.h"

namespace yaps {

	/**
	 *  Class responible for loading data from input
	 */
	class InputCollector {
		DataMatrix<float> &riverBottom; // Reference to data matrix
		std::ifstream dataFile; // File input stream
	public:
		/**
		 *	Constructor
		 *	@param riverBottomRef Init value for riverBottom 
		 */
		InputCollector(DataMatrix<float> &);
		/**
		 *	Destructor
		 */
		~InputCollector();
		/**
		 *	Prepare file for reading data
		 *	@param filePath Standard path to data source
		 *	@return true on success, false otherwise
		 */
		bool openFile(std::string);
		/**
		 *	Load data from given file to the matrix (distance x width)
		 *	@return true on success, false otherwise
		 */
		bool loadDataFromFile();
	};

}
