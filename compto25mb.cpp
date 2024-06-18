#include <iostream>
#include <sstream>
#include <array>
#include <cstdio>
#include <stdexcept>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main(int argc, char *argv[]) {
    std::string fileName=argv[1];
    std::stringstream ffprobeGetLength;
    ffprobeGetLength << "ffprobe -v error -select_streams v:0 -show_entries stream=duration -of default=noprint_wrappers=1:nokey=1 \""
    << argv[1] << "\"";

    int videoLengthSeconds = stoi(exec(ffprobeGetLength.str().c_str()));
    // std::cout << videoLengthSeconds;
    
    float newBitrate = 2e8/videoLengthSeconds;

    
    size_t lastIndex = fileName.find_last_of(".");
    std::string fileNameRaw = fileName.substr(0, lastIndex);
    std::string fileNameExt = fileName.substr(lastIndex);

    std::stringstream ffmpegCommand;
    ffmpegCommand << "ffmpeg -i \"" << argv[1] << "\" -b:v " << newBitrate 
    << " \"" << fileNameRaw << "_comp" << fileNameExt << "\"";

    system(ffmpegCommand.str().c_str());
    return 0;
}