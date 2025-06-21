#include<Windows.h>
#include<important.h>


int main(int argc, char** argv) {

	HANDLE sample = LoadLibraryW(L"C:\\Users\\dell\\Desktop\\codeinJava\\javascript&typescript\\dll_injection\\x64_dll\\pookie.dll");
	if (sample == NULL) {
		warn("Error load the custom library! error code : %lx", GetLastError());
		return	EXIT_FAILURE;
	}
	else {
		okay("Successfully load the dll without any error !");
		return EXIT_SUCCESS;
	}
}