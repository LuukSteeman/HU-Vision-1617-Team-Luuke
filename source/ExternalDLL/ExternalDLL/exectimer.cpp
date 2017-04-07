#include "exectimer.h"
#include <stdio.h>
#include <iostream>

/** The default constructor which initializes everything to zero.
 */
ExecTimer::ExecTimer() : BaseTimer() {}

/** This function will time the execution of the command specified
	in the parameter of this function. This command can contain 
	arguments needed for the execution.

	@param cmd The command to be executed.
	@param resout A boolean indicating whether or not the results from the 
		command should be printed.

	@returns True if the program was able to execute, false if the
		program did not execute properly.
 */
bool ExecTimer::run(std::string cmd, bool resout) {
	reset();
	start();
	FILE* pipe = _popen(cmd.c_str(), "r");
	if (!pipe) return false;
	char buffer[128];
	std::string result = "";
	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	_pclose(pipe);
	stop();
	if (resout)
		std::cout << result << std::endl;
	return true;
}