#include <algorithm>
#include <cassert>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

struct File {
    int id;
    std::string name;
    std::vector<std::string> categories;
    int numCategories;
    int parent;
    int size;
};

/**
 * Task 1
 */
std::vector<std::string> leafFiles(std::vector<File> files) {
    std::unordered_map<int, bool> hasChild;
    for (auto& file : files) {
        // mark the parent as having a child
        hasChild[file.parent] = true;
    }

    std::vector<std::string> leafFileNames;
    for (auto& file: files) {
        // if file does not have a child then it is a leaf node
        if (!hasChild[file.id]) {
            leafFileNames.push_back(file.name);
        }
    }

    return leafFileNames;
}

/**
 * Task 2
 */
std::vector<std::string> kLargestCategories(std::vector<File> files, int k) {
    std::unordered_map<std::string, int> categoriesCounter;
    // std::unordered_map<std::string, int> categoriesSize;
    for (auto& file : files) {
        for (auto& category : file.categories) {
            categoriesCounter[category]++;
            // categoriesSize[category] += file.size;
        }
    }
    // move map into a vector with pair-elems
    std::vector<std::pair<std::string, int>> categoriesVector(categoriesCounter.begin(), categoriesCounter.end());
    // stable sort alphabetically (ascending)
    std::stable_sort(categoriesVector.begin(), categoriesVector.end());
    // stable sort by occurrances (decreasing)
    std::stable_sort(categoriesVector.begin(), categoriesVector.end(), [](auto &left, auto &right) {
        return left.second > right.second;
    });

    // move the first-k into the return list
    std::vector<std::string> kLargestCategoriesNames(k);
    int i = 0;
    for (auto& elem : categoriesVector) {
        if (i == k) break;
        kLargestCategoriesNames[i] = elem.first;
        i++;
    }
    return kLargestCategoriesNames;
}




/**
 * Task 3
 */
int calculateSize(std::unordered_map<int, std::vector<int>> fileChildren, std::unordered_map<int, int> singleFileSize, int node, std::unordered_map<int, int>& Cache) {
    if (fileChildren[node].empty()) {
        // std::cout << "leaf: " << node << '\n';
        Cache[node] = singleFileSize[node];
        return singleFileSize[node];
    } else {
        if (Cache[node]) {
            return Cache[node];
        } else {
            int sum = 0;
            for (auto& child : fileChildren[node]) {
                sum += calculateSize(fileChildren, singleFileSize, child, Cache);
            }
            Cache[node] = sum;
            return sum;
        }
    }
}

bool compare(const std::pair<int, int>& a, const std::pair<int, int>& b) {
   return a.second < b.second;
}

int largestFileSize(std::vector<File> files) {
    std::unordered_map<int, std::vector<int>> fileChildren;
    std::unordered_map<int, int> singleFileSize;
    for (auto& file : files) {
        singleFileSize[file.id] = file.size;
        if (file.parent == -1) {
            continue;
        }
        fileChildren[file.parent].push_back(file.id);
    }

    // for (auto& elem : fileChildren) {
    //     std::cout << elem.first << " has children ";
    //     for (auto& v : elem.second) {
    //         std::cout << v << " ";
    //     } 
    //     std::cout << '\n';
    // }

    std::unordered_map<int, int> Cache;
    for (auto& file : files) {
        if (file.parent == -1) {
            for (auto& child : fileChildren[file.id]) {
                // std::cout << "parent: " << file.id << " child: " << child << '\n';
                Cache[file.id] += calculateSize(fileChildren, singleFileSize, child, Cache);
            }
        }
    }
    for (auto& file : files) {
        if (!Cache[file.id]) {
            Cache[file.id] = file.size;
        }
    }

    // for (auto& elem : Cache) {
    //     std::cout << elem.first << " " << elem.second << '\n';
    // }

    int max = max_element(Cache.begin(), Cache.end(), compare)->second;
    return max;
}



int main(void) {
    std::vector<File> testFiles{
        { .id = 1, .name = "Document.txt", .categories = {"Documents"}, .numCategories = 1, .parent = 3, .size = 1024 },
        { .id = 2, .name = "Image.jpg", .categories = {"Media", "Photos"}, .numCategories = 2, .parent = 34, .size = 2048 },
        { .id = 3, .name = "Folder", .categories = {"Folder"}, .numCategories = 1, .parent = -1, .size = 0 },
        { .id = 5, .name = "Spreadsheet.xlsx", .categories = {"Documents", "Excel"}, .numCategories = 2, .parent = 3, .size = 4096 },
        { .id = 8, .name = "Backup.zip", .categories = {"Backup"}, .numCategories = 1, .parent = 233, .size = 8192 },
        { .id = 13, .name = "Presentation.pptx", .categories = {"Documents", "Presentation"}, .numCategories = 2, .parent = 3, .size = 3072 },
        { .id = 21, .name = "Video.mp4", .categories = {"Media", "Videos"}, .numCategories = 2, .parent = 34, .size = 6144 },
        { .id = 34, .name = "Folder2", .categories = {"Folder"}, .numCategories = 1, .parent = 3, .size = 0 },
        { .id = 55, .name = "Code.py", .categories = {"Programming"}, .numCategories = 1, .parent = -1, .size = 1536 },
        { .id = 89, .name = "Audio.mp3", .categories = {"Media", "Audio"}, .numCategories = 2, .parent = 34, .size = 2560 },
        { .id = 144, .name = "Spreadsheet2.xlsx", .categories = {"Documents", "Excel"}, .numCategories = 2, .parent = 3, .size = 2048 },
        { .id = 233, .name = "Folder3", .categories = {"Folder"}, .numCategories = 1, .parent = -1, .size = 4096 },
    };

    std::vector<std::string> expectedLeafFiles{
        "Audio.mp3",
        "Backup.zip",
        "Code.py",
        "Document.txt",
        "Image.jpg",
        "Presentation.pptx",
        "Spreadsheet.xlsx",
        "Spreadsheet2.xlsx",
        "Video.mp4"
    };
    std::vector<std::string> returnedLeafFiles = leafFiles(testFiles);
    std::sort(returnedLeafFiles.begin(), returnedLeafFiles.end());
    assert(expectedLeafFiles == returnedLeafFiles);

    std::vector<std::string> expectedCategories{"Documents", "Folder", "Media"};
    std::vector<std::string> returnedCategories = kLargestCategories(testFiles, 3);
    assert(expectedCategories == returnedCategories);

    assert(largestFileSize(testFiles) == 20992);

}