#include <fstream>
#include <stdio.h>
#include "SearchGraphNodeTest.hpp"
#include "SearchGraphNode.hpp"

SearchGraphNodeTest::SearchGraphNodeTest(): outputFile("testFile.out") {};

void SearchGraphNodeTest::TearDown() {
    remove(outputFile.c_str());
}

std::string SearchGraphNodeTest::getOutputFileContent() {
    std::fstream inFile;
    inFile.exceptions(std::fstream::failbit | std::fstream::badbit);
    inFile.open(outputFile, std::fstream::in);
    return std::string(std::istreambuf_iterator<char>(inFile),
                       std::istreambuf_iterator<char>());
}

/**
 * \test Checks the behavior of OutputData#writeToFile when OutputData#parent
 *       is not set.
 */
TEST_F(SearchGraphNodeTest, NoParent) {
    std::unique_ptr<Tensor<unsigned>> singleEntry(new Tensor<unsigned>());
    const SearchGraphNode noParent(singleEntry);
    noParent.writeToFile(outputFile, 1);

    EXPECT_EQ(getOutputFileContent(), "0\n");
}

/**
 * \test Successfull when OuputData#writeToFile writes own contents
 *       and contents of its' parents
 */
TEST_F(SearchGraphNodeTest, Parent) {
    std::unique_ptr<Tensor<unsigned>> entries1(new Tensor<unsigned>({1, 2}, {0, 0}));
    std::unique_ptr<Tensor<unsigned>> entries2(new Tensor<unsigned>({1, 2}, {1, 1}));
    std::shared_ptr<SearchGraphNode> parent(new SearchGraphNode(entries1));
    SearchGraphNode sut(parent, entries2);

    sut.writeToFile(outputFile, 2);

    EXPECT_EQ(getOutputFileContent(), "1 0\n1 1\n");
}

/** \test More cars than rides ( -> some lines only contain '0' ). */
TEST_F(SearchGraphNodeTest, MultipleCars) {
    std::unique_ptr<Tensor<unsigned>> multipleEntries(new Tensor<unsigned>({2, 2}, {0, 1, 2, 0}));
    const SearchGraphNode sut(multipleEntries);
    const unsigned fleetSize = 4;
    sut.writeToFile(outputFile, fleetSize);

    EXPECT_EQ(getOutputFileContent(), "1 1\n0\n1 0\n0\n");
}

/** \test More rides than cars ( -> lines contain multiple entries ). */
TEST_F(SearchGraphNodeTest, multipleEntries) {
    std::unique_ptr<Tensor<unsigned>> entries1(new Tensor<unsigned>({1, 2}, {0, 0}));
    std::unique_ptr<Tensor<unsigned>> entries2(new Tensor<unsigned>({1, 2}, {0, 1}));
    std::shared_ptr<SearchGraphNode> parent(new SearchGraphNode(entries1));
    SearchGraphNode sut(parent, entries2);

    sut.writeToFile(outputFile, 1);

    EXPECT_EQ(getOutputFileContent(), "2 0 1\n");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
};
