/*
 * This program implements Wordcount utility.
 * This utility works only with ENGLISH letters and words.
 * It has 4 commands/options
 * 1) -l --lines - count lines in file
 * 2) -w --words - count words in file
 * 3) -c --bytes - count size of file in bytes
 * 4) -m --chars - count letters in file
 *
 * In this labwork I think that
 * 1) Word is not empty of any symbols without separators
 * 2) Letter is english alphabet symbol
 * 3) All files has one line at minimum
 */


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <regex>

struct Option {
    explicit Option(bool isSelected = false, size_t (*func)(std::ifstream& file) = nullptr) {
        this->isSelected = isSelected;
        this->func = func;
    }

    bool getIsSelected() const {
        return isSelected;
    }

    void setIsSelected(bool newValue) {
        isSelected = newValue;
    }

    size_t getFunctionResult(std::ifstream& file) const {
        if (func == nullptr)
            throw std::runtime_error("Function must be initialized");
        return func(file);
    }

private:
    bool isSelected;
    size_t (*func)(std::ifstream& file);
};

size_t CountBytes(std::ifstream& file)
{
    size_t bytesCount = 0;

    while (true)
    {
        file.get();
        if (file.eof())
            break;
        ++bytesCount;
    }

    return bytesCount;
}

size_t CountLines(std::ifstream& file)
{
    size_t linesCount = 1;
    char currentSymbol;

    while (!file.eof())
    {
        currentSymbol = (char) file.get();
        if (currentSymbol == '\n')
            ++linesCount;
    }

    return linesCount;
}

size_t CountWords(std::ifstream& file)
{
    size_t wordsCount = 0;
    char lastSymbol = static_cast<char>(file.get());
    char currentSymbol = static_cast<char>(file.get());

    while (lastSymbol != EOF)
    {
        if (!std::isspace(lastSymbol) && std::isspace(currentSymbol))
        {
            ++wordsCount;
        }
        lastSymbol = currentSymbol;
        currentSymbol = static_cast<char>(file.get());
    }

    return wordsCount;
}

size_t CountLetters(std::ifstream& file)
{
    size_t lettersCounter = 0;
    char currentSymbol;

    while (!file.eof())
    {
        currentSymbol = (char) file.get();
        if (std::isalpha(currentSymbol))
            ++lettersCounter;
    }

    return lettersCounter;
}

bool CheckFileName(const char* candidate)
{
    // For file parsing I use regex
    // This pattern say that: we can have any sequence of letters
    // then we need to have dot
    // and then any characters like a file extension
    std::string optionPattern = "^.*\\..*$";
    return std::regex_match(candidate, std::regex(optionPattern));
}

struct CommandLineOptions {
private:
    std::vector<Option> options = {
            Option{false, CountLines},
            Option{false, CountWords},
            Option{false, CountBytes},
            Option{false, CountLetters},
    };

    void checkIndex(size_t index) const {
        if (index >= options.size())
            throw std::invalid_argument("Index out of range");
    }

    void parseOptions(const std::string& option) {
        if (option == "l" || option == "--lines") {
            options[0].setIsSelected(true);
        } else if (option == "w" || option == "--words") {
            options[1].setIsSelected(true);
        } else if (option == "c" || option == "--bytes") {
            options[2].setIsSelected(true);
        } else if (option == "m" || option == "--chars") {
            options[3].setIsSelected(true);
        }
    }

public:
    Option getOption(size_t index) const {
        checkIndex(index);
        return options[index];
    }

    size_t getOptionsCount() const {
        return options.size();
    }

    void saveOptions(int argc, const char* argv[]) {
        int validOptions = 0;
        for (int i = 1; i < argc; ++i)
        {
            if (CheckFileName(argv[i]))
                continue;

            ++validOptions;

            // This part handle situations like this
            // -lwm -mc --bytes and others
            // we think that every valid option has length 2 at least
            if (argv[i][0] == '-' && argv[i][1] == '-') {
                parseOptions(std::string(argv[i]));
                continue;
            }

            size_t optionLength = strlen(argv[i]);
            for (int j = 1; j < optionLength; ++j) {
                parseOptions(std::string(1, argv[i][j]));
            }
        }

        if (validOptions != 0)
            return;

        // if we have zero options
        // we need to show -l, -w, -c
        options[0].setIsSelected(true);
        options[1].setIsSelected(true);
        options[2].setIsSelected(true);
    }

};

void SaveFile(std::vector<std::string>& files, int argc, const char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        if (CheckFileName(argv[i]))
            files.emplace_back(argv[i]);
    }
}

void ExecuteOptions(const std::vector<std::string>& files, const CommandLineOptions& options) {
    for (const auto& filePath : files) {
        std::ifstream file;
        file.open(filePath, std::ios::binary);

        if (!file.is_open()) {
            std::cout << "Could not find file in path ";
        }
        else {
            for (size_t currentOptionIndex = 0; currentOptionIndex < options.getOptionsCount(); ++currentOptionIndex) {
                Option currentOption = options.getOption(currentOptionIndex);
                if (!currentOption.getIsSelected())
                    continue;
                std::cout << currentOption.getFunctionResult(file) << " ";
                file.clear();
                file.seekg(0);
            }
        }

        file.close();
        std::cout << filePath << std::endl;
    }
}

int main(int argc, const char* argv[])
{
    std::vector<std::string> filesPaths;

    // We think that we have only 4 options
    // 1) optionsType::lines - count lines
    // 2) optionsType::words - count words
    // 3) optionsType::bytes - count bytes
    // 4) optionsType::letters - count letters
    CommandLineOptions options;

    SaveFile(filesPaths, argc, argv);
    options.saveOptions(argc, argv);

    ExecuteOptions(filesPaths, options);

    return 0;
}
