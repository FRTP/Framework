#ifndef UTILITY_H
#define UTILITY_H

namespace utility {
	enum ECommand {
		GET_FILE,
		GET_MD5
	};
	enum EError {
		READ_ERROR,
		OPEN_ERROR,
		OK
	};
};

#endif //UTILITY_H
