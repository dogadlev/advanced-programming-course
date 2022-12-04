#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <exception>

class Parser
{
public:
    Parser() = default;
    /*
    Конструктор в качестве параметра принимает имя файла.
    Открывает файл, читает данные, отбрасывая пустые строки и комментарии - метод readFile()
    Метод parse() - анализирует "сырые" данные и сохраняет результат в std::map<> data
    */
    Parser(std::string &&fileName)
    {
        readFile(fileName);
        parse();
    }


    /*
    Метод get_value() возвращает значение, которое находит приватный метод findValue().
    */
    template <class T>
    T get_value(std::string &&sectionVar)
    {
        return find_value<T>(sectionVar);
    }

private:
    std::vector<std::string> rawData{};
    std::map<std::string, std::string> data{};

    /*
    Метод readFile() читает и отбрасывает не нужные данные (пустые строки, комментарии, излишние пробелы).
    При возникновении проблем с открытием файла, выбрасывается исключение.
    Файл читается построчно.
    Результат сохраняется в вектор - "сырые" данные.
    По окончании чтения, файл закрывается.
    */
    void readFile(std::string &fileName)
    {
        std::ifstream file(fileName);
        if (!file.is_open())
        {
            throw std::runtime_error("Возникла ошибка при открытии файла. Возможно имя файла задано неверно.");
        }

        std::string buff = "";
        while (!file.eof())
        {
            std::getline(file, buff);
            if (buff[0] != ';' && buff[0] != ' ' && !buff.empty())
            {
                size_t pos = buff.find(";");
                if (pos != std::string::npos)
                    buff.erase(pos, std::string::npos);
                pos = buff.find(" ");
                while (pos != std::string::npos)
                {
                    buff.erase(pos, 1);
                    pos = buff.find(" ");
                }
                rawData.push_back(buff);
            }
        }
        file.close();
    }

    /*
    Метод parse() анализирует вектор rawData и сохраняет результаты в std::map<> data.
    Если в ячейке вектора обнаружена подстрока Section, обнаруживается номер секции, результат сохраняется в переменную sec.
    Если в ячейке вектора не обнаружена подстрока Section, обнаруживается имя переменной и значение.
    В качестве "маркера" используется =.
    После обнаружения имени и значения переменной формируется ключ и данные сохраняются в data.
    */
    void parse()
    {
        std::string sec = "";
        std::string var = "";
        std::string val = "";
        std::string key = "";
        size_t pos = 0;
        for (auto itr = rawData.begin(); itr != rawData.end(); ++itr)
        {
            pos = (*itr).find("Section");
            if (pos != std::string::npos)
            {
                sec = (*itr).substr(pos, (*itr).find_last_of("Section") + 1);
            }
            else
            {
                pos = (*itr).find("=");
                var = (*itr).substr(0, pos);
                val = (*itr).substr(pos + 1, std::string::npos);
                key = sec + "." + var;
                data[key] = val;
            }
        }
    }

    /*
    Метод find_value() возвращает значение переменной.
    Используется ключ формата section.var
    Если ключ не найден, выбрасывается исключение.
    Отсутствие ключа свидетельствует либо об отсутствии сечения, либо об отсутствии запрашиваемой переменной.
    */
    template <class T>
    T find_value(std::string& sectionVar)
    {
        const auto itr = data.find(sectionVar);
        if (itr == data.end())
            throw std::runtime_error("Значение не было обнаружено. Проверьте имя секции и имя переменной.");
        return itr->second;
    }
};

template <>
double Parser::get_value(std::string &&sectionVar)
{
    auto res = stod(Parser::find_value<std::string>(sectionVar));
    return res;
}

template <>
int Parser::get_value(std::string &&sectionVar)
{
    if(sectionVar.find(".") != std::string::npos)
        throw std::invalid_argument("");
    auto res = stoi(Parser::find_value<std::string>(sectionVar));
    return res;
}

int main(int argc, char **argv)
{

    try
    {
        Parser ini_parser("inputs.ini");

        auto value = ini_parser.get_value<int>("Section1.var1");
        std::cout << value << std::endl;
        value += value;
        std::cout << value << std::endl;

        return 0;
    }

    catch (std::runtime_error &err)
    {
        std::cout << err.what();
    }
    catch (std::invalid_argument &err)
    {
        std::cout << "Тип переменной отличается от того, что вы запрашиваете. " << err.what();
    }
    catch (std::out_of_range &err)
    {
        std::cout << "Значение переменной находится за пределами значений типа, что вы запрашиваете " << err.what();
    }
}