#include "tr_paraload.hpp"

#include <curl/curl.h>

struct MemoryStruct {
  byte * memory;
  size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = (byte *)realloc((void *)mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
	/* out of memory! */
	printf("not enough memory (realloc returned NULL)\n");
	return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

void R_ParallelDownloadNetTexture(char const * name, shader_t * sh) {
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;

	chunk.memory = (byte *)malloc(1);
	chunk.size = 0;

	curl_global_init(CURL_GLOBAL_ALL);

	curl_handle = curl_easy_init();

	curl_easy_setopt(curl_handle, CURLOPT_URL, name + 1);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);

	res = curl_easy_perform(curl_handle);
	long http_code = 0;
	curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

	if(res != CURLE_OK || http_code != 200) {
		Com_Printf("CURL could not find an image at URL: \"%s\".\n", name + 1);
		curl_easy_cleanup(curl_handle);
		free(chunk.memory);
		curl_global_cleanup();
		//shader.defaultShader = true;
		//return FinishShader();
	}

	//image = R_LoadImageMemory(name, chunk.memory, chunk.size, mipRawImage, mipRawImage, qtrue, mipRawImage ? GL_REPEAT : GL_CLAMP);

	curl_easy_cleanup(curl_handle);

	free(chunk.memory);

	curl_global_cleanup();
}
