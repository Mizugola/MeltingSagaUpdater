#include "CurlFunctions.hpp"

namespace fn
{
	namespace Curl
	{
		size_t write_data(void * ptr, size_t size, size_t nmemb, FILE * stream)
		{
			size_t written = fwrite(ptr, size, nmemb, stream);
			return written;
		}

		void downloadFile(const std::string& link, const std::string& file)
		{
			CURL *curl;
			FILE *fp;
			CURLcode res;
			const char* url = link.c_str();
			const char* outfilename = file.c_str();
			curl = curl_easy_init();
			if (curl) {
				fopen_s(&fp, outfilename, "wb");
				curl_easy_setopt(curl, CURLOPT_URL, url);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				fclose(fp);
			}
		}
	}
}
