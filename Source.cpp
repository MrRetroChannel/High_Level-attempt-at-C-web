#define CURL_STATICLIB
#include <curl/curl.h>
#include <iostream>
#include <cstdlib>
using namespace std;
#pragma warning(disable:4996)

enum class header : unsigned char
{
	CONTENT_TYPE
};

size_t WriteData(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	size_t ret = fwrite(ptr, size, nmemb, stream);
	return ret;
}

class Client
{
	CURL* curl = nullptr;
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
	{
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}

	curl_slist* list = NULL;
	string url;
	public:
		Client()
		{}

		Client(string a) : url(a)
		{}

		Client setUrl(const string& url)
		{
			this->url = url;
			return *this;
		}

		Client set_header(header head, const char* data)
		{
			string res;
			switch(head)
			{
			case header::CONTENT_TYPE:
					res += "Content-type: ";
					break;
			}
			res += data;
			list = curl_slist_append(list, res.c_str());
			return *this;
		}



	string post(string postdata)
	{
		string res;
		curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		list = NULL;
		return res;
	}

	string get()
	{
		string res;
		CURL* curl;
		curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return res;
	}
};

int main()
{
	cout << Client().setUrl("http://ip-api.com/json/24.48.0.1").set_header(header::CONTENT_TYPE, "application/json").get();
}