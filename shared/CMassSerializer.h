#pragma once

#include <vector>
#include <cstring>

class CMassSerializer
{
public:
    template<typename T>
    static void SerializeToBuffer(const T& data, char* buffer, size_t& offset) {
        std::memcpy(buffer + offset, &data, sizeof(T));
        offset += sizeof(T);
    }

    template<typename T>
    static void DeserializeFromBuffer(const char* buffer, T& data, size_t& offset) {
        std::memcpy(&data, buffer + offset, sizeof(T));
        offset += sizeof(T);
    }

    template<typename T>
    static char* SerializeMessage(size_t count, const std::vector<T>& data, size_t& dataSize) {
        dataSize = sizeof(size_t) + count * sizeof(T);
        char* buffer = new char[dataSize];

        size_t offset = 0;

        // Serialize count
        SerializeToBuffer(count, buffer, offset);

        // Serialize data
        for (const auto& item : data) {
            SerializeToBuffer(item, buffer, offset);
        }

        return buffer;
    }

    template<typename T>
    static std::vector<T> DeserializeMessage(const char* buffer, size_t& count) {
        size_t offset = 0;

        // Deserialize count
        DeserializeFromBuffer(buffer, count, offset);

        // Deserialize data
        std::vector<T> data;
        data.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            T item;
            DeserializeFromBuffer(buffer, item, offset);
            data.push_back(item);
        }

        return data;
    }
};