#include <string>
#include <vector>

class StringUtils {
public:
    static std::vector<std::string_view> split(std::string_view str, char delim);
    static std::string killSwitch(std::string& str);
};