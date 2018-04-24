#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <memory>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <cstdlib>
#include <xmmintrin.h>
#include "ffm.h"

using namespace std;
using namespace ffm;

struct Option {
    string  model_path, output_path;
    int field_id;
};

string convert_help() {
    return string(
"usage: ffm-convert model_file output_file field_id\n"
);
}

Option parse_option(int argc, char **argv) {
    vector<string> args;
    for(int i = 0; i < argc; i++)
        args.push_back(string(argv[i]));

    if(argc == 1)
        throw invalid_argument(convert_help());

    Option option;

    if(argc != 4)
        throw invalid_argument("cannot parse argument");

    option.model_path = string(args[1]);
    option.output_path = string(args[2]);
    option.field_id = atoi(args[3].c_str());
    

    return option;
}


#if defined USESSE
void convert(string model_path, string output_path, int field_id) {
    ofstream f_out(output_path);
    ffm_model model = ffm_load_model(model_path);
    ffm_int align0 = 2 * get_k_aligned(model.k);
    ffm_int align1 = model.m * align0;
    cout << "align 0 (size per fields per feature)" << align0 << endl;
    cout << "align 1 (size per feature)" << align1 << endl;
    cout << "feature # " << model.n << endl;
    cout << "field # " << model.m << endl;
    cout << "vector dimension (k) # " << model.k << endl;
    cout << "kALIGN " << kALIGN << endl;


    for(int f = 0; f < model.n ; f ++){
        int j = field_id;
        f_out << f << '\t' << j;
        ffm_float *w = model.W + (ffm_long)f*align1 + j*align0;
        for(ffm_int d = 0; d < align0; d += kALIGN * 2){
            __m128  XMMw1 = _mm_load_ps(w+d);
            float result[4];
            _mm_store_ps(result, XMMw1);
            for(int kk = 0; kk < 4; kk ++) {
                f_out  << '\t' << result[kk];
            }
        }
        f_out << endl;
    }
}

#else
void convert(string model_path, string output_path, int field_id) {
    ofstream f_out(output_path);
    ffm_model model = ffm_load_model(model_path);
    ffm_int align0 = 2 * get_k_aligned(model.k);
    ffm_int align1 = model.m * align0;
    cout << "align 0 (size per fields per feature)" << align0 << endl;
    cout << "align 1 (size per feature)" << align1 << endl;
    cout << "feature # " << model.n << endl;
    cout << "field # " << model.m << endl;
    cout << "vector dimension (k) # " << model.k << endl;
    cout << "kALIGN " << kALIGN << endl;


    for(int f = 0; f < model.n ; f ++){
        int j = field_id;
        f_out << f << '\t' << j;
        ffm_float *w = model.W + (ffm_long)f*align1 + j*align0;
        for(ffm_int d = 0; d < align0; d += kALIGN * 2){
            f_out  << '\t' << w[d];
        }
        f_out << endl;
    }
}
#endif

int main(int argc, char **argv) {
    Option option;
    try {
        option = parse_option(argc, argv);
    } catch(invalid_argument const &e) {
        cout << e.what() << endl;
        return 1;
    }

    convert(option.model_path, option.output_path, option.field_id);

    return 0;
}
