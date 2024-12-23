#include <capnp/serialize.h>
#include "map.capnp.h" // Generated from the schema
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>

#include <chrono>

// Function to serialize the map
void serializeMap(const std::unordered_map<size_t, std::vector<int>>& inputMap, std::string filename) {
    ::capnp::MallocMessageBuilder message;
    Map::Builder mapBuilder = message.initRoot<Map>();
    auto entries = mapBuilder.initEntries(inputMap.size());
    
    size_t index = 0;
    for (const auto& [key, values] : inputMap) {
        auto entry = entries[index++];
        entry.setKey(key);

        auto capnValues = entry.initValues(values.size());
        for (size_t i = 0; i < values.size(); ++i) {
            capnValues.set(i, values[i]);
        }
    }

    // Serialize the message to a flat array
    auto serializedArray = ::capnp::messageToFlatArray(message);
    auto arrayPtr = serializedArray.asBytes();

    // Write the serialized data to a file
    std::ofstream
        file(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    file.write(reinterpret_cast<const char*>(arrayPtr.begin()), arrayPtr.size());
    file.close();

}


// Function to deserialize the map
std::unordered_map<size_t, std::vector<int>> deserializeMap(std::string filename) {

    // Read the serialized data from a file into const std::vector<uint8_t>& data
    std::ifstream file(filename);
    std::vector<uint8_t> data(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    // Interpret the serialized data as a Cap'n Proto message
    ::capnp::FlatArrayMessageReader message(
        ::kj::ArrayPtr<const ::capnp::word>(reinterpret_cast<const ::capnp::word*>(data.data()),
                                            data.size() / sizeof(::capnp::word))
    );
    Map::Reader mapReader = message.getRoot<Map>();

    std::unordered_map<size_t, std::vector<int>> outputMap;
    outputMap.reserve(mapReader.getEntries().size());
    for (auto entry : mapReader.getEntries()) {
        size_t key = entry.getKey();
        std::vector<int> values;
        auto capnValues = entry.getValues();

        // Manually copy elements from Cap'n Proto List to std::vector
        values.reserve(capnValues.size());
        for (auto value : capnValues) {
            values.push_back(value);
        }

        outputMap[key] = std::move(values);
    }

    return outputMap;
}


int main() {
    // Example data
    std::unordered_map<size_t, std::vector<int>> exampleMap = {
        {1, {10, 20, 30}},
        {2, {40, 50}},
        {3, {60}}
    };

    // append 1 million elements to the map
    for (size_t i = 4; i < 1000004; i++) {
        exampleMap[i] = {(int)i};
    }

    // filename
    std::string filename = "map.bin";

    // Serialize
    auto start_time = std::chrono::high_resolution_clock::now();
    serializeMap(exampleMap, filename);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    // show time in milliseconds
    std::cout << "Time taken to write: " << elapsed.count() << " s" << std::endl;

    // Deserialize
    start_time = std::chrono::high_resolution_clock::now();
    auto deserializedMap = deserializeMap(filename);
    end_time = std::chrono::high_resolution_clock::now();
    elapsed = end_time - start_time;
    // show time in milliseconds
    std::cout << "Time taken to read: " << elapsed.count() << " s" << std::endl;


    // now check how long it would take to manually write and read the same data
    start_time = std::chrono::high_resolution_clock::now();
    std::ofstream
        file(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    for (const auto& [key, values] : exampleMap) {
        file.write(reinterpret_cast<const char*>(&key), sizeof(key));
        size_t size = values.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        file.write(reinterpret_cast<const char*>(values.data()), size * sizeof(int));
    }
    file.close();
    end_time = std::chrono::high_resolution_clock::now();
    elapsed = end_time - start_time;
    // show time in milliseconds
    std::cout << "Time taken to write manually: " << elapsed.count() << " s" << std::endl;

    start_time = std::chrono::high_resolution_clock::now();
    std::ifstream file2(filename);
    std::unordered_map<size_t, std::vector<int>> outputMap;
    while (file2) {
        size_t key;
        file2.read(reinterpret_cast<char*>(&key), sizeof(key));
        if (!file2) {
            break;
        }
        size_t size;
        file2.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::vector<int> values(size);
        file2.read(reinterpret_cast<char*>(values.data()), size * sizeof(int));
        outputMap[key] = std::move(values);
    }
    file2.close();
    end_time = std::chrono::high_resolution_clock::now();
    elapsed = end_time - start_time;
    // show time in milliseconds
    std::cout << "Time taken to read manually: " << elapsed.count() << " s" << std::endl;


    return 0;
}
