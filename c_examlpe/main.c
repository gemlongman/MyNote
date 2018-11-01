#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "wavetrans.h"

#define gy_debug

#ifdef gy_debug
    int staticI = 0;
#endif

extern char *optarg;

enum runType {nosiy,equalizer,trans,other};

enum runType run_type=other;
/*
struct noise_reduction_context {
    long double **noise;
    long double alfa, beta;
};

void trans_noise_reduction(void *context, int n, int channel, t_complex *data) {
    struct noise_reduction_context *ctx = context;
    int size = 1 << n;
    int i;
    long double mod, est_mod;
    long long coef;
    
    for(i = 0; i < size; i++) {
        mod = sqrtl((long double)data[i].r * data[i].r +
                (long double)data[i].i * data[i].i) / (1LL << (2 * COMPLEX_PRECISION));
        if(mod < 0.0000001) // underflow protection
            continue;
        if(ctx->alfa == 1) {
            est_mod = mod - ctx->beta * ctx->noise[channel][i];
            est_mod = est_mod > 0 ? est_mod : 0;
        } else {
            est_mod = powl(mod, ctx->alfa) - ctx->beta * powl(ctx->noise[channel][i], ctx->alfa);
            est_mod = est_mod > 0 ? powl(est_mod, 1 / ctx->alfa) : 0;
        }
        coef = (est_mod / mod) * (1 << COMPLEX_PRECISION);
        data[i].r = data[i].r * coef / (1 << COMPLEX_PRECISION);
        data[i].i = data[i].i * coef / (1 << COMPLEX_PRECISION);
    }
}
*/
#define MAX_EQ_CELLS 128
struct equalizer_context {
    double f[MAX_EQ_CELLS];
    double a[MAX_EQ_CELLS];
    double q[MAX_EQ_CELLS];
    t_complex *data;

    int ncells;
};

void trans_equalizer(void *context, int n, int channel, t_complex *data) {
    struct equalizer_context *ctx = context;
    int size = 1 << n;// = 1 << 12 = 4096
    int i;

    for(i = 0; i < size; i++) {
#ifdef gy_debug
        //int size = 4096; //c->size
        int samples_per_sec = 44100; //c->wave_fmt.samples_per_sec 
        int f = ( i <= size / 2 ) ? (double) i / size * samples_per_sec : (double) ( size - i ) / size * samples_per_sec;
        // // fprintf(stdout, "\n %d \t\t %lld \t %lld \t * \t %lld \t %lld", i, data[i].r, data[i].i, ctx->data[i].r,  ctx->data[i].i);
        //// print i f r i r+i r^2+i^2
        fprintf(stdout, "%d\t%d\t%lld\t%lld\t%lld\t%lld", i, f, data[i].r/ (1 << COMPLEX_PRECISION), data[i].i/ (1 << COMPLEX_PRECISION) , data[i].r/ (1 << COMPLEX_PRECISION)+data[i].i/ (1 << COMPLEX_PRECISION) , data[i].r/ (1 << COMPLEX_PRECISION)*data[i].r/ (1 << COMPLEX_PRECISION) + data[i].i/ (1 << COMPLEX_PRECISION) * data[i].i/ (1 << COMPLEX_PRECISION));
#endif
        data[i].r = data[i].r * ctx->data[i].r / (1 << COMPLEX_PRECISION);
        data[i].i = data[i].i * ctx->data[i].r / (1 << COMPLEX_PRECISION);

#ifdef gy_debug
        // //fprintf(stdout, "\n %d \t\t %lld \t %lld \t * \t %lld \t %lld", i, data[i].r, data[i].i, ctx->data[i].r,  ctx->data[i].i);
        // print continue r i r+i r^2+i^2
        fprintf(stdout, "\t%lld\t%lld\t%lld\t%lld\n", data[i].r/ (1 << COMPLEX_PRECISION), data[i].i/ (1 << COMPLEX_PRECISION), data[i].r/ (1 << COMPLEX_PRECISION)+ data[i].i/ (1 << COMPLEX_PRECISION) , data[i].r/ (1 << COMPLEX_PRECISION)*data[i].r/ (1 << COMPLEX_PRECISION) + data[i].i/ (1 << COMPLEX_PRECISION) * data[i].i/ (1 << COMPLEX_PRECISION));
#endif
    }
}

