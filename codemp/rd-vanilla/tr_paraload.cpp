#include "tr_paraload.hpp"

#include <curl/curl.h>

#include <atomic>
#include <vector>
#include <thread>

typedef struct curlmem_s {
  byte * memory;
  size_t size;
} curlmem_t;

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  curlmem_t *mem = (curlmem_t *)userp;

  mem->memory = (byte *)realloc((void *)mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
	return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

typedef struct pdl_s {
	shader_t * sh;
	char name [MAX_QPATH];
	std::thread * thr;
	std::atomic_bool done;
	bool success;
	curlmem_t mem;
	CURL *curl_handle;
} pdl_t;

static void R_FreePDL(pdl_t * * pdl) {
	curl_easy_cleanup((*pdl)->curl_handle);
	free((*pdl)->mem.memory);
	delete *pdl;
}

static void R_ParallelDownloadNetTexture_ThreadRun(pdl_t * pdl) {
	CURLcode res;

	pdl->mem.memory = (byte *)malloc(1);
	pdl->mem.size = 0;

	pdl->curl_handle = curl_easy_init();

	curl_easy_setopt(pdl->curl_handle, CURLOPT_URL, pdl->name + 1);
	curl_easy_setopt(pdl->curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(pdl->curl_handle, CURLOPT_WRITEDATA, (void *)&pdl->mem);
	curl_easy_setopt(pdl->curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(pdl->curl_handle, CURLOPT_FOLLOWLOCATION, 1);

	res = curl_easy_perform(pdl->curl_handle);
	long http_code = 0;
	curl_easy_getinfo (pdl->curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

	if(res != CURLE_OK || http_code != 200) {
		pdl->success = false;
	} else {
		pdl->success = true;
	}
	pdl->done.store(true);
}

static std::vector<pdl_t *> pdls;

void R_ParallelInitialize() {
	curl_global_init(CURL_GLOBAL_ALL);
}

void R_ParallelShutdown() {
	curl_global_cleanup();
}

void R_ParallelDownloadNetTexture(char const * name, shader_t * sh) {
	pdl_t * pdl = new pdl_t;
	pdl->sh = sh;
	Q_strncpyz(pdl->name, name, MAX_QPATH);
	pdl->done.store(false);
	pdl->success = false;

	pdl->thr = new std::thread {R_ParallelDownloadNetTexture_ThreadRun, pdl};
	pdls.push_back(pdl);
	Com_Printf("New CURL thread began.\n");
}

bool R_ParallelDownloadReady() {
	image_t * image = NULL;
	std::vector<pdl_t *>::iterator pdli;
	for (pdli = pdls.begin(); pdli != pdls.end();) {
		if(!(*pdli)->done) {
			pdli++;
			continue;
		}
		Com_Printf("CURL thread ended.\n");
		(*pdli)->thr->join();
		if ((*pdli)->success) {
			image = R_LoadImageMemory((*pdli)->name, (*pdli)->mem.memory, (*pdli)->mem.size, qtrue, qtrue, qtrue, GL_REPEAT);
			if (image) R_FinishFutureShader((*pdli)->sh, image);
		}
		R_FreePDL(&*pdli);
		pdli = pdls.erase(pdli);
		if (image) break;
	}
	return image != NULL;
}
