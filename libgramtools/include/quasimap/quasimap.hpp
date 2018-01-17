#include "parameters.hpp"
#include "quasimap/coverage/types.hpp"


#ifndef GRAMTOOLS_QUASIMAP_HPP
#define GRAMTOOLS_QUASIMAP_HPP


struct QuasimapReadsStats {
    uint64_t all_reads_count = 0;
    uint64_t skipped_reads_count = 0;
    uint64_t mapped_reads_count = 0;
};

QuasimapReadsStats quasimap_reads(const Parameters &parameters,
                                  const KmerIndex &kmer_index,
                                  const PRG_Info &prg_info);

void quasimap_forward_reverse(QuasimapReadsStats &quasimap_reads_stats,
                              Coverage &coverage,
                              const Pattern &read,
                              const Parameters &parameters,
                              const KmerIndex &kmer_index,
                              const PRG_Info &prg_info);

bool quasimap_read(const Pattern &read,
                   Coverage &coverage,
                   const KmerIndex &kmer_index,
                   const PRG_Info &prg_info,
                   const Parameters &parameters);

void record_allele_base_coverage(Coverage &coverage,
                                 const SearchStates &search_states,
                                 const uint64_t &read_length,
                                 const PRG_Info &prg_info);

void record_read_coverage(Coverage &coverage,
                          const SearchStates &search_states,
                          const uint64_t &read_length,
                          const PRG_Info &prg_info);

void dump_coverage(const Coverage &coverage,
                   const Parameters &parameters);

Coverage generate_coverage_structure(const PRG_Info &prg_info);

SitesAlleleBaseCoverage generate_base_coverage_structure(const PRG_Info &prg_info);

Pattern get_kmer_from_read(const uint32_t kmer_size, const Pattern &read);

uint64_t inter_site_base_count(const uint64_t &first_site_marker,
                               const uint64_t &second_site_marker,
                               const PRG_Info &prg_info);

#endif //GRAMTOOLS_QUASIMAP_HPP