void trans_dummy(void *context, int n, int channel, t_complex *data) { /* Dummy run (no transform) 虚拟运行（不改变） */
}

struct global_context {
    /* variables for fourier transformation 傅里叶变换 */
    int n, size;
    int *rev_map;
    t_complex *w;

    /* variables for the format wave/pcm */
    int channels;
    int interleave;
    int bytes_per_sample;

    t_riff_hdr riff_hdr;
    t_wave_format_ex wave_fmt;

    t_complex_promote complex_promote;
    t_complex_reduce complex_reduce;

    int frame_len;
    char pad_value[4];
    unsigned long wave_len;
};

void global_context_init(struct global_context *c) {
    c->n = 12;
    c->complex_promote = NULL;
    c->complex_reduce = NULL;
}

int riff_read(struct global_context *c, FILE *in) {
    t_chunk tmp_chunk;
    unsigned long data_len = 0;
    int status;

    c->wave_len = 0;

    /* reading riff header */
    status = fread(&c->riff_hdr, sizeof(t_riff_hdr), 1, in);
    assert(status);

    if(strncmp(c->riff_hdr.chunk.id, "RIFF", 4)) {
        fprintf(stderr, "Input data is not RIFF\n");
        return 1;
    }

    if(strncmp(c->riff_hdr.file_type, "WAVE", 4)) {
        fprintf(stderr, "Unknown format '%4s'\n", c->riff_hdr.file_type);
        return 2;
    }

    data_len = c->riff_hdr.chunk.size;

    /* the rest of the data chunk sites are specific wave */
    do {
        assert(data_len >= sizeof(t_chunk));
        status = fread(&tmp_chunk, sizeof(t_chunk), 1, in);
        //debug :windows 0
        assert(status);
        data_len -= sizeof(t_chunk);
        if(!strncmp(tmp_chunk.id, "fmt ", 4)) {
            assert(tmp_chunk.size <= sizeof(t_wave_format_ex));
            memset(&c->wave_fmt, 0, sizeof(t_wave_format_ex));
            assert(data_len >= tmp_chunk.size);
            status = fread(&c->wave_fmt, tmp_chunk.size, 1, in);
            assert(status);
            data_len -= tmp_chunk.size;

            if(c->wave_fmt.format_tag != WAVE_FORMAT_PCM) {
                fprintf(stderr, "Unsupported wave format %d\n",
                        (int)c->wave_fmt.format_tag);
                return 3;
            }

            fprintf(stdout, "WAVE PCM format detected\n");
            fprintf(stdout, "%d bit audio samples\n",
                    (int)c->wave_fmt.bits_per_sample);
            fprintf(stdout, "%d channel(s), %d Hz\n",
                    (int)c->wave_fmt.channels,
                    (int)c->wave_fmt.samples_per_sec);

            /* interleave calculation */
            c->channels = c->wave_fmt.channels;
            c->bytes_per_sample = c->wave_fmt.bits_per_sample / 8;
            c->interleave = c->channels * c->bytes_per_sample;

            switch(c->wave_fmt.bits_per_sample) {
            case 8:
                c->complex_promote = complex_promote_u8;
                c->complex_reduce = complex_reduce_u8;
                *(unsigned char *)(c->pad_value) = 128;
                break;
            case 16:
                c->complex_promote = complex_promote_s16;
                c->complex_reduce = complex_reduce_s16;
                *(t_s16 *)(c->pad_value) = 0;
                break;
            default:
                fprintf(stderr, "Unsupported sample type!\n");
                return 4;
            }

            continue;
        }

        if(!strncmp(tmp_chunk.id, "data", 4)) {
            assert(data_len >= tmp_chunk.size);
            c->wave_len = tmp_chunk.size;
            break;
        }

        /*jump over any other kind of chunk (which i don't understand)*/
        assert(data_len >= tmp_chunk.size);
        status = fseek(in, tmp_chunk.size, SEEK_CUR);
        assert(!status);
        data_len -= tmp_chunk.size;
    } while(1);

    c->size = 1 << c->n;
    c->frame_len = c->interleave * c->size;
    return 0;
}

