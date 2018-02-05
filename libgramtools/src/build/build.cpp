#include "common/parameters.hpp"
#include "common/timer_report.hpp"

#include "prg/prg.hpp"
#include "prg/masks.hpp"

#include "kmer_index/kmer_index.hpp"
#include "build/build.hpp"


void commands::build::run(const Parameters &parameters) {
    std::cout << "Executing build command" << std::endl;
    auto timer = TimerReport();

    PRG_Info prg_info;

    std::cout << "Generating integer encoded PRG" << std::endl;
    timer.start("Encoded PRG");
    prg_info.encoded_prg = generate_encoded_prg(parameters);
    timer.stop();
    std::cout << "Number of charecters in integer encoded linear PRG: "
              << prg_info.encoded_prg.size()
              << std::endl;

    std::cout << "Generating FM-Index" << std::endl;
    timer.start("Generate FM-Index");
    prg_info.fm_index = generate_fm_index(parameters);
    timer.stop();

    std::cout << "Generating PRG masks" << std::endl;
    timer.start("Generating PRG masks");
    generate_dna_bwt_masks(prg_info.fm_index, parameters);

    MasksParser masks(parameters.site_mask_fpath);
    prg_info.sites_mask = masks.sites;
    prg_info.max_alphabet_num = masks.max_alphabet_num;

    prg_info.allele_mask = generate_allele_mask(prg_info.encoded_prg);
    sdsl::store_to_file(prg_info.allele_mask, parameters.allele_mask_fpath);

    prg_info.prg_markers_mask = generate_prg_markers_mask(prg_info.encoded_prg);
    prg_info.prg_markers_rank = sdsl::rank_support_v<1>(&prg_info.prg_markers_mask);
    prg_info.prg_markers_select = sdsl::select_support_mcl<1>(&prg_info.prg_markers_mask);

    prg_info.bwt_markers_mask = generate_bwt_markers_mask(prg_info.fm_index);
    prg_info.bwt_markers_rank = sdsl::rank_support_v<1>(&prg_info.bwt_markers_mask);
    prg_info.bwt_markers_select = sdsl::select_support_mcl<1>(&prg_info.bwt_markers_mask);
    prg_info.markers_mask_count_set_bits =
            prg_info.bwt_markers_rank(prg_info.bwt_markers_mask.size());

    prg_info.dna_bwt_masks = load_dna_bwt_masks(prg_info.fm_index, parameters);
    prg_info.rank_bwt_a = sdsl::rank_support_v<1>(&prg_info.dna_bwt_masks.mask_a);
    prg_info.rank_bwt_c = sdsl::rank_support_v<1>(&prg_info.dna_bwt_masks.mask_c);
    prg_info.rank_bwt_g = sdsl::rank_support_v<1>(&prg_info.dna_bwt_masks.mask_g);
    prg_info.rank_bwt_t = sdsl::rank_support_v<1>(&prg_info.dna_bwt_masks.mask_t);
    timer.stop();

    std::cout << "Generating kmer index" << std::endl;
    timer.start("Generate kmer index");
    generate_kmer_index(parameters, prg_info);
    timer.stop();

    timer.report();
}