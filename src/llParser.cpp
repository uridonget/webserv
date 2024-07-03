#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <map>
#include <stdexcept>

// Define constants
const char NULL_CHAR = '\0';
const char CR = '\r';
const char LF = '\n';
const char SP = ' ';

// HTTP Request Structure
struct HttpRequest
{
    std::string method;
    std::string target;
    std::string version;
    std::map<std::string, std::string> headers;
};

// Custom exception for parsing errors
struct ParseException : public std::runtime_error
{
    ParseException(const std::string &msg) : std::runtime_error(msg) {}
};

// LL Parser for HTTP request
class HttpParser
{
public:
    HttpParser(const std::string &data) : stream(data), currentChar(NULL_CHAR)
    {
        nextChar();
    }

    HttpRequest parse()
    {
        HttpRequest request;
        request.method = parseToken();
        consumeWhitespace();
        request.target = parseToken();
        consumeWhitespace();
        request.version = parseToken();
        consumeCRLF();

        while (currentChar != NULL_CHAR && currentChar != CR)
        {
            auto header = parseHeader();
            request.headers[header.first] = header.second;
        }
        consumeCRLF(); // consume the last CRLF after headers

        return request;
    }

private:
    std::istringstream stream;
    char currentChar;

    void nextChar()
    {
        stream.get(currentChar);
        if (stream.eof())
        {
            currentChar = NULL_CHAR;
        }
    }

    void consumeWhitespace()
    {
        if (currentChar == SP)
        {
            nextChar();
        }
        else
        {
            throw ParseException("Expected SP");
        }
    }

    void consumeCRLF()
    {
        if (currentChar == CR)
        {
            nextChar();
            if (currentChar == LF)
            {
                nextChar();
            }
            else
            {
                throw ParseException("Expected LF after CR");
            }
        }
        else
        {
            throw ParseException("Expected CRLF");
        }
    }

    std::string parseToken()
    {
        std::string token;
        while (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR \
             && currentChar != SP && currentChar != '\t')
        {
            token += currentChar;
            nextChar();
        }
        if (token.size() == 0)
        {
            throw ParseException("Expected token");
        }
        return token;
    }

    std::pair<std::string, std::string> parseHeader()
    {
        std::string name = parseFieldName();
        expect(':');
        nextChar();
        consumeOptionalWhitespace();
        std::string value = parseFieldValue();
        consumeCRLF();
        return (std::make_pair(name, value));
    }

    std::string parseFieldName()
    {
        std::string name;
        while (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR \
             && currentChar != SP && currentChar != '\t' && currentChar != ':')
        {
            name += currentChar;
            nextChar();
        }
        return name;
    }

    std::string parseFieldValue()
    {
        std::string value;
        while (currentChar != CR && currentChar != LF && currentChar != NULL_CHAR)
        {
            value += currentChar;
            nextChar();
        }
        return value;
    }

    void consumeOptionalWhitespace()
    {
        while (currentChar == ' ')
        {
            nextChar();
        }
    }

    void expect(char expected)
    {
        if (currentChar != expected)
        {
            throw ParseException(std::string("Expected '") + expected + "' but found '" + currentChar + "'");
        }
    }
};

int main()
{
    std::string requestStr = "GET /index.html HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    HttpParser parser(requestStr);

    try
    {
        HttpRequest request = parser.parse();
        std::cout << "Method: " << request.method << std::endl;
        std::cout << "Target: " << request.target << std::endl;
        std::cout << "Version: " << request.version << std::endl;
        std::cout << "Headers:" << std::endl;
        for (const auto &header : request.headers)
        {
            std::cout << header.first << ": " << header.second << std::endl;
        }
    }
    catch (const ParseException &e)
    {
        std::cerr << "Parsing error: " << e.what() << std::endl;
    }

    return 0;
}