#include <fstream>
#include <map>
#include <string>

using namespace std;

enum class BlocksEnum {
    AirBlock,
    GrassBlock,
    SandBlock,
    WaterBlock,
    DirtBlock,
    BricksBlock,
    CrackedBricksBlock,
    TreasureBricksBlock,
    TrapBricksBlock,
    END
};

constexpr int chunk_size = 16;

class Chunk {
   public:
    int x = 0;
    int y = 0;
    int blocks[chunk_size][chunk_size];
};

void write_chunk(ofstream &file, Chunk &chunk) {
    int traits[2] = {chunk.x, chunk.y};
    file.write(reinterpret_cast<char*>(traits), sizeof(traits));
    file.write(reinterpret_cast<char*>(chunk.blocks), sizeof(chunk.blocks));
    file.flush();
}

map<char, BlocksEnum> mapping = {{'.', BlocksEnum::AirBlock},
                                 {'#', BlocksEnum::BricksBlock},
                                 {'X', BlocksEnum::CrackedBricksBlock},
                                 {'$', BlocksEnum::TreasureBricksBlock},
                                 {'!', BlocksEnum::TrapBricksBlock},
                                 {'@', BlocksEnum::WaterBlock},
                                 {'G', BlocksEnum::GrassBlock},
                                 {'D', BlocksEnum::DirtBlock},
                                 {'S', BlocksEnum::SandBlock}};

// string chunk_raw = // Land
// "................\
// ................\
// ................\
// ..........DD....\
// ....##...DXXD...\
// ....#!#.DXXXD...\
// .....###XXXD....\
// ......#$X#D.....\
// ......X$$#......\
// .....######.....\
// ....#X#..#!#....\
// ....##....##....\
// ................\
// ................\
// ................\
// ................\
// ";

// string chunk_raw = // Aqua
// "................\
// ................\
// ................\
// ....###@@###....\
// ...#$##@@##$#...\
// ...###@@@@###...\
// ...##@@!#@@##...\
// ...@@@#@@$@@@...\
// ...@@@$@@#@@@...\
// ...##@@#!@@##...\
// ...###@@@@###...\
// ...#!##@@##$#...\
// ....###@@###....\
// ................\
// ................\
// ................\
// ";

string chunk_raw = // Fort
"...@@@@@@@@@@...\
..@@@@@@@@@@@@..\
.@@@@@@@@@@@@@@.\
@@@@@@####@@@@@@\
@@@@@######@@@@@\
@@@@#!####!#@@@@\
@@@##########@@@\
@@@####$$####@@@\
@@@####$$####@@@\
@@@##########@@@\
@@@@#!####!#@@@@\
@@@@@######@@@@@\
@@@@@@####@@@@@@\
.@@@@@@@@@@@@@@.\
..@@@@@@@@@@@@..\
...@@@@@@@@@@...\
";

int main() {
    Chunk chunk;

    for(int y = 0; y < chunk_size; y++) {
        for(int x = 0; x < chunk_size; x++) {
            BlocksEnum block = mapping[chunk_raw[y*chunk_size + x]];
            chunk.blocks[y][x] = static_cast<int>(block);
        }
    }

    ofstream file("structure.chunk", ios::binary);
    write_chunk(file, chunk);

    return 0;
}