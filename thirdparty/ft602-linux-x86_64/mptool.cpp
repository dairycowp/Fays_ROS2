#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <cstring>
#include "ft602_private.h"

using namespace std;

static void show_help(const char *prog)
{
	cout << "Sample FT602 mass product tool" << endl;
	cout << "Usage: " << prog <<
		" <chip configuration dump> [serial number string]" << endl;
}

static unique_ptr<char[]> read_file(const char *path, size_t expected_size)
{
	ifstream file(path, ios::in | ios::binary);

	if (!file.is_open()) {
		cout << "Failed to open file " << path << endl;
		return nullptr;
	}
	file.seekg (0, file.end);
	size_t size = file.tellg();

	if (size != expected_size) {
		cout << "File size is not correct" << endl;
		cout << "Expect " << expected_size <<
			" but the file is " << size << endl;
		return nullptr;
	}

	unique_ptr<char[]> result = make_unique<char[]>(size);

	file.seekg(0, ios::beg);
	file.read(result.get(), size);
	file.close();
	return move(result);
}

int main(int argc, char *argv[])
{
	if (argc != 2 && argc != 3) {
		show_help(argv[0]);
		return 1;
	}

	auto file = read_file(argv[1], sizeof(struct _FT_602CONFIGURATION));

	if (file == nullptr)
		return 1;

	auto conf = (FT_602CONFIGURATION *)file.get();

	if (conf->common.length != sizeof(FT_602CONFIGURATION)) {
		cout << "Damaged chip configuration" << endl;
		return 1;
	}

	/* Remove second language support */
	conf->desc[FT602_STR_DESC_LANGID_ARRAY].length = 2 + 2;

	/* Replace serial number */
	if (argc == 3) {
		string str(argv[2]);
		size_t len = str.size();

		if (len > 31) {
			cout << "Serial number string is too long" << endl;
			return 1;
		}

		conf->desc[FT602_STR_DESC_SERIALNUMBER].length = len * 2 + 2;
		for (size_t i = 0; i < len; i++)
			conf->desc[FT602_STR_DESC_SERIALNUMBER].string[i] = str[i];
	}

	FT_HANDLE handle = NULL;

	if (FT_OK != FT_Create(0, FT_OPEN_BY_INDEX, &handle)) {
		cout << "Failed to open device" << endl;
		return 1;
	}
	if (FT_OK != FT_SetChipConfiguration(handle, conf)) {
		cout << "Failed to update chip configuration" << endl;
		return 1;
	}
	FT_Close(handle);
	cout << "Succeeded" << endl;
	return 0;
}
