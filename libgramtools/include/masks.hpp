#include <vector>
#include <string>
#include <fstream>

#include "utils.hpp"


#ifndef GRAMTOOLS_PARSE_MASKS_H
#define GRAMTOOLS_PARSE_MASKS_H

using SitesMask = std::vector<Marker>;
using AlleleMask = std::vector<AlleleId>;

class MasksParser {
public:
    uint64_t max_alphabet_num;
    std::vector<uint64_t> sites;

    std::vector<AlleleId> allele;

    MasksParser(){};
    MasksParser(const std::string &sites_fname, const std::string &alleles_fname);

private:
    void parse_sites(std::istream &stream);
    void parse_allele(std::istream &stream);
};

#endif //GRAMTOOLS_PARSE_MASKS_H
