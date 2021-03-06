#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iterator>
#include <fstream>


struct Film
{
    std::string Name, Producer;
    std::vector<std::string> Genre{ 5 }, State{ 5 };
    unsigned int Year, Rating;
    double Length;
};

bool operator==(const Film &a, const Film &b)
{
    return
        a.Name == b.Name &&
        a.Genre == b.Genre &&
        a.State == b.State &&
        a.Year == b.Year &&
        a.Rating == b.Rating &&
        a.Length == b.Length;
}

std::istream &operator>>(std::istream &in, std::vector<std::string> &v)
{
    for (auto &i : v)
    {
        in >> i;
    }
    return in;
}

std::istream &operator>>(std::istream &in, Film &f)
{
    return in >> f.Name >> f.Producer >> f.State >> f.Genre >> f.Year >> f.Length >> f.Rating;
}

std::ostream &operator<<(std::ostream &out, const std::vector<std::string> &v)
{
    for (size_t i = 0; i < v.size() - 1; ++i)
    {
        out << v[i] << ", ";
    }
    return out << v.back();
}

std::ostream &operator<<(std::ostream &out, Film &f)
{
    return out
        << "\nName: " << f.Name
        << "\nProducer: " << f.Producer
        << "\nGenre: " << f.Genre
        << "\nState: " << f.State
        << "\nRating: " << f.Rating
        << "\nYear: " << f.Year
        << "\nLength: " << f.Length
        << std::endl;
}


namespace HashFunctions
{
    size_t Hash1(const std::string_view &k)
    {
        return k[0] + k[1];
    }

    size_t Hash2(const std::string_view &k)
    {
        return Hash1(k) + 1;
    }

    size_t Hash3(const std::string_view &k)
    {
        return Hash2(k) / 2;
    }

    size_t Hash4(const std::string_view &k)
    {
        return Hash3(k) + 1;
    }

    size_t Hash5(const std::string_view &k)
    {
        return Hash4(k) / 2;
    }

    size_t Hash6(const std::string_view &k)
    {
        return Hash5(k) + 1;
    }

    size_t Hash7(const std::string_view &k)
    {
        return Hash6(k) / 2;
    }
}


class FilmTable
{
private:
    struct Item
    {
        const std::string Key;
        Film Value;
    };

    std::vector<std::unique_ptr<Item>> Table{ 256 };
    std::vector<std::function<size_t(const std::string_view&)>> Func
    {
        HashFunctions::Hash1,
        HashFunctions::Hash2,
        HashFunctions::Hash3,
        HashFunctions::Hash4,
        HashFunctions::Hash5,
        HashFunctions::Hash6,
        HashFunctions::Hash7
    };

public:
    bool Append(const std::string &key, const Film &val);

    Film &operator[](const std::string &key);

    bool Exists(const std::string_view &key);
};

bool FilmTable::Append(const std::string & key, const Film & val)
{
    for (const auto &i : Func)
    {
        const auto currHash = i(key);
        if (currHash >= Table.size())
        {
            Table.resize(currHash + 1);
        }
        if (!Table[currHash])
        {
            Table[currHash] = std::make_unique<Item>(Item{ key, val });
            return true;
        }
    }
    return false;
}

Film &FilmTable::operator[](const std::string &key)
{
    for (const auto &i : Func)
    {
        const auto currHash = i(key);
        if (currHash >= Table.size())
        {
            Table.resize(currHash + 1);
            Table[currHash] = std::make_unique<Item>(Item{ key });
            return Table[currHash]->Value;
        }
        if (const auto &item = Table[currHash]; item && item->Key == key)
        {
            return item->Value;
        }
    }
}

bool FilmTable::Exists(const std::string_view & key)
{
    for (const auto &i : Func)
    {
        const auto currHash = i(key);
        if (currHash >= Table.size())
        {
            return false;
        }
        if (const auto &item = Table[currHash]; item && item->Key == key)
        {
            return true;
        }
    }
    return false;
}


int main()
{
    setlocale(LC_ALL, "");
    std::ifstream test("TestData.txt");
    FilmTable hashTable;
    for (Film i; test >> i;)
    {
        hashTable.Append(*std::istream_iterator<std::string>(test), i);
    }
    for (std::string i; std::cout << ">>> ", std::cin >> i;)
    {
        std::cout << hashTable[i] << std::endl;
    }
}
