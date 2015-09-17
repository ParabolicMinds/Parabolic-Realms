#include "tr_paraload.hpp"

#include <curl/curl.h>

#include <atomic>
#include <vector>
#include <thread>

typedef struct curlmem_s {
  byte * memory;
  size_t size;
} curlmem_t;

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
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

typedef struct pdli_s {
	image_t * img;
	char name [MAX_QPATH];
	std::thread * thr;
	std::atomic_bool done;
	bool success;
	curlmem_t mem;
	CURL *curl_handle;
} pdli_t;

static void R_FreePDLI(pdli_t * * pdl) {
	curl_easy_cleanup((*pdl)->curl_handle);
	free((*pdl)->mem.memory);
	delete *pdl;
}

static void R_ParallelDownloadNetImage_ThreadRun(pdli_t * pdl) {
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

static std::vector<pdli_t *> pdlis;
static std::vector<image_t *> pli;

void R_ParallelInitialize() {
	curl_global_init(CURL_GLOBAL_ALL);
}

void R_ParallelShutdown() {
	curl_global_cleanup();
}

bool R_ParallelDownloadReady() {
	image_t * image = NULL;
	std::vector<pdli_t *>::iterator pdlii;
	for (pdlii = pdlis.begin(); pdlii != pdlis.end();) {
		if(!(*pdlii)->done) {
			pdlii++;
			continue;
		}
		Com_Printf("CURL thread ended.\n");
		(*pdlii)->thr->join();
		if ((*pdlii)->success) {
			image = R_LoadImageMemory((*pdlii)->name, (*pdlii)->mem.memory, (*pdlii)->mem.size, qtrue, qtrue, qtrue, GL_REPEAT, qfalse);
			if (image) {
				if (image) memcpy((*pdlii)->img, image, sizeof(image_t));
				Z_Free(image);
			}
		}
		if (!image) {
			image = R_FindImageFile("textures/para/nettexfail", qtrue, qtrue, qtrue, GL_REPEAT);
			memcpy((*pdlii)->img, image, sizeof(image_t));
			Q_strncpyz((*pdlii)->img->imgName, (*pdlii)->name, MAX_QPATH);
		}
		pli.push_back(image);
		R_FreePDLI(&*pdlii);
		pdlii = pdlis.erase(pdlii);
		if (image) break;
	}
	if (image != NULL) return true;
	return false;
}

image_t * R_ParallelDownloadNetImage(char const * name) {
	image_t * nettex = R_FindImageFile("textures/para/nettexload", qtrue, qtrue, qtrue, GL_REPEAT);
	if (!nettex) {
		Com_Error(ERR_FATAL, "MISSING ASSETSPARA.PK3\n");
	}
	image_t * newimg = R_CopyImageNewName(nettex, name);

	Com_Printf(name);

	pdli_t * pdl = new pdli_t;
	pdl->img = newimg;
	Q_strncpyz(pdl->name, name, MAX_QPATH);
	pdl->done.store(false);
	pdl->success = false;

	pdl->thr = new std::thread {R_ParallelDownloadNetImage_ThreadRun, pdl};
	pdlis.push_back(pdl);
	Com_Printf("New CURL thread began.\n");

	return newimg;
}

void R_ReloadNetImages() {

}
