#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>

using namespace std;

// Callback function to handle writing the downloaded data to a file
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    ofstream *output = (ofstream*)userp;
    output->write((char*)contents, total_size);
    return total_size;
}

void download_with_tor(const string& url, const string& output_filename) {
    CURL *curl;
    CURLcode res;
    ofstream output_file(output_filename, ios::binary);
    
    if (!output_file.is_open()) {
        cerr << "Error: Unable to open output file!" << endl;
        return;
    }

    // Initialize CURL globally
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL to download
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Use Tor as a proxy (Tor usually runs on localhost:9050)
        curl_easy_setopt(curl, CURLOPT_PROXY, "socks5h://127.0.0.1:9050");

        // Set the callback function to handle the downloaded data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_file);

        // Perform the download
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            cerr << "CURL error: " << curl_easy_strerror(res) << endl;
        } else {
            cout << "Download completed successfully!" << endl;
        }

        // Clean up CURL
        curl_easy_cleanup(curl);
    } else {
        cerr << "Error: Unable to initialize CURL!" << endl;
    }

    // Clean up CURL global state
    curl_global_cleanup();
    
    // Close the output file
    output_file.close();
}

int main() {
    string url;
    string filename;

    // Ask the user for a URL
    cout << "Enter the URL to download: ";
    cin >> url;

    // Ask the user for the output filename
    cout << "Enter the filename to save the download as: ";
    cin >> filename;

    // Download the file using Tor
    download_with_tor(url, filename);

    return 0;
}