void riff_write(struct global_context *c, FILE *out) {
    t_chunk wave_fmt_chunk;
    t_chunk tmp_chunk;
    int status;

    /* Output header riff, format wave and chunk date */
    /* FIXME  calculation length data */
    status = fwrite(&c->riff_hdr, sizeof(t_riff_hdr), 1, out);
    assert(status);

    strncpy(wave_fmt_chunk.id, "fmt ", 4);
    wave_fmt_chunk.size = sizeof(t_wave_format_ex);
    status = fwrite(&wave_fmt_chunk, sizeof(t_chunk), 1, out);
    assert(status);
    status = fwrite(&c->wave_fmt, sizeof(t_wave_format_ex), 1, out);
    assert(status);

    strncpy(tmp_chunk.id, "data", 4);
    tmp_chunk.size = c->wave_len;
    status = fwrite(&tmp_chunk, sizeof(t_chunk), 1, out);
    assert(status);


}

void init_fft_maps(struct global_context *c) {
    /* preparation for tables and memory buffers 工作表和内存缓冲区 */
    c->rev_map = (int *)malloc(c->size * sizeof(int));
    assert(c->rev_map != NULL);
    c->w = (t_complex *)malloc(c->size * sizeof(t_complex));
    assert(c->w != NULL);

    bit_reverse_map(c->n, c->rev_map);
    w_map(c->n, c->w);
}

void free_fft_maps(struct global_context *c) {
    free(c->rev_map);
    free(c->w);
}

void read_eq_cells(struct equalizer_context *c, FILE *in) {
    char buf[1024], *comment;
    c->ncells = 0;
    while(fgets(buf, sizeof(buf), in) != NULL) {
        assert(strlen(buf) < sizeof(buf) - 1);
        if((comment = strchr(buf, '#')) != NULL)
            *comment = '\0';
        if(sscanf(buf, "%lf%lf%lf", &c->f[c->ncells],
                    &c->a[c->ncells], &c->q[c->ncells]) != 3)
            continue;
        if(++(c->ncells) >= MAX_EQ_CELLS){
            fprintf(stderr, "Only read %d lines\n", MAX_EQ_CELLS);
            break;
        }
    }
    fprintf(stdout, "Successfully read %d equalizer cells\n", c->ncells);
}

void read_trans_loss_cells(struct equalizer_context *c, FILE *in) {
    char buf[1024], *comment;
    c->ncells = 0;
    double f,tl;// Transmission loss = 10 * log10(Pi/Po)
    int i= 0;
    double sumAmplitude = 0.0;// for  weighted by their amplitude
    while(fgets(buf, sizeof(buf), in) != NULL) {
        assert(strlen(buf) < sizeof(buf) - 1);
        if((comment = strchr(buf, '#')) != NULL)
            *comment = '\0';
        if(sscanf(buf, "%lf%lf",&f,&tl) != 2)
            continue;
        c->f[c->ncells] = f;
        c->a[c->ncells] = 1/powl(10,tl/20);// Po/Pi = 1 / 10 ^ (loss / 20)
        sumAmplitude += c->a[c->ncells];// 
        c->q[c->ncells] = 1;
        if(++(c->ncells) >= MAX_EQ_CELLS){
            fprintf(stderr, "Only read %d lines\n", MAX_EQ_CELLS);
            break;
        }
    }
    // equally average weight ( 1 / c->ncells )or weighted by their amplitude( c->a[c->ncells] / sumAmplitude )
    for( i= 0; i < c->ncells; i++ ) {
        //c->q[ i ] = c->a[i] / sumAmplitude;
        fprintf(stdout, "%lf\t%lf\t%lf\n",c->f[i], c->a[i], c->q[i]);
    }
    //fprintf(stdout, "Successfully read %d trans equalizer cells\n", c->ncells);
}

