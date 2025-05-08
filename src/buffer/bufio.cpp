#include <filesystem>
#include <fstream>
#include <vector>

#include "bufio.h"

using std::string, std::vector, std::ifstream;
namespace fs = std::filesystem;

// externally accessible variables
size_t buf_line_pos;
size_t buf_col_pos;

// internal variables
size_t reading_pos;
vector<char> buffer;
vector<int> line_starts;
vector<int> line_lengths;

// buffer initialization
int buf_init(const char *filepath) {
    // initialize position variables
    reading_pos = 0;
    buf_line_pos = buf_col_pos = 1;

    // open file, return -1 if file not found
    fs::path input_filepath{filepath};
    if (!fs::exists(input_filepath))
        return -1;

    // resize buffer to file size, read file to buffer
    buffer.resize(fs::file_size(input_filepath));
    ifstream file (input_filepath, ifstream::binary);
    file.read(buffer.data(), buffer.size());
    file.close();

    for (size_t i = 0; i < buffer.size(); i++) {
        if (i == 0 || buffer[i-1] == '\n') {
            line_starts.push_back(i);
        }
        if (i == buffer.size() - 1 || buffer[i] == '\n') {
            line_lengths.push_back(i - line_starts.back() + 1);
        }
    }

    return 0;
}

// buffer cleanup
void buf_cleanup() {
    buffer.clear();
    line_starts.clear();
    line_lengths.clear();
}

// move reading position to the next character in the buffer
// - return -2 if eof
// - return  0 otherwise
int buf_next_char() {
    if (buf_eof())
        return -2;
    reading_pos += 1;

    // start of a new line
    if (buffer[reading_pos-1] == '\n') {
        buf_line_pos += 1;
        buf_col_pos = 0;

        if (buf_line_pos > line_starts.size())
            line_starts.push_back(reading_pos);
    }

    buf_col_pos += 1;
    return 0;
}

// move reading position to the previous character in the buffer
// - return  1 if no more to back up
// - return  0 otherwise
int buf_prev_char() {
    // decrement reading_position, return 1 if already at beginning of buffer
    if (reading_pos == 0)
        return 1;

    reading_pos -= 1;
    buf_col_pos -= 1;
    
    // move stored line & col positions to end of previous line if necessary
    if (buf_col_pos == 0)
        buf_col_pos = line_lengths[--buf_line_pos-1];

    return 0;
}

// get character at current position
// - return -2 if current position is eof
// - return  0 otherwise
int buf_get_curr_char(char &ch) {
    if (buf_eof())
        return -2;

    ch = buffer[reading_pos];
    return 0;
}

// move to next position and get the character there
// - return -2 if eof
// - return  0 otherwise
int buf_get_next_char(char &ch) {
    buf_next_char();
    return buf_get_curr_char(ch);
}

// return true / false as to whether the buffer position is end of file
bool buf_eof() {
    return reading_pos >= buffer.size();
}

// return a string 
string buf_getline(int line_number) {
    size_t first = line_starts[line_number-1];
    size_t last  = first + line_lengths[line_number-1];
    return string(buffer.begin()+first, buffer.begin()+last);
}