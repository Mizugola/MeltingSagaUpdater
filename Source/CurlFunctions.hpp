#pragma once

#include <stdio.h>
#include <string>
#include <curl/curl.h>

namespace fn
{
	namespace Curl
	{
		size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
		void downloadFile(const std::string& link, const std::string& file);
	}
}