void compute_equalizer_data(struct global_context *c, struct equalizer_context *e) {
    /* for each spectral component calculates amplification depending on the parameters of egalizatorului vectors f, a, q of the context e */
    int i, j;
    double *adb;
    double log10 = log(10);
    double k;
    double prod;
    double adbi_k;

    adb = alloca(e->ncells * sizeof(double));
    /* precalculate amplification in decibels 分贝 for each cell */
    for(i = 0; i < e->ncells; i++)
        adb[i] = 10 * log(e->a[i]) / log10;
    /* global_context c */
    for(j = 0; j < c->size; j++) {
        /* calculate the real k component j depending on the frequency of sampling */
        k = ( j <= c->size / 2 ) ?
            (double) j / c->size * c->wave_fmt.samples_per_sec :
            (double) ( c->size - j ) / c->size * c->wave_fmt.samples_per_sec;
        /* v */
        prod = 1;
        for(i = 0; i < e->ncells; i++) {
            adbi_k = adb[i] * exp( -fabs( log( k / e->f[i] ) ) * e->q[i] );
            prod *= pow(10, adbi_k / 10);
        }
        /* fprintf(stderr, "%lf\n", prod); */
        e->data[j].r = prod * (1 << COMPLEX_PRECISION);
        e->data[j].i = 0;
    }
}

void compute_trans_data(struct global_context *c, struct equalizer_context *e) {
    /* for each spectral component calculates amplification depending on the parameters of egalizatorului vectors f, a, q of the context e */
    int i, j;
    double *adb;
    double log10 = log(10); // it is ln
    double k;
    double prod;
    double adbi_k;

    adb = alloca(e->ncells * sizeof(double));
    /* precalculate amplification in decibels 分贝 for each cell */
    for(i = 0; i < e->ncells; i++)
        adb[i] = 10 * log(e->a[i]) / log10;

    /* global_context c */
    for(j = 0; j < c->size; j++) {
        /* calculate the real k component j depending on the frequency of sampling */
        k = ( j <= c->size / 2 ) ?
            (double) j / c->size * c->wave_fmt.samples_per_sec :
            (double) ( c->size - j ) / c->size * c->wave_fmt.samples_per_sec;
        /* v */
        prod = 1;
        for(i = 0; i < e->ncells; i++) {
            adbi_k = adb[i] * exp( -fabs( log( k / e->f[i] ) ) * e->q[i] );
            prod *= pow(10, adbi_k / 10);
#ifdef gy_debug
            //fprintf(stdout, "%lf\t", adbi_k);// , adb[i]
#endif
        }
#ifdef gy_debug
        //fprintf(stdout, "\n%lf\t%lf\n", k, prod);
        //fprintf(stdout, "\n");
#endif
        /* fprintf(stderr, "%lf\n", prod); */
        e->data[j].r = prod * (1 << COMPLEX_PRECISION);
        e->data[j].i = 0;
    }
}

void print_help(char *arg) {
    fprintf(stderr, "Usage: %s [options]\n\n"
            "Options:\n"
            "\t-i file\t\tRead input data from file (necessary)\n"
            "\t-o file\t\tOutput data to file (necessary)\n"
            //"\t-s\t\tPerform noise spectrum sampling\n"
            "\t-f n\t\tFFT size (in power of 2 i.e. 2^n) (defaults to 4096)\n"
            // "\t-r file\t\tPerform noise reduction with noise spectrum\n"
            // "\t\t\tfrom file\n"
            "\t-e file\t\tPerform parametric equalization with equalizer\n"
            "\t\t\tdata from file\n"
            "\t-d\t\tDummy run (only FFT and iFFT transform)\n"
            "\t-h\t\tThis help\n", arg);
}

