#include "CmstpPatcher.h"

int Help() {
	printf("[*] Usage:\n\t- program.exe <path/to/config>\n");
	return (0);
}

int wmain(int ac, WCHAR *argv[]) {
	if (ac != 2)
		return Help();

	if (!CmstpPatcher(argv[1])) {
		return -1;
	}

	return (0);
}