int main(int argc, char **argv) {
    struct global_context c;
    /* variables for the data stream */
    FILE *in = NULL; 
    FILE *out = NULL;
    FILE *eq_file = NULL;
    int frame_cnt = 1, frames;
    t_complex *weight;

    /* variables of general use */
    int status;
    int i, j, k, opt;

    /* variables for transformation applied */
    int no_wave_output = 1;
    t_trans_f trans_f = NULL;
    void *trans_ctx = NULL;

    struct equalizer_context equalizer_context;

    /* Buffer data */
    t_complex **fb0, **fb1, **fb_tmp, **fb_base;
    t_complex *fb_aux, *noise_fb_aux;
    char *buf0, *buf1, *buf_pad, *buf_tmp;
    int buf1_len;

    while((opt = getopt(argc, argv, "i:o:fe:t:dh")) != -1) {
        switch(opt) {
        case 'i': /* Input file */
            if((in = fopen(optarg, "r")) == NULL) {
                fprintf(stderr, "Failed opening %s\n", optarg);
                return 15;
            }
            break;
        case 'o': /* Output file */
            if((out = fopen(optarg, "w")) == NULL) {
                fprintf(stderr, "Failed opening %s\n", optarg);
                return 16;
            }
            break;
        case 'f': /* FFT Size */
            if(sscanf(optarg, "%d", &c.n) != 1) {
                fprintf(stderr, "Invalid number for FFT Size\n");
                return 21;
            }
            if(c.n < 8 || c.n > 16) {
                fprintf(stderr, "FFT Size must be between 8 and 16\n");
                return 22;
            }
            break;
        case 'd': /* Dummy run (no transform) 虚拟运行（不改变） */
            trans_f = trans_dummy;
            no_wave_output = 0;
            run_type = other;
            break;
        case 'e': /* Perform Parametric Equalization 执行参数均衡 */
            if((eq_file = fopen(optarg, "r")) == NULL) {
                fprintf(stderr, "Failed opening %s\n", optarg);
                return 18;
            }
            read_eq_cells(&equalizer_context, eq_file);
            fclose(eq_file);
            no_wave_output = 0;
            trans_f = trans_equalizer;
            trans_ctx = &equalizer_context;
            run_type = equalizer;
            break;
        case 't': /* Perform Parametric Tqualization 执行参数均衡 */
            if((eq_file = fopen(optarg, "r")) == NULL) {
                fprintf(stderr, "Failed opening %s\n", optarg);
                return 18;
            }
            read_trans_loss_cells(&equalizer_context, eq_file);
            fclose(eq_file);
            no_wave_output = 0;
            trans_f = trans_equalizer;
            trans_ctx = &equalizer_context;
            run_type = trans;
            break;
        case 'h':
            print_help(argv[0]);
            return 0;
        case '?':
        case ':':
            print_help(argv[0]);
            return 20;
        }
    }

    if(trans_f == NULL || in == NULL || out == NULL) {
        print_help(argv[0]);
        return 30;
    }

    global_context_init(&c);
    if( (status = riff_read(&c, in)) )
        return status;

    // compute_equalizer_data
    if(trans_f == trans_equalizer) {
        equalizer_context.data =(t_complex *)malloc(c.size * sizeof(t_complex));
        assert(equalizer_context.data != NULL);
        switch(run_type){
            case equalizer:
                compute_equalizer_data(&c, &equalizer_context);
                break;
            case trans:
                compute_trans_data(&c, &equalizer_context);
                break;
            default:
                break;
        }
        
    }

    if(no_wave_output) {
        fclose(in);
        fclose(out);
        return 0;
    }

    weight = (t_complex *)malloc(c.size * sizeof(t_complex));
    assert(weight != NULL);

    fb_base = (t_complex **)malloc(2 * c.channels * sizeof(t_complex *));
    assert(fb_base != NULL);
    for(i = 0; i < 2 * c.channels; i++) {
        fb_base[i] = (t_complex *)malloc(c.size * sizeof(t_complex));
        assert(fb_base[i] != NULL);
    }
    fb0 = fb_base;
    fb1 = fb_base + c.channels;
    fb_aux = (t_complex *)malloc(c.size * sizeof(t_complex));
    assert(fb_aux != NULL);

    buf0 = (char *)malloc(c.frame_len / 2);
    assert(buf0 != NULL);
    buf1 = (char *)malloc(c.frame_len / 2);
    assert(buf1 != NULL);
    buf_pad = (char *)malloc(c.frame_len / 2);
    assert(buf_pad != NULL);

    /* initialization tables */
    weight_map(c.n, weight);
    init_fft_maps(&c);

    for(i = c.size / 2 * c.channels, buf_tmp = buf_pad; i;
            i--, buf_tmp += c.bytes_per_sample)
        memcpy(buf_tmp, c.pad_value, c.bytes_per_sample);

    riff_write(&c, out);

    /* the application transformarilor
       the next contact off screen apply in parallel on all channels (because they are enmeshed in the wave), 
       but for simplicity pseudocodul(pseudocode 伪码) will be written for a single channel. all prelucrarile(先驱的) at the level of the frame are in parallel channels.

       the algorithm uses 2 buffers raw of size 2 ^ n / 2 (half frame) and two frame buffers (in complex dimension 2^n).

       algorithm to process a frame additional (supplemented with 0) at the beginning and the end file,
       not to mitigate 减轻 the first, 2 ^ n / 2 respectively the samples.

       the function join (frame0, frame1) overlap the second half of the first frame in the first half of the second frame, using weighting function.       

       buf0 <- 0;
       read(buf1);
       frame1 <- concat(buf0, buf1);
       transform(frame1);
       while(length(buf1) == 2^n / 2) {
           buf0 <- buf1;
           read(buf1); // padding cu 0 daca e < 2^n / 2
           frame0 <- frame1;
           frame1 <- concat(buf0, buf1);
           transform(frame1);
           buf0 <- join(frame0, frame1);
           write(buf0);
       }
       if(length(buf1) > 0) {
           buf0 <- buf1;
           buf1 <- 0;
           frame0 <- frame1;
           frame1 <- concat(buf0, buf1);
           transform(frame1);
           buf0 <- join(frame0, frame1);
           write(buf0);
       } 
    */

    frames = (c.wave_len + c.frame_len - 1) / c.frame_len;

    memcpy(buf0, buf_pad, c.frame_len / 2);
    buf1_len = fread(buf1, 1, c.frame_len / 2, in);
    for(i = 0; i < c.channels; i++) {
        c.complex_promote(c.size / 2, c.interleave,
                buf0 + i * c.bytes_per_sample, fb1[i]);
        c.complex_promote(c.size / 2, c.interleave,
                buf1 + i * c.bytes_per_sample, fb1[i] + c.size / 2);
        
        bit_reverse(c.n, c.rev_map, fb1[i], fb_aux);
        dec_time_fft(c.n, c.w, fb_aux);
        trans_f(trans_ctx, c.n, i, fb_aux);

        bit_reverse(c.n, c.rev_map, fb_aux, fb1[i]);
        dec_time_ifft(c.n, c.w, fb1[i]);
    }
    while(buf1_len == c.frame_len / 2) {
        if(!(frame_cnt % 10)) {
            /* actually i count half frames */
#ifndef gy_debug
            fprintf(stdout, "\rFrame %d/%d (%d%%)", frame_cnt / 2, frames, frame_cnt * 50 / frames);
#endif
            fflush(stderr);
            fflush(stdout);
        }
        frame_cnt++;

        buf_tmp = buf0; buf0 = buf1; buf1 = buf_tmp;
        buf1_len = fread(buf1, 1, c.frame_len / 2, in);
        if(buf1_len < c.frame_len / 2)
            memcpy(buf1 + buf1_len, buf_pad, c.frame_len / 2 - buf1_len);
        fb_tmp = fb0; fb0 = fb1; fb1 = fb_tmp;

        for(i = 0; i < c.channels; i++) {
            
#ifdef gy_debug
    //fprintf(stdout, "%d %d\n", staticI++, *buf1);
#endif

            c.complex_promote(c.size / 2, c.interleave,
                    buf0 + i * c.bytes_per_sample, fb1[i]);
            c.complex_promote(c.size / 2, c.interleave,
                    buf1 + i * c.bytes_per_sample, fb1[i] + c.size / 2);

            bit_reverse(c.n, c.rev_map, fb1[i], fb_aux);
            dec_time_fft(c.n, c.w, fb_aux);
            trans_f(trans_ctx, c.n, i, fb_aux);
            bit_reverse(c.n, c.rev_map, fb_aux, fb1[i]);
            dec_time_ifft(c.n, c.w, fb1[i]);

            for(j = 0; j < c.size / 2; j++) {
                fb_aux[j].r =
                    (fb0[i][c.size / 2 + j].r * weight[c.size / 2 + j].r +
                     fb1[i][j].r * weight[j].r) / (1 << COMPLEX_PRECISION);
            }
            c.complex_reduce(c.size / 2, c.interleave, fb_aux,
                    buf0 + i * c.bytes_per_sample);
        }
#ifdef gy_debug
    //fprintf(stdout, "%d %d %d\n", staticI++, *buf0,c.frame_len / 2);
#endif
        status = fwrite(buf0, c.frame_len / 2, 1, out);
        assert(status);
    }
    if(buf1_len) {
        buf_tmp = buf0; buf0 = buf1; buf1 = buf_tmp;
        memcpy(buf1, buf_pad, c.frame_len / 2);
        fb_tmp = fb0; fb0 = fb1; fb1 = fb_tmp;

        for(i = 0; i < c.channels; i++) {
            c.complex_promote(c.size / 2, c.interleave,
                    buf0 + i * c.bytes_per_sample, fb1[i]);
            c.complex_promote(c.size / 2, c.interleave,
                    buf1 + i * c.bytes_per_sample, fb1[i] + c.size / 2);

            bit_reverse(c.n, c.rev_map, fb1[i], fb_aux);
            dec_time_fft(c.n, c.w, fb_aux);
            trans_f(trans_ctx, c.n, i, fb_aux);
            bit_reverse(c.n, c.rev_map, fb_aux, fb1[i]);
            dec_time_ifft(c.n, c.w, fb1[i]);

            for(j = 0; j < c.size / 2; j++) {
                fb_aux[j].r =
                    (fb0[i][c.size / 2 + j].r * weight[c.size / 2 + j].r +
                     fb1[i][j].r * weight[j].r) / (1 << COMPLEX_PRECISION);
            }
            c.complex_reduce(c.size / 2, c.interleave, fb_aux,
                    buf0 + i * c.bytes_per_sample);
        }
#ifdef gy_debug
    //fprintf(stdout, "%d %d %d \n", staticI++, *buf0,buf1_len);
#endif
        status = fwrite(buf0, buf1_len, 1, out);
        assert(status);
    }

#ifndef gy_debug
    fprintf(stdout, "\rFrame %d/%d (100%%)\n", frames, frames);
#endif
    fflush(stderr);
    fflush(stdout);
 
    /* release memory */
    if(trans_f == trans_equalizer) {
        free(equalizer_context.data);
    }
    free_fft_maps(&c);
    free(weight);
    for(i = 0; i < 2 * c.channels; i++)
        free(fb_base[i]);
    free(fb_base);
    free(fb_aux);
    free(buf0);
    free(buf1);

    fclose(in);
    fclose(out);
    return 0;
}
